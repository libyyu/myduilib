#pragma once
#include "std.h"
#include "Flowrate.h"

class NetService;
typedef boost::shared_ptr<NetService> spNetServiceT;

class SockBase;
typedef SockBase      SockContextT;
typedef boost::shared_ptr<SockContextT>          spSockContextT;


class SockDelegate
{
public:
	virtual void onConnected(SockBase*) = 0;
	virtual void onDisconnected(SockBase*,const int) = 0;
	virtual void onSendCompleted(SockBase*,const void*,const int,size_t) = 0;
	virtual size_t onRecvCompleted(SockBase*,const void*,size_t) = 0;
	virtual void onDestroy(SockBase*) = 0;
};

class SockBase : 
	public Flowrate,
	public SpeedConfig,
	public Tag,
	public boost::asio::ip::tcp::socket,
	public boost::enable_shared_from_this<SockBase>
{
public:
	SockBase(const spNetServiceT&,SockDelegate* = NULL);
	~SockBase();

	spNetServiceT getNetSrv() const;

	bool isTimeOut() const;
	void setTimeOut(size_t millsec);

	void onConnected(const int ec);
	void onSendCompleted(const void* pbuffer,const int ec,size_t bytes);
	size_t onRecvCompleted(const void* pbuffer,const int ec,size_t bytes);

#if BOOST_VERSION >= 107000
	boost::asio::io_context& get_io_service();
#endif

protected:
	void onDisconnect(const int ec);
protected:
	size_t         _timeoutMillSec;

	spNetServiceT  _pNetSrv;
	SockDelegate*  _delegate;
private:
	friend class SendReciver;
	lockT          mu;
	spBufferListT  sendBufferList;
	spBufferT      sendingBuffer;
	size_t         sendindex;
	size_t         nextsendindex;

	spBufferT recvBuffer;
};
