#ifndef __FLIB_SOCKET_H__
#define __FLIB_SOCKET_H__
#pragma once

#include "FType.h"
#include <ctype.h>
#include <functional>
#include <memory>
#if FLIB_COMPILER_WINDOWS
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#define F_SOCKET unsigned int
#define F_SOCK_STREAM 1
#define F_INVALID_SOCKET (F_SOCKET)(~0)
#define socklen_t int
#define F_SOCKET_STARTUP               \
	{ WSADATA wsd; ::WSAStartup(MAKEWORD(2, 2), &wsd); }
#define F_SOCKET_CLEANUP               \
	{ ::WSACleanup(); }
#define F_ERRNO WSAGetLastError()
#else
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#define F_SOCKET int
#define F_SOCK_STREAM SOCK_STREAM
#define F_INVALID_SOCKET -1
#define INVALID_SOCKET F_INVALID_SOCKET
#define SOCKET_ERROR -1
#define F_SOCKET_STARTUP 	signal(SIGPIPE, SIG_IGN);
#define F_SOCKET_CLEANUP
#define F_ERRNO errno
#endif

_FStdBegin

class F_DLL_API FSockAddr
{
	friend class CSockAddrImpl;
	class CSockAddrImpl* _impl;
public:
	FSockAddr();
	FSockAddr(unsigned short uport, const char* ip = NULL);
	~FSockAddr();
	void* SockAddr() const;
	socklen_t addlen;
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