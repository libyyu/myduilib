// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once


#include "targetver.h"

#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers 
#include <afxwin.h> // MFC core and standard components 
#include <afxext.h> // MFC extensions 
#include <afxdisp.h> // MFC Automation classes 
#include <afxdtctl.h> // MFC support for Internet Explorer 4 Common Controls 
#ifndef _AFX_NO_AFXCMN_SUPPORT 
#include <afxcmn.h> // MFC support for Windows Common Controls 
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxmt.h>
#include <afxsock.h>
/*#include <WinSock2.h>*/
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#include <Mmsystem.h>
// #define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// // Windows ͷ�ļ�:
// #include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <wchar.h> 
#include <deque>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlstr.h>

#include "resource.h"
// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "UIlib.h"
using namespace DuiLib;

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 5556
#define THREAD_SLEEP 500
#define TO_DUISTRING(s) DuiLib::toDuiString(s)
#define TO_STDSTRING(s) DuiLib::toStdString(s)

#ifdef _UNICODE
#define MYSTDSTRING std::wstring
#else
#define MYSTDSTRING std::string
#endif

namespace DuiLib
{
	CDuiString toDuiString(const char* arg);
	std::string toStdString(const LPCTSTR str);
}


#include "Convert.h"
using namespace FConvert;