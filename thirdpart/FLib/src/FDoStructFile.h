
#ifndef __FLIB_DOSTRUCTFILE_H__
#define __FLIB_DOSTRUCTFILE_H__
#pragma once
#include "FDReadAWriteLock.h"
#include <algorithm>

_FStdBegin

template <class T,class U = std::vector<T> >
class F_DLL_API FDoStructFile
{ 
public:
	typedef typename U::value_type value_type;
	typedef typename U::size_type size_type;
	typedef U  container_type;
	typedef typename U::const_iterator const_itor;
private:
	FDReadAWriteLock m_Lock; 
	U m_contian;	
	const unsigned int m_nSize;
	typedef bool (_STDCALL *EnumVaulePro)(const value_type&,void*);
public:
	FDoStructFile();
    ~FDoStructFile();
public:
    bool ReadFileToMem(const char* lpFileName);
	bool WriteFileFormMem(const char* lpFileName);	
	void InsertVaule(const value_type& x);
	void Clear();
	void EnumVaule(const EnumVaulePro lpEnumVaulePro,void* pParam = NULL);
};

_FStdEnd
#endif//__FLIB_DOSTRUCTFILE_H__
