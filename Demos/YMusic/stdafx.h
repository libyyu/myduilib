// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#include "targetver.h"

#ifdef _MSC_VER
#pragma warning (disable : 4511) // copy operator could not be generated
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4702) // unreachable code (bugs in Microsoft's STL)
#pragma warning (disable : 4786) // identifier was truncated
#pragma warning (disable : 4996) // function or variable may be unsafe (deprecated)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
#endif
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // eliminate deprecation warnings for VS2005
#endif
#ifndef D_SCL_SECURE_NO_WARNINGS
#define D_SCL_SECURE_NO_WARNINGS
#endif
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#endif // _MSC_VER


#include <winsock2.h>
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <process.h>
#include <cassert>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlstr.h>
#include <atltrace.h>

#include <shlobj.h>   //BROSEINFO
#include <shellapi.h> //��ӦShell��

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <vector>
#include <set>
#include <map>
#include <deque>
#include <string>
#include <xstring>
#include <memory>
#include <wchar.h>



#include <WinInet.h>
#include <mmsystem.h>
#include "YMusic.h"

#define CONSOLE_TRACE
#if defined(_MSC_VER) && defined(CONSOLE_TRACE) && defined(DEBUG)
	//#pragma comment( linker, "/subsystem:console /entry:wWinMainCRTStartup" )
	#define console_trace printf
#else
	#define console_trace
#endif

/*

// ------------------------------------------------------------ 
// ---------------------- new and delete ---------------------- 
// ------------------------------------------------------------ 
// ---------------------------------------- operator new 
inline void * operator new (size_t size, const char* file, const size_t line,const char* func)
{
	void *p = malloc(size);
	if (p == NULL) throw std::bad_alloc();

	DuiLib::MemLooker::Instance()->Alloc(size);
	
	DuiLib::MemLooker::Instance()->AddMemInfo((long)p,DuiLib::MemInfo((file==NULL ? __FILE__:file),line,(func==NULL?__FUNCTION__:func),size));
	return p;
}
// ---------------------------------------- operator new[] 
inline void * operator new [](size_t size, const char* file, const size_t line,const char* func)
{
	void *p = malloc(size);
	if (p == NULL) throw std::bad_alloc();
	DuiLib::MemLooker::Instance()->Alloc(size);

	DuiLib::MemLooker::Instance()->AddMemInfo((long)p,DuiLib::MemInfo((file==NULL ? __FILE__:file),line,(func==NULL?__FUNCTION__:func),size));
	return p;
}
// inline void * operator new(size_t size)
// {
// 	void *p = malloc(size);
// 	if (p == NULL) throw std::bad_alloc();
// 	DuiLib::MemLooker::Instance()->Alloc(size);
// 
// 	DuiLib::MemLooker::Instance()->AddMemInfo((long)p,DuiLib::MemInfo(__FILE__,__LINE__,__FUNCTION__,size));
// 	return p;
// }
// inline void * operator new[](size_t size)
// {
// 	void *p = malloc(size);
// 	if (p == NULL) throw std::bad_alloc();
// 	DuiLib::MemLooker::Instance()->Alloc(size);
// 
// 	DuiLib::MemLooker::Instance()->AddMemInfo((long)p,DuiLib::MemInfo(__FILE__,__LINE__,__FUNCTION__,size));
// 	return p;
// }
// ---------------------------------------- operator delete 
inline void operator delete (void * pointer, size_t size)
{
	//	size_t size = sizeof(pointer);
	DuiLib::MemLooker::Instance()->Delloc(size);
	DuiLib::MemLooker::Instance()->UpdateMemInfo((long)pointer,size);
	free(pointer);
	OutputDebugString(_T("delete\n"));
}
inline void operator delete(void* pointer)
{
	OutputDebugString(_T("delete1\n"));
	byte* p = (byte*)pointer;
	size_t size = sizeof(p);
	DuiLib::MemLooker::Instance()->Delloc(size);
	DuiLib::MemLooker::Instance()->UpdateMemInfo((long)pointer,size);
	free(pointer);
}

inline void operator delete[](void* pointer)
{
	OutputDebugString(_T("delete2\n"));
	byte* p = (byte*)pointer;
	size_t size = sizeof(p);
	DuiLib::MemLooker::Instance()->Delloc(size);
	DuiLib::MemLooker::Instance()->UpdateMemInfo((long)pointer,size);
	free(pointer);
}
// ---------------------------------------- operator delete[] 
inline void operator delete[](void * pointer,size_t size)
{
	OutputDebugString(_T("delete3\n"));
//	size_t size = sizeof(pointer);
	DuiLib::MemLooker::Instance()->Delloc(size);
	DuiLib::MemLooker::Instance()->UpdateMemInfo((long)pointer,size);
	free(pointer);

}
inline void operator delete(void *p ,size_t size, const char *file, const size_t line,const char* func)
{
	OutputDebugString(_T("delete4\n"));
//	size_t size = sizeof(p);
	DuiLib::MemLooker::Instance()->Delloc(size);
	DuiLib::MemLooker::Instance()->UpdateMemInfo((long)p,size);
	free(p);
}

inline void operator delete[](void *p,size_t size, const char *file, const size_t line,const char* func)
{
	OutputDebugString(_T("delete5\n"));
//	size_t size = sizeof(p);
	DuiLib::MemLooker::Instance()->Delloc(size);
	DuiLib::MemLooker::Instance()->UpdateMemInfo((long)p,size);
	free(p);
}
*/

// #define MC_NEW  new(__FILE__, __LINE__,__FUNCTION__)
// #define new  MC_NEW


