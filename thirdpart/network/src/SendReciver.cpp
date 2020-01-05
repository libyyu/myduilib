#include "SendReciver.h"
#include "flib.h"
#include "NetService.h"
//#include <WinSock2.h>

bool SendReciver::waitReadable(const spSockContextT& s,size_t nTimeOut /* = 10 */,ExitWaitPred isExitWait/* = NULL */)
{
	assert(s);
	assert(s->is_open());
	if(!s || !s->is_open())
		return false;
	
	if(0 >= nTimeOut)
		nTimeOut = 1;
	while(nTimeOut-- && !(isExitWait ? isExitWait() : false))
	{
		fd_set      fdRead;
		timeval     tv;
		FD_ZERO(&fdRead);
		FD_SET(s->native_handle(),&fdRead);
		tv.tv_sec  = 1;
		tv.tv_usec = 0;
#ifdef _MSC_VER
		int nRet = ::select(0,&fdRead,NULL,NULL,&tv);
#else
		int nRet = ::select(s->native_handle()+1,&fdRead,NULL,NULL,&tv);
#endif
		if(0 < nRet)
			return true;
		if(0 > nRet)
			break;
	}
	return false;
}
bool SendReciver::waitWriteable(const spSockContextT& s,size_t nTimeOut/* = 10 */,ExitWaitPred isExitWait/* = NULL */)
{
	assert(s);
	assert(s->is_open());
	if(!s || !s->is_open())
		return false;
	if(0 >= nTimeOut)
		nTimeOut = 1;
	while(nTimeOut-- && !(isExitWait ? isExitWait() : false))
	{
		fd_set      fdWrite;
		timeval     tv;
		FD_ZERO(&fdWrite);
		FD_SET(s->native_handle(),&fdWrite);
		tv.tv_sec  = 1;
		tv.tv_usec = 0;
#ifdef _MSC_VER
		int nRet = ::select(0,NULL,&fdWrite,NULL,&tv);
#else
		int nRet = ::select(s->native_handle()+1,NULL,&fdWrite,NULL,&tv);
#endif
		if(0 < nRet)
			return true;
		if(0 > nRet)
			break;
	}

	return false;
}

SendReciver::SendReciver(spNetServiceT& netSrv)
	:_timerStrand(netSrv->getIoSrv()),
    _sendStrand(netSrv->getIoSrv()),
	_sendTimer(netSrv->getIoSrv()),
	_recvStrand(netSrv->getIoSrv()),
	_recvTimer(netSrv->getIoSrv()),
	_sendcnt(0),
	_recvcnt(0)
{
	
}

SendReciver::~SendReciver()
{

}

void SendReciver::addSock(const spSockContextT& s)
{
	assert(s);
	assert(s->is_open());
	if(s && s->is_open())
		_timerStrand.post(boost::bind(&SendReciver::onAddSock,shared_from_this(),s));
}
void SendReciver::removeSock(const spSockContextT& s)
{
	assert(s);
	if(s)
		_timerStrand.post(boost::bind(&SendReciver::onRemoveSock,shared_from_this(),s));
}


void SendReciver::onAddSock(spSockContextT& s)
{
	assert(s);
	s->clear();
	s->put_write_bytes(0);
	s->put_read_bytes(0);
	_socks.insert(s);
	if(!_timer)
	{
#if BOOST_VERSION >= 107000
		_timer = sptimerT(new timerT(_timerStrand.context()));
#else
		_timer = sptimerT(new timerT(_timerStrand.get_io_service()));
#endif
		_timer->expires_from_now(boost::posix_time::seconds(1));
		_timer->async_wait(_timerStrand.wrap(boost::bind(&SendReciver::onTimer,shared_from_this(),_1)));
	}
}

void SendReciver::onRemoveSock(spSockContextT& s)
{
	_socks.erase(s);
}
void SendReciver::onTimer(const errcodeT& ec)
{
	if(!ec)
	{
		for (auto it = _socks.begin(), iend = _socks.end(); it != iend;)
		{
			(*it)->update();
			if((*it)->isTimeOut() && (*it)->is_open())
			{
				errcodeT ec;
				(*it)->close(ec);
				printf("timeout\n");
			}

			if(!(*it)->is_open())
			{
				_socks.erase(it++);
			}
			else
				++it;
		}

#define SPEEDRATE  (1.0f/(1024))
		
		printf("link:%lu\tio:%lld,%lld\tspeed:%.3f\t%.3f\r",
			   _socks.size(),
			   _sendcnt,
			   _recvcnt,
			   (this->get_read_bytes_pre_sec()*SPEEDRATE),
			   (this->get_write_bytes_pre_sec()*SPEEDRATE)
			   );

		this->update();

		if(!_socks.empty())
		{
			_timer->expires_from_now(boost::posix_time::seconds(1));
			_timer->async_wait(_timerStrand.wrap(boost::bind(&SendReciver::onTimer,shared_from_this(),_1)));
		}
		else
		{
			_timer.reset();
		}
	}
}

bool SendReciver::asynSend(const spSockContextT& s,const void* pdata,size_t bytes)
{
	assert(s);
	assert(s->is_open());
	assert(pdata);
	assert(bytes);

	if(!s || !pdata || !bytes || !s->is_open())
		return false;

	static const size_t n = (BUFFER_SIZE -1);
	const size_t bufferSize =  (((bytes)+n) &~n);

	bool bNotify = false;
	{
		lockerT locker(s->mu);
		if(!s->sendingBuffer)
		{
			s->sendingBuffer = spBufferT(new _FStd(FBuffer)(bufferSize));
			s->sendingBuffer->Write((uint8*)pdata,bytes);
			s->sendindex     = s->sendingBuffer->rpos();
			s->nextsendindex = s->sendindex;
			bNotify          = true;
		}
		else
		{
			bool bMerge = false;
			if(!s->sendBufferList.empty())
			{
				auto it = s->sendBufferList.rbegin();
				if(BUFFER_NORMAL == it->first && it->second->nextwriteblocksize() >= bytes)
				{
					it->second->Write((uint8*)pdata,bytes);
					bMerge = true;
				}
			}

			if(!bMerge)
			{
				spBufferT buffer(new _FStd(FBuffer)(bufferSize));
				buffer->Write((uint8*)pdata,bytes);
				s->sendBufferList.push_back(std::make_pair(BUFFER_NORMAL,buffer));
			}
		}
	}

	if(bNotify)
		_sendStrand.post(boost::bind(&SendReciver::onAsynSend,shared_from_this(),s));
	return true;
}
bool SendReciver::asynSend(const spSockContextT& s, const spBufferT& buffer,emBufferT bufferType)
{
	assert(s);
	assert(s->is_open());
	assert(buffer);
	assert(!buffer->empty());
	assert(buffer->nextreadblocksize() == buffer->cnt());

	if(!s || !buffer || buffer->empty() || !s->is_open())
		return false;

	bool bNotify = false;
	{
		lockerT locker(s->mu);
		if (!s->sendingBuffer)
		{
			s->sendingBuffer = buffer;
			s->sendindex     = s->sendingBuffer->rpos();
			s->nextsendindex = s->sendindex;
			bNotify          = true;
		}
		else
		{
			bool bMerge = false;
			if(!s->sendBufferList.empty() && (BUFFER_NORMAL == bufferType))
			{
				auto it = s->sendBufferList.rbegin();
				if(BUFFER_NORMAL == it->first && it->second->nextwriteblocksize() >= buffer->cnt())
				{
					(*(it->second))<<(*buffer);
					bMerge = true;
				}
			}

			if(!bMerge)
				s->sendBufferList.push_back(std::make_pair(bufferType,buffer));
		}
	}
	if(bNotify)
		_sendStrand.post(boost::bind(&SendReciver::onAsynSend,shared_from_this(),s));
	return true;
}
void SendReciver::onAsynSend(spSockContextT& s)
{
	assert(s);
	assert(s->sendingBuffer);
	assert(!s->sendingBuffer->empty());
	assert(s->sendingBuffer->nextreadblocksize() == s->sendingBuffer->cnt());
	assert(s->sendingBuffer->rpos() == s->sendindex);
	assert(s->sendindex == s->nextsendindex);
	
	spBufferT theBuffer(s->sendingBuffer);
	//doSend(s,theBuffer);
	//checkSend();

	if(!doSend(s,theBuffer))
	{
		bool bTimer	= _sendBuffers.empty();
		_sendBuffers.insert(s);
		if(bTimer)
		{
			_sendTimer.expires_from_now(boost::posix_time::seconds(1));
			_sendTimer.async_wait(_sendStrand.wrap(boost::bind(&SendReciver::onSendTimer,shared_from_this(),_1)));
		}
	}
}
bool SendReciver::doSend(spSockContextT& s,spBufferT& thebuffer)
{
	assert(s);
	assert(thebuffer);
	assert(!thebuffer->empty());
	assert(s->sendingBuffer->nextreadblocksize() == s->sendingBuffer->cnt());
	assert(s->nextsendindex < thebuffer->cnt());
	assert(s->sendindex < thebuffer->cnt());
	assert(s->nextsendindex >= s->sendindex);
	bool bOver = false;
	if(thebuffer && !thebuffer->empty() && s->nextsendindex == s->sendindex)
	{
		const uint64 nMaxTotalSendBytesPreSec = this->get_max_send_bytes_pre_sec();
		const uint64 nMaxSendBytesPreSec      = s->get_max_send_bytes_pre_sec();

		size_t bytes = 0;
		if(0 == nMaxTotalSendBytesPreSec && 0 == nMaxSendBytesPreSec)
		{
			bytes = thebuffer->cnt() - s->sendindex; 
			assert(bytes);
		}
		else
		{
			const uint64 retainbytes = std::max<int64>(0,((0 != nMaxTotalSendBytesPreSec) ? (nMaxTotalSendBytesPreSec - this->get_write_bytes_pre_sec()) : (nMaxSendBytesPreSec - s->get_write_bytes_pre_sec())));
			bytes = std::min<size_t>((size_t)retainbytes,(thebuffer->cnt() - s->sendindex));
		}	

		if(bytes > 0)
		{	
			++_sendcnt;
			
			// 				const size_t index = thebuffer->rpos();
			// 				thebuffer->rpos((int)bytes);
			s->nextsendindex += bytes;
			s->put_write_bytes(bytes);
			this->put_write_bytes(bytes);

			bOver = (s->nextsendindex >= thebuffer->cnt());
			s->async_send(
				boost::asio::buffer((*thebuffer)[s->sendindex],bytes),
				boost::bind(&SendReciver::onSendCompleted,shared_from_this(),s,_1,_2)
				);	
		}
	}

	return bOver;
}
void SendReciver::checkSend()
{
	for (sockSetItetT it = _sendBuffers.begin(),end = _sendBuffers.end(); it != end;)
	{
		spSockContextT s(*it);
		spBufferT thebuffer(s->sendingBuffer);
		if(doSend(s,thebuffer))
			_sendBuffers.erase(it++);
		else
			++it;
	}
}
void SendReciver::onSendTimer(const errcodeT& ec)
{
	if(!ec)
	{
		checkSend();
		if(!_sendBuffers.empty())
		{
			_sendTimer.expires_from_now(boost::posix_time::seconds(1));
			_sendTimer.async_wait(_sendStrand.wrap(boost::bind(&SendReciver::onSendTimer,shared_from_this(),_1)));
		}
	}
}

void SendReciver::onSendCompleted(spSockContextT& s,const errcodeT& ec,size_t bytes)
{
	assert(s);
	assert(s->sendingBuffer);
	assert(s->nextsendindex > s->sendindex);

	--_sendcnt;

	const size_t needsendbytes = (s->nextsendindex - s->sendindex);
	if(ec)
	{
		s->pop_write_bytes(needsendbytes);
		this->pop_write_bytes(needsendbytes);
		std::cout<<"send error:"<<ec.message()<<std::endl;
	}
	else
	{
		assert(needsendbytes == bytes);
	}
	s->onSendCompleted((*(s->sendingBuffer))[s->sendindex],ec.value(),bytes);

	{
		s->sendindex = s->nextsendindex; //?????????

		bool bNotify = false;
		if(s->sendindex >= s->sendingBuffer->cnt())
		{
			lockerT locker(s->mu);
			if(!s->sendBufferList.empty())
			{
				s->sendingBuffer = s->sendBufferList.rbegin()->second;
				s->sendindex     = s->sendingBuffer->rpos();
				s->nextsendindex = s->sendindex;
				s->sendBufferList.pop_front();
			}
			else
			{
				s->sendingBuffer = spBufferT();
				s->sendindex     = 0;
				s->nextsendindex = s->sendindex;
			}

			bNotify = (s->sendingBuffer && !s->sendingBuffer->empty());
		}
		if(bNotify)
			_sendStrand.post(boost::bind(&SendReciver::onAsynSend,shared_from_this(),s));
	}
}

bool SendReciver::asynRecv(const spSockContextT& s)
{
	assert(s);
	if(!s)
		return false;
	s->async_receive(boost::asio::null_buffers(),boost::bind(&SendReciver::onRecvCompleted,shared_from_this(),s,_1,_2));
	//_recvStrand.post(boost::bind(&SendReciver::onAsynRecv,shared_from_this(),s,pbuffer));
	return true;
}

void SendReciver::onAsynRecv(spSockContextT& s)
{
	assert(s);

	//checkRecv();
	if(!doRecv(s))
	{
		_recvBuffers.insert(s);
		if(_recvBuffers.size() > 1)
		{
			_recvTimer.expires_from_now(boost::posix_time::seconds(1));
			_recvTimer.async_wait(_recvStrand.wrap(boost::bind(&SendReciver::onRecvTimer,shared_from_this(),_1)));
		}
	}
}
bool SendReciver::doRecv(spSockContextT& s)
{
	assert(s);
//	assert(s->available());
	if(!s->recvBuffer)
		s->recvBuffer = spBufferT(new _FStd(FBuffer)());
	spBufferT thebuffer(s->recvBuffer);
	if(thebuffer->full())
		thebuffer->resize();
	if(thebuffer->empty())
		thebuffer->clear();

	assert(thebuffer);
	assert(thebuffer->nextwriteblocksize());

	errcodeT e;
	const size_t availBytes = std::max<size_t>(1,s->available(e));
	

	assert(availBytes);
	const uint64 nMaxTotalRecvBytesPreSec = this->get_max_recv_bytes_pre_sec();
	const uint64 nMaxRecvBytesPreSec      = s->get_max_recv_bytes_pre_sec();

	if(0 == nMaxTotalRecvBytesPreSec && 0 == nMaxRecvBytesPreSec)
	{
		++_recvcnt;

		const size_t bytes = std::min(thebuffer->nextwriteblocksize(),availBytes);
		this->put_read_bytes(bytes);
		s->put_read_bytes(bytes);

		s->async_receive(
			boost::asio::buffer((*thebuffer)[thebuffer->wpos()],bytes),
			boost::bind(&SendReciver::onRecvCompleted,shared_from_this(),s,bytes,_1,_2)
			);
	}
	else
	{
		const int64 retainbytes = ((0 !=nMaxTotalRecvBytesPreSec) ? (nMaxTotalRecvBytesPreSec - this->get_read_bytes_pre_sec()) : (nMaxRecvBytesPreSec - s->get_read_bytes_pre_sec()));
		const size_t bytes = std::min(availBytes,std::min<size_t>((size_t)retainbytes,thebuffer->nextwriteblocksize()));
		if (bytes > 0)
		{	
			assert(bytes);
			++_recvcnt;

			this->put_read_bytes(bytes);
			s->put_read_bytes(bytes);
			
			s->async_receive(
				boost::asio::buffer((*thebuffer)[thebuffer->wpos()],bytes),
				boost::bind(&SendReciver::onRecvCompleted,shared_from_this(),s,bytes,_1,_2)
				);
		}
		else
			return assert(false),false;
	}
	return true;
}
void SendReciver::checkRecv()
{
	for (sockSetItetT it = _recvBuffers.begin(),end = _recvBuffers.end(); it != end;)
	{
		spSockContextT s(*it);
		if(doRecv(s))
			_recvBuffers.erase(it++);
		else
			++it;
	}
}
void SendReciver::onRecvTimer(const errcodeT& ec)
{
	if(!ec)
	{
		checkRecv();
		if(!_recvBuffers.empty())
		{
			_recvTimer.expires_from_now(boost::posix_time::seconds(1));
			_recvTimer.async_wait(_recvStrand.wrap(boost::bind(&SendReciver::onRecvTimer,shared_from_this(),_1)));
		}
	}
}


void SendReciver::onRecvCompleted(spSockContextT& s,const size_t needrecvbytes,const errcodeT& ec,size_t bytes)
{
	assert(s);
	--_recvcnt;

	spBufferT thebuffer(s->recvBuffer);
	if (!ec)
	{
		assert(s->recvBuffer);
		assert(bytes);
		assert(needrecvbytes >= bytes);
		assert(thebuffer->nextwriteblocksize() >= bytes);
		thebuffer->wpos((int)bytes);
		assert(!thebuffer->empty());

		
		if(!thebuffer->empty())
		{
			if(thebuffer->nextreadblocksize() != thebuffer->cnt())
				thebuffer->resize(thebuffer->size());

			size_t cnt = 0;
			do
			{
				cnt = s->onRecvCompleted((*thebuffer)[thebuffer->rpos()],ec.value(),thebuffer->nextreadblocksize());
				assert(cnt <= thebuffer->cnt());
				thebuffer->rpos((int)cnt);
			}while(cnt > 0 && !thebuffer->empty());
		}
	}
	else
	{
		//std::cout<<"recv error:"<<ec.value()<<":"<<ec.message()<<std::endl;
		s->pop_read_bytes(needrecvbytes);
		this->pop_read_bytes(needrecvbytes);
		s->onRecvCompleted(NULL,ec.value(),bytes);
		s->recvBuffer = spBufferT();
	}
	if(!ec)
	{
		errcodeT e;
		size_t cnt = s->available(e);
		if(cnt > 0 && !e)
			_recvStrand.post(boost::bind(&SendReciver::onAsynRecv,shared_from_this(),s));
		else
			this->asynRecv(s);
	}
}
void SendReciver::onRecvCompleted(spSockContextT& s,const errcodeT& ec, size_t bytes)
{
	assert(bytes == 0);
	
	if(!ec)
	{
		errcodeT e;
		size_t cnt = s->available(e);
		if(cnt > 0 && !e)
			_recvStrand.post(boost::bind(&SendReciver::onAsynRecv,shared_from_this(),s));
		else
			s->onRecvCompleted(NULL,boost::asio::error::eof,bytes);
	}
	else
	{
		s->onRecvCompleted(NULL,ec.value(),bytes);
	}
}
