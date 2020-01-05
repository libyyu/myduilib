#pragma once
#include "SockBase.h"
#include "IoServicePool.h"

class SendReciver;
class Accepter;
typedef boost::shared_ptr<SendReciver> spSendReciverT;
typedef boost::shared_ptr<Accepter>  spAccepterT;
typedef SockContextT   clientT;
typedef spSockContextT spClientT;

class NetService : 
	public boost::enable_shared_from_this<NetService>
{
public:
	static bool Init(size_t ioSrvCnt,size_t threadCntPreIoSrv);
	static boost::shared_ptr<NetService> Instance();
public:
	explicit NetService(size_t ioSrvCnt,size_t threadCntPreIoSrv);
	~NetService();

	void run(bool asyn = false);
	void stop();

	ioSrvT& getIoSrv();

	spSendReciverT getSendReciver() const;
	void setSendReciver(spSendReciverT&);
	
	void addClient(const spClientT&);
	void removeClient(const spClientT&);

	void addAccepter(const spAccepterT&);
	void removeAccepter(const spAccepterT&);

protected:
	IoServicePool   _ioSrvPool;
	spSendReciverT  _sendReciver;

	typedef boost::unordered_set<spClientT> clientsSetT;
	typedef clientsSetT::iterator clientsSetIterT;
	typedef boost::unordered_set<spAccepterT> epsSetT;
	typedef epsSetT::iterator     epsSetIterT;

	lockT         _mu;
	clientsSetT   _clients;
	epsSetT       _eps;

	static boost::shared_ptr<NetService> _theNetService;
};

#define theSrv (NetService::Instance())