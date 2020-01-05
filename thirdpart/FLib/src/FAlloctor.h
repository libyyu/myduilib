#ifndef __FLIB_ALLOCTOR_H__
#define __FLIB_ALLOCTOR_H__
#pragma once
#include "FLock.h"
#include "FMemory.h"

_FStdBegin
template<size_t T,typename LOCK = FLock,size_t C = 16> 
class F_DLL_API FAlloctorPool
{ 
public: 
	FAlloctorPool();
	virtual ~FAlloctorPool();
	typedef FAlloctorPool<T,LOCK,C> self_type;
	typedef LOCK               		lock_type;
    
public: 
	void*   Alloc();
	void    Free(void *p);
    
protected: 
	FAlloctorPool(const self_type &Pool); 
	self_type operator =(const self_type &Pool);    
protected: 
	typedef struct tagNODE 
	{ 
		tagNODE *       pPre; 
		char            szBuf[T]; 
	}NODE, *PNODE; 
    
	typedef struct tagBLOCK 
	{
		tagBLOCK *      pPre;
		NODE            pNodes[C];
	}BLOCK, *PBLOCK; 
    
protected: 
	PBLOCK       _pBlocks; 
	PNODE        _pFreeNodes; 
    lock_type    _lock;
};

template<typename LOCK,size_t C> 
class FAlloctorPool<0,LOCK,C>; //MAKE ERROR

class F_DLL_API FAlloctorPoolBase
{
public:
    FAlloctorPoolBase(){}
    virtual ~FAlloctorPoolBase(){} 
public:
    virtual void* Alloc(size_t size)=0;
    virtual void  Free(void* p)=0;
};

template <typename LOCK = FLock>
class F_DLL_API FAlloctorPoolMgr : public FAlloctorPoolBase
{
public:
	explicit FAlloctorPoolMgr();
	virtual ~FAlloctorPoolMgr();
public:
	void* Alloc(size_t size);
	void  Free(void* p);
    
protected: 
	typedef FAlloctorPoolMgr<LOCK> self_type;
	typedef LOCK    lock_type;
	FAlloctorPoolMgr(const self_type &Pool); 
	self_type operator =(const self_type &Pool);
    
protected:
	typedef struct tagNODE {
		tagNODE *  pPre;
		tagNODE *  pNext;
		size_t     size;
	}NODE,*PNODE;
    
protected:
	PNODE            _pHeader;
	lock_type        _lock;
    
    FAlloctorPool<16,lock_type>     _pool16; 
	FAlloctorPool<32,lock_type>     _pool32; 
	FAlloctorPool<64,lock_type>     _pool64; 
	FAlloctorPool<128,lock_type>    _pool128; 
	FAlloctorPool<256,lock_type>    _pool256; 
	FAlloctorPool<512,lock_type>    _pool512; 
	FAlloctorPool<1024,lock_type>   _pool1024;
	FAlloctorPool<2048,lock_type>   _pool2048;
	FAlloctorPool<4096,lock_type>   _pool4096;
	FAlloctorPool<8192,lock_type>   _pool8192;
};


F_DLL_API void* FPoolMalloc(size_t nSize);
F_DLL_API void FPoolFree(void* p);

_FStdEnd

_FStdBegin
template <typename T> 
class FSTLAllocator;
template <> 
class FSTLAllocator<void> 
{
public:
	typedef void* pointer;
	typedef const void* const_pointer;
	// reference to void members are impossible.
	typedef void value_type;
	template <class U>
	struct rebind 
	{
		typedef FSTLAllocator<U> other;
	};
};

namespace internal {
	FLIB_FORCEINLINE void destruct(char*) {}
	FLIB_FORCEINLINE void destruct(wchar_t*) {}
	template <typename T>
	FLIB_FORCEINLINE void destruct(T* t) 
	{
		((void)&t);
		t->~T();
	}
} // namespace internal

template <typename T>
class FSTLAllocator {
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

	template <class U>
	struct rebind 
	{
		typedef FSTLAllocator<U> other;
	};

	FSTLAllocator() {}
	pointer address(reference x) 
	{
		return&x;
	}
	const_pointer address(const_reference x) const 
	{
		return &x;
	}
	pointer allocate(size_type size, FSTLAllocator<void>::const_pointer hint = 0) const 
	{
		return static_cast<pointer>(FLIB_ALLOC(sizeof(T) * size));
	}

	// For Dinkumware (VC6SP5):
	char* _Charalloc(size_type n) 
	{
		return static_cast<char*>(FLIB_ALLOC(n));
	}
	// end Dinkumware

	template <class U> FSTLAllocator(const FSTLAllocator<U>&) {}
	FSTLAllocator(const FSTLAllocator<T>&) {}
	void deallocate(pointer p, size_type n) const 
	{
		FLIB_FREE(p);
	}
	void deallocate(void* p, size_type n) const 
	{
		FLIB_FREE(p);
	}
	size_type max_size() const throw() 
	{
		return size_t(-1) / sizeof(value_type);
	}
	void construct(pointer p, const T& val) 
	{
		::new(static_cast<void*>(p)) T(val);
	}
	void destroy(pointer p) 
	{
		internal::destruct(p);
	}
private:
};

template <typename T, typename U>
FLIB_FORCEINLINE bool operator==(const FSTLAllocator<T>&, const FSTLAllocator<U>) 
{
	return true;
}

template <typename T, typename U>
FLIB_FORCEINLINE bool operator!=(const FSTLAllocator<T>&, const FSTLAllocator<U>) 
{
	return false;
}
_FStdEnd


#endif//__FLIB_ALLOCTOR_H__
