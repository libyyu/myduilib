#define FLIB_DLL
#include "FMemPool.h"
#include <iostream>


_FStdBegin
const unsigned int NodeSize = sizeof(FMemPool::F_Node);
const unsigned int MaxSaveBlock = 1024 * 1024 * 1;

FMemPool::FMemPool(const unsigned int& nDataSize, const unsigned int& nMaxSize/*=0*/)
	: m_nBlockSize(nDataSize/*+NodeSize*/),
	m_nMaxSize(nMaxSize), 
	m_pFirst(NULL), 
	m_pNext(NULL), 
	m_nInCount(0), 
	m_nOutConut(0) 
{}

FMemPool::~FMemPool()
{
	m_Lock.EnterWrite();
	{
		if (m_pFirst) DeleteNext(m_pFirst);
		m_pFirst = NULL;
		if (m_pNext) delete m_pNext;
		m_pNext = NULL;
	}
	m_Lock.LeaveWrite();
}

void* FMemPool::GetBuf(const unsigned int& nSize)
{
	void* pResutl = NULL;
	F_Node* pNewNode = NULL;

	if ((nSize + NodeSize) <= m_nBlockSize)
	{		                              
		m_Lock.EnterWrite();
		{
			if (m_pFirst)                  
			{                             				  				
				pNewNode = m_pFirst;
				m_pFirst = m_pFirst->next;
				pNewNode->next = NULL;
				--m_nInCount;
				++m_nOutConut;
				pResutl = (char*)pNewNode + NodeSize; 
			}
			else if (0 == m_nMaxSize || m_nOutConut < m_nMaxSize)
			{
				pNewNode = (F_Node*)malloc(m_nBlockSize);
				if (pNewNode)
				{
					pNewNode->nLen = m_nBlockSize;      
					pNewNode->next = NULL;
					pResutl = (char*)pNewNode + NodeSize;
					++m_nOutConut;
				}
			}
		}
		m_Lock.LeaveWrite();
	}
	else 
	{
		m_Lock.EnterWrite();
		if (!m_pNext)
			m_pNext = new FMemPool(2 * (m_nBlockSize/*-NodeSize*/), m_nMaxSize);
		if (m_pNext)
			pResutl = m_pNext->GetBuf(nSize);
		m_Lock.LeaveWrite();
	}
	return pResutl;
}
bool FMemPool::FreeBuf(void* pBuf, bool bDelete)                  
{
	if (!pBuf) return false;
	bool bResult = false;
	F_Node* pNode = GetNode(pBuf);
	if (!pNode) return false;

	if (m_nBlockSize == pNode->nLen) 
	{
		if (m_nBlockSize >= MaxSaveBlock || bDelete)
		{
			free(pNode);
			--m_nOutConut;
		}
		else
		{
			m_Lock.EnterWrite();         
			{
				pNode->next = m_pFirst;
				m_pFirst = pNode;
			}
			m_Lock.LeaveWrite();
			--m_nOutConut;
			++m_nInCount;
		}
		bResult = true;
	}
	else
	{
		m_Lock.EnterRead();
		{
			if (m_pNext)
				bResult = m_pNext->FreeBuf(pBuf, bDelete);
		}
		m_Lock.LeaveRead();
	}

	return bResult;
}
bool FMemPool::IsIn(const void* pBuf, const unsigned int& nNewSize)
{
	assert(pBuf);
	if (0 >= nNewSize) return false;
	if ((FMemPool::GetNode(pBuf)->nLen - NodeSize) >= nNewSize)
		return true;
	else
		return false;
}
unsigned int FMemPool::GetBufLen(const void* pBuf)
{
	return GetNode(pBuf)->nLen;
}
void FMemPool::PrintInfo()
{
	//TODO:
}

void FMemPool::DeleteNext(FMemPool::F_Node* pNext)
{
	if (!pNext) return;
	F_Node* pNode = pNext;
	F_Node* pTmpNode = NULL;
	while (1)
	{
		pTmpNode = pNode->next;
		free(pNode);
		--m_nInCount;
		if (!pTmpNode) break;
		pNode = pTmpNode;
	}
}

FMemPool::F_Node* FMemPool::GetNode(const void* pBuf)
{
	assert(pBuf);
	F_Node* pTmpNode = (F_Node*)((char*)pBuf - NodeSize);
	return pTmpNode;
}

/////////////////////////////////////////////////////////////////////
//
FMemPoolMgr::FMemPoolMgr(unsigned int nMinSize/* = 8*/, const unsigned int nMaxSize/* = 0*/)
{
	m_pHead = new FMemPool((nMinSize < 16 ? nMinSize = 16 : nMinSize), nMaxSize);
}
FMemPoolMgr::~FMemPoolMgr()
{
	if (m_pHead)
	{
		delete m_pHead;
		m_pHead = NULL;
	}
}

void* FMemPoolMgr::GetBuf(unsigned int& nSize)
{
	if (0 >= nSize) return NULL;

	void* pResult = NULL;
	if (m_pHead)
	{
		pResult = m_pHead->GetBuf(nSize);
	}
	return pResult;
};
void* FMemPoolMgr::ReGetBuf(void* pOldBuf, unsigned int& nNewSize, bool bCopyOldData /*= true*/)
{
	assert(pOldBuf);

	void* pResutl = NULL;
	if (0 >= nNewSize)
	{
		return pResutl;
	}
	if (m_pHead->IsIn(pOldBuf, nNewSize))
	{
		pResutl = pOldBuf;
	}
	else
	{
		pResutl = m_pHead->GetBuf(nNewSize);
		if (pResutl && pOldBuf)
		{
			if (bCopyOldData)
				memcpy(pResutl, pOldBuf, FMemPool::GetBufLen(pOldBuf));
		}
	}
	return pResutl;
};
bool  FMemPoolMgr::Free(void* pBuf, bool bDelete /*= false*/)
{
	if (!pBuf) return false;
	if (m_pHead)
		m_pHead->FreeBuf(pBuf, bDelete);
	return true;
};


_FStdEnd

