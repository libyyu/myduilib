#ifndef  __FLIB_COUNTER_H__
#define  __FLIB_COUNTER_H__
#pragma once
#include "FLock.h"

_FStdBegin
class F_DLL_API FCounter
{
public:
	explicit FCounter();
	~FCounter();
private:
    FLock m_Lock;
    unsigned int m_i;
public:
	unsigned int  Set(const unsigned int& n);
	unsigned int  Add();
	unsigned int  Dec();
	unsigned int  GetCount();
};
_FStdEnd

#endif//__FLIB_COUNTER_H__

