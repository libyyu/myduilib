#define FLIB_DLL
#include "FAlloctor.h"

_FStdBegin

template<size_t T, typename LOCK, size_t C>
FAlloctorPool<T, LOCK, C>::FAlloctorPool() : _pBlocks(NULL), _pFreeNodes(NULL) {}
template<size_t T, typename LOCK, size_t C>
FAlloctorPool<T, LOCK, C>::~FAlloctorPool()
{
	_lock.lock();
	while (NULL != _pBlocks)
	{
		PBLOCK pItem = _pBlocks;
		_pBlocks = _pBlocks->pPre;
		//free(pItem);
		delete pItem;
	}
	_lock.unlock();
}

template<size_t T, typename LOCK, size_t C>
void*   FAlloctorPool<T, LOCK, C>::Alloc()
{
	_lock.lock();

	if (_pFreeNodes == NULL)
	{
		// PBLOCK pBlock = (PBLOCK)malloc(sizeof(BLOCK)); 
		PBLOCK pBlock = new BLOCK;
		assert(pBlock);
		if (NULL == pBlock)
		{
			_lock.unlock();
			return NULL;
		}

		pBlock->pPre = _pBlocks;
		pBlock->pNodes[0].pPre = NULL;
		for (size_t i = 1; i<C; ++i)
		{
			pBlock->pNodes[i].pPre = &pBlock->pNodes[i - 1];
		}
		_pBlocks = pBlock;
		_pFreeNodes = &_pBlocks->pNodes[C - 1];
	}

	PNODE pNode = _pFreeNodes;
	_pFreeNodes = _pFreeNodes->pPre;

	_lock.unlock();

	//pNode->pPre = (PNODE)T; //指向一个常指针区域
	pNode->pPre = (PNODE)this;

	return pNode->szBuf;
}
template<size_t T, typename LOCK, size_t C>
void    FAlloctorPool<T, LOCK, C>::Free(void *p)
{
	PNODE pNode = (PNODE)((char*)p - sizeof(PNODE));

	_lock.lock();
	pNode->pPre = _pFreeNodes;
	_pFreeNodes = pNode;
	_lock.unlock();
}

template <typename LOCK>
FAlloctorPoolMgr<LOCK>::FAlloctorPoolMgr() :_pHeader(NULL) {}

template <typename LOCK>
FAlloctorPoolMgr<LOCK>::~FAlloctorPoolMgr()
{
	_lock.lock();
	while (NULL != _pHeader)
	{
		PNODE pItem = _pHeader;
		_pHeader = _pHeader->pPre;
		free(pItem);
	}
	_lock.unlock();
}

template <typename LOCK>
void* FAlloctorPoolMgr<LOCK>::Alloc(size_t size)
{
	assert(size > 0);
	if (size <= 0)
		return NULL;
	if (size <= 16)
		return _pool16.Alloc();
	if (size <= 32)
		return _pool32.Alloc();
	if (size <= 64)
		return _pool64.Alloc();
	if (size <= 128)
		return _pool128.Alloc();
	if (size <= 256)
		return _pool256.Alloc();
	if (size <= 512)
		return _pool512.Alloc();
	if (size <= 1024)
		return _pool1024.Alloc();
	if (size <= 2048)
		return _pool2048.Alloc();
	if (size <= 4096)
		return _pool4096.Alloc();
	if (size <= 8192)
		return _pool8192.Alloc();

	size_t newsize = size + sizeof(NODE);
	newsize = (((newsize)+7) &~7);// 按8字节对齐

	PNODE pItem = (PNODE)malloc(newsize);

	if (NULL == pItem)
		return NULL;

	_lock.lock();
	pItem->pPre = _pHeader;
	if (NULL != _pHeader)  (_pHeader->pNext = pItem);
	_pHeader = pItem;
	_lock.unlock();

	pItem->size = (size_t)this;

	//  printf("alloc pitem:%p,this:%p\n",pItem,this);
	return (char*)pItem + sizeof(NODE);
}

template <typename LOCK>
void  FAlloctorPoolMgr<LOCK>::Free(void* p)
{
	assert(NULL != p);
	if (NULL == p)
		return;

	size_t size = *((size_t*)p - 1);
	assert(size > 0);

	if (this == (self_type *)size)
	{
		PNODE pItem = (PNODE)((char*)p - sizeof(NODE));
		//   printf("free  pitem:%p,this:%p\n",pItem,this);

		_lock.lock();
		if (pItem == _pHeader)
			_pHeader = _pHeader->pPre;
		else
		{
			PNODE preNode = pItem->pPre;
			pItem->pNext->pPre = preNode;
			if (preNode)
				preNode->pNext = pItem->pNext;
		}
		_lock.unlock();

		free(pItem);
		return;
	}

	FAlloctorPool<16, LOCK> * pool = (FAlloctorPool<16, LOCK> *)size;
	return pool->Free(p);
}

static FAlloctorPoolMgr<FLock> s_alloctor_pool;

void* FPoolMalloc(size_t nSize)
{
    return s_alloctor_pool.Alloc(nSize);
}
void  FPoolFree(void* p)
{
    return s_alloctor_pool.Free(p);
}

_FStdEnd

