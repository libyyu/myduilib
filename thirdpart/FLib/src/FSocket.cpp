
#define FLIB_DLL
#if defined( _MSC_VER )
#	ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#	define _WINSOCK_DEPRECATED_NO_WARNINGS
#	endif
#	include <winsock2.h>
#	include <windows.h>
#endif
#include "FSocket.h"
#if FLIB_COMPILER_WINDOWS
#pragma comment(lib,"ws2_32.lib")
#include<ws2tcpip.h>
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
#endif
_FStdBegin

int FGetNetLastError()
{
#if FLIB_COMPILER_WINDOWS
	return WSAGetLastError();
#else
	return errno;
#endif
}
int FSocketStartup()
{
#if FLIB_COMPILER_WINDOWS
	WSADATA wsd; 
	return ::WSAStartup(MAKEWORD(2, 2), &wsd);
#else
	return signal(SIGPIPE, SIG_IGN);
#endif
}
int FSocketCleanup()
{
#if FLIB_COMPILER_WINDOWS
	return ::WSACleanup();
#else
	return 0;
#endif
}

class CSockAddrImpl : public sockaddr_in
{
public:
	CSockAddrImpl()
	{
	}
	CSockAddrImpl(unsigned short uport, const char* ip = NULL)
	{
		unsigned long  nIp = htonl(INADDR_ANY);
		if (ip)
		{
			if (isalpha(ip[0]))
			{
#if FLIB_COMPILER_WINDOWS         
				LPHOSTENT     lphostent;
				lphostent = ::gethostbyname(ip);
				if (lphostent)
				{
					nIp = inet_addr(inet_ntoa(*((LPIN_ADDR)* lphostent->h_addr_list)));
				}
#else       
				struct hostent* hptr;
				if ((hptr = gethostbyname(ip)) != NULL)
				{
					nIp = inet_addr(inet_ntoa(*((in_addr*)* hptr->h_addr_list)));
				}
#endif
			}
			else
			{
				nIp = inet_addr(ip);
			}
		}

		assert(uport);
		this->sin_family = AF_INET;
		this->sin_port = htons(uport);
#if FLIB_COMPILER_WINDOWS
		this->sin_addr.S_un.S_addr = nIp;
#else
		this->sin_addr.s_addr = nIp;
#endif
	}
};

FSockAddr::FSockAddr() 
	: _impl(new CSockAddrImpl())
{
}
FSockAddr::FSockAddr(unsigned short uport, const char* ip /*= NULL*/) 
	: _impl(new CSockAddrImpl(uport, ip))
{
}
FSockAddr::~FSockAddr() 
{
	delete _impl;
	_impl = NULL;
}
void* FSockAddr::SockAddr() const
{
	return _impl;
}

#define __SOCKET_ADDR_LEN sizeof(sockaddr_in)

FSocket::FSocket(F_SOCKET s/* = F_INVALID_SOCKET*/) : _s(s) {}
FSocket::~FSocket() { Close(); }
FSocket::operator F_SOCKET() const { return _s; }
bool FSocket::IsCreate()const { return F_INVALID_SOCKET != _s; }

bool FSocket::Ioctl(long cmd, unsigned long* argp)
{
	assert(IsCreate());
	if(!IsCreate()) return false;
#if FLIB_COMPILER_WINDOWS
	return F_SOCKET_ERROR != ::ioctlsocket(_s,cmd,argp);
#else
    return F_SOCKET_ERROR != ioctl(_s, cmd, argp);
#endif
}
void FSocket::SetKeepAlive(bool on)
{
    assert(IsCreate());
    if(!IsCreate()) return;
    char optval = on ? 1 : 0;
	::setsockopt(_s, SOL_SOCKET, SO_KEEPALIVE, &optval, static_cast<socklen_t>(sizeof optval));
}
void FSocket::SetTcpNoDelay(bool on)
{
    assert(IsCreate());
    if(!IsCreate()) return;
    char optval = on ? 1 : 0;
    ::setsockopt(_s, SOL_SOCKET, TCP_NODELAY, &optval, static_cast<socklen_t>(sizeof optval));
}
bool FSocket::Create(int nType)
{
	if(IsCreate()) return true;
#if FLIB_COMPILER_WINDOWS
    _s = ::WSASocket(AF_INET, nType, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
#else
    _s = ::socket(AF_INET, nType, 0);
#endif
    if (F_INVALID_SOCKET == _s)
    {
        return false;
    }

	assert(IsCreate());
	return IsCreate();
}
bool FSocket::Close()
{
	if(!IsCreate()) return true;
#if FLIB_COMPILER_WINDOWS
    ::shutdown(_s, SD_BOTH);
    if (F_SOCKET_ERROR == ::closesocket(_s))
        return false;
#else
    ::shutdown(_s, SHUT_RDWR);
    if (-1 == ::close(_s))
        return false;
#endif
	_s = F_INVALID_SOCKET;
	return !IsCreate();
}
bool FSocket::Bind(const FSockAddr* addr)
{
	assert(IsCreate());
	if(!IsCreate()) return false;
	assert(addr);
	return F_SOCKET_ERROR != ::bind(_s,(sockaddr*)(addr->SockAddr()), __SOCKET_ADDR_LEN);
}
bool FSocket::Listen(int nbacklog /* = 5 */)
{
	assert(IsCreate());
	if(!IsCreate()) return false;

	return F_SOCKET_ERROR != ::listen(_s, nbacklog);
}
F_SOCKET FSocket::Accept(FSockAddr* addr/* = NULL*/)
{
	assert(IsCreate());
	if(!IsCreate()) return false;
	socklen_t len = sizeof(sockaddr_in);
	return ::accept(_s, (sockaddr*)(addr->SockAddr()), &len);
}
bool FSocket::IsConnect() const
{
	assert(IsCreate());
	if(!IsCreate()) return false;

	socklen_t optval, optlen = sizeof(int);
	if(F_SOCKET_ERROR != getsockopt(_s, SOL_SOCKET, SO_ERROR,(char*) &optval, &optlen))
	{
		switch(optval)
		{
		case 0: return true; 
#if FLIB_COMPILER_WINDOWS 
		case ECONNREFUSED:break;
#endif
		}
	}
	
	return false;
}
bool FSocket::Connect(const FSockAddr* pRemoteaddr)
{
	assert(IsCreate());
	if(!IsCreate()) return false;

	assert(pRemoteaddr);
	int ret = ::connect(_s,(sockaddr*)(pRemoteaddr->SockAddr()), __SOCKET_ADDR_LEN);
    if(F_SOCKET_ERROR == ret
#if FLIB_COMPILER_WINDOWS
        && WSAEWOULDBLOCK != ::WSAGetLastError()
#endif
    )
    {
        return false;
    }
    return true;
}
bool FSocket::ConnectEx(const FSockAddr* pRemoteaddr,int nTimeOut /* = 10 */,FSocket::ExitWaitPred isExitWaitPred/* = NULL*/)
{
	assert(IsCreate());
	if(!IsCreate()) return false;

	assert(pRemoteaddr);
	unsigned long ul = 1; 
	this->Ioctl(FIONBIO, &ul);//设置阻塞模式<0为阻塞，1非阻塞>  
	int nRet = ::connect(_s,(sockaddr*)(pRemoteaddr->SockAddr()), __SOCKET_ADDR_LEN);
	if(F_SOCKET_ERROR == nRet)
	{
        int nError = F_ERRNO;
#if FLIB_COMPILER_WINDOWS
		if(WSAEWOULDBLOCK != nError && 
			WSAEALREADY != nError &&
			WSAEISCONN != nError)
		{ 
			return false;
		}
		if(WSAEISCONN != nError)
		{
			if(!this->WaitWriteable(nTimeOut,isExitWaitPred))
				return false;
		}
#else
        if(EWOULDBLOCK != nError && 
            EALREADY != nError &&
            EISCONN != nError)
        {
            return false;
        }
        if(EISCONN != nError)
        {
            if(!this->WaitWriteable(nTimeOut,isExitWaitPred))
				return false;
        }
#endif
	}
	ul = 0;
	this->Ioctl(FIONBIO, &ul);
	return true;
}
bool FSocket::WaitReadable(unsigned int nTimeOut /* = 10 */,FSocket::ExitWaitPred isExitWaitPred /* = NULL */)
{
	assert(IsCreate());
	if(!IsCreate()) return false;

	if(0 >= nTimeOut)
		nTimeOut = 1;
	while(nTimeOut-- && !(isExitWaitPred ? isExitWaitPred() : false))
	{
		fd_set      fdRead;
		timeval     tv;
		FD_ZERO(&fdRead);
		FD_SET(_s,&fdRead);
		tv.tv_sec  = 1;
		tv.tv_usec = 0;
#if FLIB_COMPILER_WINDOWS
		int nRet = ::select(0,&fdRead,NULL,NULL,&tv);
#else
		int nRet = ::select(_s+1,&fdRead,NULL,NULL,&tv);
#endif
		if(0 < nRet)
			return true;
		if(0 > nRet)
			break;
	}
	return false;
}
bool FSocket::WaitWriteable(unsigned int nTimeOut /* = 10 */,FSocket::ExitWaitPred isExitWaitPred /* = NULL */)
{
	assert(IsCreate());
	if(!IsCreate()) return false;

	if(0 >= nTimeOut)
		nTimeOut = 1;
	while(nTimeOut-- && !(isExitWaitPred ? isExitWaitPred() : false))
	{
		fd_set      fdWrite;
		timeval     tv;
		FD_ZERO(&fdWrite);
		FD_SET(_s,&fdWrite);
		tv.tv_sec  = 1;
		tv.tv_usec = 0;
#if FLIB_COMPILER_WINDOWS
		int nRet = ::select(0,NULL,&fdWrite,NULL,&tv);
#else
		int nRet = ::select(_s+1,NULL,&fdWrite,NULL,&tv);
#endif
		if(0 < nRet)
			return true;
		if(0 > nRet)
			break;
	}

	return false;
}
int FSocket::Recv(char* buf,size_t cnt)
{
	assert(IsCreate());
	return ::recv(_s,buf,cnt,0);
}
int FSocket::Send(char* buf,size_t cnt)
{
	assert(IsCreate());

	size_t nSendLen      = 0;
	while(nSendLen != cnt)
	{
		int nRet = ::send(_s,(char*)buf+nSendLen,cnt-nSendLen,0);
		if(nRet <= 0)
			break;

		nSendLen += nRet;
	}

	return nSendLen;
}
int FSocket::RecvFrom(char* buf,size_t cnt,FSockAddr* pRemoteaddr)
{
	assert(IsCreate());
	assert(pRemoteaddr);   
	socklen_t len = sizeof(sockaddr_in);
	return ::recvfrom(_s,buf,cnt,0,(sockaddr*)(pRemoteaddr->SockAddr()), &len);
}
int FSocket::SendTo(char* buf,size_t cnt,FSockAddr* pRemoteaddr)
{
	assert(IsCreate());
	assert(pRemoteaddr);

	return ::sendto(_s,buf,cnt,0,(sockaddr*)(pRemoteaddr->SockAddr()), __SOCKET_ADDR_LEN);
}

_FStdEnd
