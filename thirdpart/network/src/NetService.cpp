#include "NetService.h"
#include "SendReciver.h"
#include "Accepter.h"

boost::shared_ptr<NetService> NetService::_theNetService;
bool NetService::Init(size_t ioSrvCnt,size_t threadCntPreIoSrv)
{
	if(!_theNetService)
	{
		boost::shared_ptr<NetService> theNet(new NetService(ioSrvCnt,threadCntPreIoSrv));
		spSendReciverT  sendReciver(new SendReciver(theNet));
		theNet->setSendReciver(sendReciver);

		_theNetService = theNet;

		return true;
	}
	return false;
}
boost::shared_ptr<NetService> NetService::Instance()
{
	if(!_theNetService)
		Init(0,0);
	return _theNetService;
}

NetService::NetService(size_t ioSrvCnt,size_t threadCntPreIoSrv)
	:_ioSrvPool(ioSrvCnt,threadCntPreIoSrv)
{

}

NetService::~NetService()
{

}
void NetService::run(bool asyn /* = false */)
{
	_ioSrvPool.run(asyn);
}

void NetService::stop()
{
	_ioSrvPool.stop();
}

ioSrvT& NetService::getIoSrv()
{
	return _ioSrvPool.getIoSrv();
}

spSendReciverT NetService::getSendReciver() const
{
	return _sendReciver;
}
void NetService::setSendReciver(spSendReciverT& s)
{
	_sendReciver = s;
}

void NetService::addClient(const spClientT& pclient)
{
	assert(pclient);
	lockerT locker(_mu);
	_clients.insert(pclient);
}
void NetService::removeClient(const spClientT& pclient)
{
	assert(pclient);
	lockerT locker(_mu);
	_clients.erase(pclient);
}

void NetService::addAccepter(const spAccepterT& ep)
{
	assert(ep);
	lockerT locker(_mu);
	_eps.insert(ep);
}

void NetService::removeAccepter(const spAccepterT& ep)
{
	assert(ep);
	lockerT locker(_mu);
	_eps.erase(ep);
}