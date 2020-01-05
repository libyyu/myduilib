#ifndef  __FLIB_MEMPOOL_H__
#define  __FLIB_MEMPOOL_H__
#pragma once
#include "FDReadAWriteLock.h"

_FStdBegin

class F_DLL_API FMemPool
{
public:
	typedef struct _F_Node_
	{
		unsigned int nLen;
		struct _F_Node_* next;
	}F_Node;
	
	FMemPool(const unsigned int& nDataSize, const unsigned int& nMaxSize = 0);
	~FMemPool();
public:
	void* GetBuf(const unsigned int& nSize);
	bool FreeBuf(void* pBuf, bool bDelete);
	bool IsIn(const void* pBuf, const unsigned int& nNewSize);
	static unsigned int GetBufLen(const void* pBuf);
	void PrintInfo();
private:
	void DeleteNext(F_Node* pNext);
	static F_Node* GetNode(const void* pBuf);
private:
	const unsigned int  m_nBlockSize; 
	const unsigned int  m_nMaxSize;
	unsigned long       m_nOutConut;
	unsigned long       m_nInCount;
	F_Node*         m_pFirst;
	FDReadAWriteLock m_Lock; 
	FMemPool*   m_pNext;
};

class FMemPoolMgr
{
public:
	FMemPoolMgr(unsigned int nMinSize = 8, const unsigned int nMaxSize = 0);
	~FMemPoolMgr();
public:
	void* GetBuf(unsigned int& nSize);
	void* ReGetBuf(void* pOldBuf, unsigned int& nNewSize, bool bCopyOldData = true);
	bool  Free(void* pBuf, bool bDelete = false);
private:
	FMemPool* m_pHead;
};


_FStdEnd

#endif//__FLIB_MEMPOOL_H__
