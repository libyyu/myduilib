#include "SockBase.h"
#include "NetService.h"
#include "SendReciver.h"


SockBase::SockBase(const spNetServiceT& netSrv,SockDelegate* del/* = NULL */)
	:boost::asio::ip::tcp::socket(netSrv->getIoSrv()),
	_pNetSrv(netSrv),
	_delegate(del),
	_timeoutMillSec(1000*60)
{

}

SockBase::~SockBase()
{
	if(_delegate)
		_delegate->onDestroy(this);
	_delegate = NULL;
}
spNetServiceT SockBase::getNetSrv() const
{
	return _pNetSrv;
}

bool SockBase::isTimeOut() const
{
	if(0 == _timeoutMillSec)
		return false;
	return is_timeout(_timeoutMillSec);
}
void SockBase::setTimeOut(size_t millsec)
{
	_timeoutMillSec = millsec;
}
void SockBase::onConnected(const int ec)
{
	if(!ec)
	{
		spSendReciverT theSendReciver(_pNetSrv->getSendReciver());
		if(theSendReciver)
		{
			theSendReciver->addSock(shared_from_this());
			theSendReciver->asynRecv(shared_from_this());
		}
	}

	if(_delegate)
	{
		if(!ec)
			return _delegate->onConnected(this);
		else
			return onDisconnect(ec);
	}	
}
void SockBase::onSendCompleted(const void* pbuffer,const int ec,size_t bytes)
{
// 	if(!ec)
// 		put_write_bytes(bytes);

	if(_delegate)
	{
		return _delegate->onSendCompleted(this,pbuffer,ec,bytes);
	}
}
size_t SockBase::onRecvCompleted(const void* pbuffer,const int ec,size_t bytes)
{
// 	if(!ec)
// 		put_read_bytes(bytes);

	if(!ec)
	{
		if(_delegate)
			return _delegate->onRecvCompleted(this,pbuffer,bytes);
	}
	else
	{
		spSendReciverT theSendReciver(_pNetSrv->getSendReciver());
		if(theSendReciver)
			theSendReciver->removeSock(shared_from_this());
		onDisconnect(ec);
	}

	return bytes;
}
void SockBase::onDisconnect(const int ec)
{
	if(is_open())
	{
		errcodeT e;
		close(e);
	}
	if (_delegate)
	{
		return _delegate->onDisconnected(this,ec);
	}
}

#if BOOST_VERSION >= 107000
boost::asio::io_context& SockBase::get_io_service()
{
	return _pNetSrv->getIoSrv();
}
#endif
