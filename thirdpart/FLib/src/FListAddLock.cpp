#define FLIB_DLL
#include "FListAddLock.h"

_FStdBegin

template <class T>
FListAddLock<T>::FListAddLock() :m_pHead(NULL), m_nCount(0) {}
template <class T>
FListAddLock<T>::~FListAddLock() { Clear(); }

template <class T>
void FListAddLock<T>::Add(T &data)
{
	Node* pNewNode = new Node;
	if (!pNewNode) return;
	pNewNode->data = data;

	m_Lock.EnterWrite();
	{
		if (!m_pHead)
		{
			m_pHead = pNewNode;		
		}
		else
		{
			pNewNode->pNext = m_pHead;
			m_pHead = pNewNode;
		}	
		++m_nCount;
	}
	m_Lock.LeaveWrite();
}

template <class T>
bool FListAddLock<T>::Del(T &data)
{
	bool bResult = false;
	Node* pDelNode = NULL;
	Node* pTmpNode = NULL;
	Node* pFirNode = NULL;
	m_Lock.EnterWrite();
	{
		if (!m_pHead)	goto __End_Fun;
		
		if (data == m_pHead->data)
		{
			pDelNode = m_pHead;
			m_pHead->pNext ?(m_pHead = m_pHead->pNext) : m_pHead = NULL;		
			goto __End_Fun;
		}

		pFirNode = m_pHead;
		pTmpNode = m_pHead->pNext;  //?????????????????????????????????????????????????????
		while(pTmpNode)
		{
			if (data == pTmpNode->data)
			{
				pDelNode = pTmpNode;
				pTmpNode->pNext ?(pFirNode->pNext  = pDelNode->pNext):(pFirNode->pNext = NULL);				
				goto __End_Fun;
			}
			pTmpNode = pTmpNode->pNext;
		}	
	}

__End_Fun:
	if (pDelNode)
	{
		bResult = true;
		delete pDelNode;
		pDelNode = NULL;
		--m_nCount;
	}
	m_Lock.LeaveWrite();
	return bResult;
}

template <class T>
void FListAddLock<T>::Clear()
{
	Node* pDleNode = NULL;
	m_Lock.EnterWrite();
	{
		while (m_pHead)
		{
			pDleNode = m_pHead;
			m_pHead->pNext ? (m_pHead = m_pHead->pNext):(m_pHead = NULL);			
     
			delete pDleNode;
			pDleNode = NULL;
			--m_nCount;
		}
	}
	m_Lock.LeaveWrite();
}

template <class T>
bool FListAddLock<T>::Empty()
{
	bool bResult = false;
	m_Lock.EnterRead();
	m_pHead ? bResult = false : bResult =true;
	m_Lock.LeaveRead();
	return bResult;
}

template <class T>
unsigned int FListAddLock<T>::GetCount()
{
	unsigned int nResult = 0;
	m_Lock.EnterRead();
	nResult = m_nCount;
	m_Lock.LeaveRead();
	return nResult;
}

template <class T>
bool FListAddLock<T>::EnumVaule(EnumVaulePro pEnumFun,void* pParam /* = NULL */)
{
	bool bResult = false;
	m_Lock.EnterRead();
	{
		Node* pNode = NULL;
		pNode = m_pHead;
		while(pNode)
		{
			if ( pEnumFun(pNode->data,pParam))
			{
				bResult = true;
				break;
			}
			pNode = pNode->pNext;
		}
	}
	m_Lock.LeaveRead();
	return bResult;
}

_FStdEnd

