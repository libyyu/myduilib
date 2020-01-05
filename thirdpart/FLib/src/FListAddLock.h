#ifndef   __FLIB_LISTADDLOCK_H__
#define   __FLIB_LISTADDLOCK_H__
#pragma once
#include "FDReadAWriteLock.h"

_FStdBegin

template <class T>
class F_DLL_API FListAddLock
{
public:
	FListAddLock();
	~FListAddLock();
private:
	struct Node
	{
		Node* pNext;
		T data;
        
		Node():data(T()),pNext(NULL){};
	};
	typedef bool (_STDCALL *EnumVaulePro)(T&,void*);
private:
	FDReadAWriteLock m_Lock;
	unsigned int m_nCount;
	Node* m_pHead;
public:
	void Add(T& data);
	bool Del(T& data);
	void Clear();
	bool Empty();
	unsigned int GetCount();
	bool EnumVaule(EnumVaulePro pEnumFun,void* pParam = NULL);
};

_FStdEnd

#endif//__FLIB_LISTADDLOCK_H__
