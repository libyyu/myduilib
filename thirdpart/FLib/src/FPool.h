#ifndef __FLIB_POOL_H__
#define __FLIB_POOL_H__
#pragma once
#include "FLock.h"
#include "FMemory.h"

_FStdBegin

template <class T, unsigned int nInitCount = 10>
class F_DLL_API FPool
{
public:
	template <class C>
	struct _F_Node
	{
		_F_Node<C>* pNext;
		C data;
	};
private:
    _F_Node<T>*         m_pHead;
    unsigned long       m_nOutCount;
    unsigned long       m_nInCount;
	FLock               m_lock;
public:
	FPool();
	~FPool();
public:
    T* GetBuf();
    void FreeBuf(T* p,bool bDelete = false);
    void PrintInfo();
private:
    void DeleteAll_Node();
};
_FStdEnd


#endif//__FLIB_POOL_H__
