#pragma once

#include "resource.h"
#include <memory>
#include <curl/curl.h>
#include "UIlib.h"
//#include "MyControls/MyControls.h"
//#include "BaseWindow/BaseWindow.h"
//#include "Utilty/Convert/Convert.h"

typedef   DuiLib::CDuiString   YString;
#define  _count_(x)  (sizeof(x)/sizeof(*x))
#define  YTrace_ _consoletrace
void inline _consoletrace(LPCTSTR pstrFormat,...)
{
#if defined(_DEBUG)
	TCHAR szBuffer[300] = { 0 };
	va_list args;
	va_start(args, pstrFormat);
	::wvnsprintf(szBuffer, _count_(szBuffer) - 2, pstrFormat, args);
//		_tcscat(szBuffer, _T("\n"));
	va_end(args);

	time_t t = time(NULL);
	tm* aTm = localtime(&t);

	YString sTrDebug;
	sTrDebug.Format(_T("\n[%-4d-%02d-%02d %02d:%02d:%02d][%lX]|[%s] "),
		aTm->tm_year+1900,
		aTm->tm_mon+1,
		aTm->tm_mday,
		aTm->tm_hour,
		aTm->tm_min,
		aTm->tm_sec,
		::GetCurrentThreadId(),
		szBuffer);
	::OutputDebugString(sTrDebug);
#endif
}

DuiLib::CDuiString TransToDuiString(const char* u8);
std::string TransToStdString(const DuiLib::CDuiString& ds);

//#include "Variable/Var.h"
//
//#include "Utilty/Thread.h"
//#include "Utilty/log/log.h"
//#include "Utilty/win_io/FileMgr.h"
//#include "Utilty/down_http_file.h"
//#include "Utilty/UrlPraser.h"
//#include "DB/ShareData.h"
//#include "DB/Database.h"
//#include "DB/DatabaseManager.h"
//
//#include "Login/YLogin.h"
//#include "MenuWindow/MainMenu.h"
//#include "SkinWindow/SkinWindow.h"
//
//#include "WebService/md5_checksum.h"
//
//
//#include "CacheMgr/CacheMgr.h"
//#include "FilePathMgr/FilePathMgr.h"
////#include "PlayMgr/PlayCenter.h"
//
//#include "PlayMgr/AVPlayer.h"
//#include "PlayMgr/PlayListMgr.h"
//
//
//#include "PlayListItem/YPlayListItem.h"
//#include "SongListItem/YSongListItem.h"
//
//#include "Setting/ConfigMgr.h"
//#include "LrcMgr/LrcMgr.h"
//
//#include "Web/HttpManager.h"
//#include "Web/ServerManager.h"
//
//#include "WebService/WebService.h"
//
//#include "BaseWindow/InfoWindow.h"
//#include "BaseWindow/trayIcon.h"


