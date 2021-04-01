#ifndef __FLIB_SOCKET_H__
#define __FLIB_SOCKET_H__
#pragma once

#include "FType.h"
#include <ctype.h>
#include <functional>
#include <memory>
#if FLIB_COMPILER_WINDOWS
#if FLIB_COMPILER_64BITS
#define F_SOCKET unsigned __int64
#else
#define F_SOCKET unsigned int
#endif
#define F_SOCK_STREAM    1
#define F_INVALID_SOCKET (F_SOCKET)(~0)
#define F_SOCKET_ERROR   (F_SOCKET)(~0)
#else
#define F_SOCKET            int
#define F_SOCK_STREAM       1
#define F_INVALID_SOCKET   -1
#define F_SOCKET_ERROR     -1
#endif

#define F_SOCKET_STARTUP 	_FStd(FSocketStartup)();
#define F_SOCKET_CLEANUP    _FStd(FSocketCleanup)();
#define F_ERRNO             _FStd(FGetNetLastError)()

_FStdBegin

F_DLL_API int FGetNetLastError();
F_DLL_API int FSocketStartup();
F_DLL_API int FSocketCleanup();

class F_DLL_API FSockAddr
{
	friend class CSockAddrImpl;
	class CSockAddrImpl* _impl;
public:
	FSockAddr();
	FSockAddr(unsigned short uport, const char* ip = NULL);
	~FSockAddr();
	void* SockAddr() const;
};

class F_DLL_API FSocket
{
public:
    typedef std::function<bool(void)> ExitWaitPred;
	FSocket(F_SOCKET s = F_INVALID_SOCKET);
	virtual ~FSocket();
public:
	operator F_SOCKET() const;

    bool Ioctl(long cmd, unsigned long* argp);
    void SetKeepAlive(bool on); 
    void SetTcpNoDelay(bool on);

	bool IsCreate() const;
	bool Create(int ntype = F_SOCK_STREAM);
	bool Close();
	bool Bind(const FSockAddr* addr);
    bool Listen(int nbacklog = 5);
	F_SOCKET Accept(FSockAddr* addr = NULL);
    bool IsConnect() const;
	bool Connect(const FSockAddr* pRemoteaddr);
	bool ConnectEx(const FSockAddr* pRemoteaddr,int nTimeOut = 10,ExitWaitPred isExitWaitPred = nullptr);

    bool WaitReadable(unsigned int nTimeOut = 10,ExitWaitPred isExitWaitPred = nullptr);
	bool WaitWriteable(unsigned int nTimeOut = 10,ExitWaitPred isExitWaitPred = nullptr);

	int Recv(char* buf,size_t cnt);
	int Send(char* buf,size_t cnt);

	int RecvFrom(char* buf,size_t cnt,FSockAddr* pRemoteaddr = NULL);
	int SendTo(char* buf,size_t cnt,FSockAddr* pRemoteaddr = NULL);
protected:
	F_SOCKET    _s;
};

_FStdEnd


#endif//__FLIB_SOCKET_H__