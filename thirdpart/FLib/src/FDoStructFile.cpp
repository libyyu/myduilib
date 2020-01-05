#define FLIB_DLL
#include "FDoStructFile.h"
#include <fstream>

_FStdBegin

template <class T, class U>
FDoStructFile<T, U>::FDoStructFile() :m_nSize(sizeof(value_type)) {}

template <class T, class U>
FDoStructFile<T, U>::~FDoStructFile(){}

template <class T, class U>
bool FDoStructFile<T,U>::ReadFileToMem(const char* lpFileName)
{
	assert(lpFileName);
	if(!lpFileName) return false;  
	bool bVar = false;    
	
	m_Lock.EnterWrite();  
	std::ifstream fs(lpFileName,std::ios::in|std::ios::binary);
	if(!fs.is_open()) goto _End_Fun;
	
	{
	  bVar = true;
	  m_contian.clear();
	  value_type vule;
      while(fs.read((char*)&vule,m_nSize))
      {
    	m_contian.push_back(vule);
	  }
	  fs.close();
	  goto _End_Fun;
	}
	
_End_Fun:
    m_Lock.LeaveWrite();                	
    return bVar;	
}

template <class T,class U>
bool FDoStructFile<T,U>::WriteFileFormMem(const char* lpFileName)
{
	assert(lpFileName);
	if(!lpFileName) return false;
	bool bVar = false;
	                  
    m_Lock.EnterRead();  
	std::ofstream fs(lpFileName,std::ios::out|std::ios::binary);
	if(!fs.is_open()) goto _End_Fun;
	
	{
	  bVar = true;	
	  const_itor i;
	  for(i=m_contian.begin(); i!=m_contian.end(); ++i)
	  {
		value_type vale = *i;	
	    fs.write((char*)&vale,m_nSize);
	  }
	  fs.close();
	  goto _End_Fun;
	}

_End_Fun:
    m_Lock.LeaveRead();	 
	return bVar;
}

template <class T,class U>
void FDoStructFile<T,U>::EnumVaule(const EnumVaulePro lpEnumVaulePro, void *pParam/* = NULL*/)
{
	m_Lock.EnterRead();
	const_itor i;
	for (i=m_contian.begin();i!=m_contian.end();++i)
	{
		if (  lpEnumVaulePro(*i,pParam) )	break;	
	}
    m_Lock.LeaveRead();
}

template <class T,class U>
void FDoStructFile<T,U>::InsertVaule(const value_type& x)
{
	m_Lock.EnterWrite();
	m_contian.push_back(x);
	m_Lock.LeaveWrite();
}

template <class T,class U>
void FDoStructFile<T,U>::Clear()
{
	m_Lock.EnterWrite();
	m_contian.clear();
	m_Lock.LeaveWrite();
}
_FStdEnd

