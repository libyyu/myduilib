#pragma once
#include "SockBase.h"

class SendReciver : 
	public Flowrate,
	public SpeedConfig,
	public boost::enable_shared_from_this<SendReciver>
{
public:
	static const size_t BUFFER_SIZE = 1024*4;
	SendReciver(spNetServiceT&);
	~SendReciver();

	typedef boost::function<bool (void)> ExitWaitPred;
	static bool waitReadable(const spSockContextT&,size_t = 10,ExitWaitPred = NULL);
	static bool waitWriteable(const spSockContextT&,size_t = 10,ExitWaitPred = NULL);

	void addSock(const spSockContextT&);
	void removeSock(const spSockContextT&);

	bool asynSend(const spSockContextT&,const void*,size_t);
	bool asynSend(const spSockContextT&,const spBufferT&,emBufferT =BUFFER_NORMAL);
	bool asynRecv(const spSockContextT&);
protected:
	void onAddSock(spSockContextT&);
	void onRemoveSock(spSockContextT&);
	void onTimer(const errcodeT&);

	void onAsynSend(spSockContextT&);
	bool doSend(spSockContextT&,spBufferT&);
	void checkSend();
	void onSendTimer(const errcodeT&);

	void onAsynRecv(spSockContextT&);
	bool doRecv(spSockContextT&);
	void checkRecv();
	void onRecvTimer(const errcodeT&);

	void onSendCompleted(spSockContextT&,const errcodeT&,size_t);
	void onRecvCompleted(spSockContextT&,const size_t,const errcodeT&,size_t);
	void onRecvCompleted(spSockContextT&,const errcodeT& ,size_t);
protected:
	typedef boost::unordered_set<spSockContextT> sockSetT;
	typedef sockSetT::iterator sockSetItetT;
	typedef boost::unordered_map<spSockContextT,spBufferT> bufferMapT;
	typedef bufferMapT::iterator   bufferMapIterT;

	sockSetT           _socks;
	sptimerT           _timer;
	strandT            _timerStrand;

	sockSetT           _sendBuffers;
	timerT             _sendTimer;
	strandT            _sendStrand;

	sockSetT           _recvBuffers;
	timerT             _recvTimer;
	strandT            _recvStrand;

	volatile int64    _sendcnt;
	volatile int64    _recvcnt;
};
