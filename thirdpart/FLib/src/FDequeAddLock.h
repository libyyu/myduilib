#ifndef  __FLIB_DEQUEADDLOCK_H__
#define  __FLIB_DEQUEADDLOCK_H__
#pragma once
#include "FDReadAWriteLock.h"

_FStdBegin
template <class T>
class F_DLL_API FDequeAddLock
{
public:
	FDequeAddLock();
	~FDequeAddLock();
private:
    typedef struct __Node
    {
        struct __Node* pFront;
        struct __Node* pNext;
        T data;
    }Node;
    typedef bool (__stdcall *EnumVaulePro)(const T&,void*);
private:
    FDReadAWriteLock m_Lock;
    unsigned int m_nCount;
    Node* m_pHead;
    Node* m_pTail;	
public:
    void AddTail(T& data);
    bool DelHead();
    void Clear();
    bool Empty();
    bool GetHead(T& outResult);
    unsigned int GetCount();
    void EnumVaule(EnumVaulePro pEnumFun,void* pParam = NULL);
};


_FStdEnd

#endif//__FLIB_DEQUEADDLOCK_H__