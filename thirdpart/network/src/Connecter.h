#pragma once
#include "SockBase.h"

class Connecter
{
public:
	static bool connect(const spSockContextT& s,const char* peer,const char* sport,size_t timeout = 10);
	static bool asynConnect(const spSockContextT& s,const char* peer,const char* sport,size_t timeout = 10);
};