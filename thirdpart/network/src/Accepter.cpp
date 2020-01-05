#include "Accepter.h"
#include <limits>
#include "NetService.h"
#include "SendReciver.h"

Accepter::Accepter(const spNetServiceT& netSrv,AccepterDelegate* del,unsigned int clientTimeoutMillsec /* = 1000*10 */,unsigned int nMaxClientCnt /* = 1024*10 */)
	:_netSrv(netSrv),
	_delegate(del),
	_clinetTimeoutMillsec(clientTimeoutMillsec),
	_maxClinetCnt(nMaxClientCnt)
{

}
Accepter::~Accepter()
{
	close();
	onDestroy(NULL);
	_delegate = NULL;
}

bool Accepter::listen(unsigned int uport,const char* ip /* = NULL */,bool reuseAddr /* = true */)
{
	if(_ep)
		return false;

	if(!ip || !strlen(ip))
		ip = "0.0.0.0";
	errcodeT ec;
	boost::asio::ip::address addr = boost::asio::ip::address::from_string(ip,ec);
	assert(!ec);
	if(ec) return false;
	endpointT endpt(addr,uport);
	spAcceptorT ep(new acceptorT(_netSrv->getIoSrv()));
	ep->open(endpt.protocol(),ec);
	if(ec) return false;
	if(reuseAddr)
	{
		ep->set_option(boost::asio::socket_base::reuse_address(reuseAddr),ec);
		if(ec) return false;
	}
	ep->bind(endpt,ec);
	if(ec) return false;

	size_t nMaxConcurrent = 10;
	if(_maxClinetCnt)
	{
		if(_maxClinetCnt > nMaxConcurrent)
			nMaxConcurrent = std::max<size_t>(_maxClinetCnt/10,nMaxConcurrent);
		else
			nMaxConcurrent = _maxClinetCnt;
	}
	
	nMaxConcurrent = std::min<size_t>(1024,nMaxConcurrent);
	if(!ep->listen(boost::asio::socket_base::max_connections,ec))
	{
		_ep = ep;
		while(nMaxConcurrent--)
			doAccept();
	}
	return !!_ep;
}
void Accepter::close()
{
	spAcceptorT ep(_ep);
	_ep.reset();
	if(ep)
	{
		errcodeT ec;
		ep->close(ec);
	}

	typedef boost::weak_ptr<clientT> wpClientT;
	typedef std::set<wpClientT> wpClientSetT;

	wpClientSetT socks;
	{
		lockerT locker(_mu);
		for (clientSetItetT it = _clinets.begin(),end =_clinets.end();it != end;++it)
			socks.insert(wpClientT(*it));
	}
	errcodeT ec;
	for (auto it = socks.begin(),end = socks.end();it != end;++it)
	{
		spClientT theclient((*it).lock());
		if(theclient) theclient->close(ec);
	}

	do 
	{
		boost::this_thread::sleep(boost::posix_time::seconds(1));
		bool bfind = false;
		for (auto it = socks.begin(),end = socks.end();it != end;++it)
		{
			if((*it).lock())
			{
				bfind = true;
				break;
			}
		}

		if(!bfind)
			break;

	} while (true);
}
bool Accepter::asynAllClient(const void* pdata,size_t bytes)
{
	assert(pdata);
	assert(bytes);
	if(!pdata || !bytes)
		return false;

	spSendReciverT sender(_netSrv->getSendReciver());
	assert(sender);
	if(!sender) return false;

	std::vector<spClientT> clientsArray;
	{
		lockerT locker(_mu);
		std::copy(_clinets.begin(),_clinets.end(),std::back_inserter(clientsArray));
	}

	if(clientsArray.empty())
		return false;
	spBufferT buffer(new _FStd(FBuffer)(bytes));
	buffer->Write((uint8*)pdata,bytes);
	for (auto it = clientsArray.begin(),end = clientsArray.end(); it != end;++it)
		sender->asynSend(*it,buffer,BUFFER_SHARED);
	return true;
}
void Accepter::doAccept()
{
	spAcceptorT ep(_ep);
	if(ep && ep->is_open())
	{
		spClientT pClient(new clientT(_netSrv,this));
		pClient->setTimeOut(_clinetTimeoutMillsec);

		ep->async_accept(*pClient,boost::bind(&Accepter::onAccept,shared_from_this(),pClient,_1));
	}
}
void Accepter::onAccept(spClientT& pclient,const errcodeT& ec)
{
	doAccept();
	if(!ec)
	{
		{
			lockerT locker(_mu);
			if(0 != _maxClinetCnt && _clinets.size() >= _maxClinetCnt)
				return pclient->close();
			else
				_clinets.insert(pclient);
		}

		pclient->onConnected(ec.value());
	}
}
void Accepter::onConnected(SockBase* pclient)
{	
	if(_delegate)
		_delegate->onConnected(this,pclient);
}
void Accepter::onDisconnected(SockBase* pclient,const int ec)
{
	{
		spClientT theClient(pclient->shared_from_this());
		assert(theClient);
		lockerT locker(_mu);
		_clinets.erase(theClient);
	//	_strand.post(boost::bind(&Accepter::onRemove,shared_from_this(),theClient));
	}
	if(_delegate)
		_delegate->onDisconnected(this,pclient,ec);
}
void Accepter::onSendCompleted(SockBase* pclient,const void* pbuffer,const int ec,size_t bytes)
{
	if(_delegate)
		_delegate->onSendCompleted(this,pclient,pbuffer,ec,bytes);
}
size_t Accepter::onRecvCompleted(SockBase* pclient,const void* pbuffer,size_t bytes)
{
	if(_delegate)
		return _delegate->onRecvCompleted(this,pclient,pbuffer,bytes);
	return bytes;
}
void Accepter::onDestroy(SockBase* pclient)
{
	if(_delegate)
		return _delegate->onDestroy(this,pclient);
}
