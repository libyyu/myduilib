#pragma once
#include "SockBase.h"

class Accepter;
class AccepterDelegate
{
public:
	virtual void onConnected(Accepter*,SockBase*) = 0;
	virtual void onDisconnected(Accepter*,SockBase*,const int) = 0;
	virtual void onSendCompleted(Accepter*,SockBase*,const void*,const int,size_t) = 0;
	virtual size_t onRecvCompleted(Accepter*,SockBase*,const void*,size_t) = 0;
	virtual void onDestroy(Accepter*,SockBase*) = 0;
};

class Accepter :
	public SockDelegate,
	public boost::enable_shared_from_this<Accepter>
{
public:
	typedef SockContextT   clientT;
	typedef spSockContextT spClientT;
	Accepter(const spNetServiceT&,AccepterDelegate* = NULL,unsigned int clientTimeoutMillsec = 1000*10,unsigned int nMaxClientCnt = 1024*10);
	~Accepter();

	bool listen(unsigned int uport,const char* ip = NULL,bool reuseAddr = true);
	void close();

	bool asynAllClient(const void* pdata,size_t bytes);
public:
	virtual void onConnected(SockBase*);
	virtual void onDisconnected(SockBase*,const int);
	virtual void onSendCompleted(SockBase*,const void*,const int,size_t);
	virtual size_t onRecvCompleted(SockBase*,const void*,size_t);
	virtual void onDestroy(SockBase*);
protected:
	void doAccept();
	void onAccept(spClientT&,const errcodeT&);
protected:
	typedef boost::asio::ip::tcp::acceptor acceptorT;
	typedef boost::shared_ptr<acceptorT> spAcceptorT;

	typedef boost::unordered_set<spClientT>  clientSetT;
	typedef clientSetT::iterator             clientSetItetT;

	spNetServiceT     _netSrv;
	spAcceptorT       _ep;
	unsigned int      _clinetTimeoutMillsec;
	unsigned int      _maxClinetCnt;

	lockT             _mu;
	clientSetT        _clinets;

	AccepterDelegate* _delegate;
};
