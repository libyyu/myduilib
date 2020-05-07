#include "flib.h"
#include <iostream>
#include <Windows.h>

int main()
{
    F_SOCKET_STARTUP
    _FStd(FSocket) sock;
    _FStd(FSockAddr) addr(80, "libyyu.com");

    sock.Create();
    bool ret = sock.Connect(&addr);
    if (!ret) {
        std::cerr << "Connect Return " << (F_ERRNO) << std::endl;
    }
    F_SOCKET_CLEANUP
    return 0;
}