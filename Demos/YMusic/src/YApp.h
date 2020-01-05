#pragma once

#include "RuntimeState.h"
#include "YMainWindow.h"

class CApp : public DuiLib::Singleton<CApp>
{
	DECLARE_SINGLETON_CLASS(CApp)
	CApp();
public:
#ifdef _UNICODE 
	typedef std::wstring         string_type;
#else
	typedef std::string          string_type;
#endif
	typedef TCHAR                char_type;
	typedef bool(*CmdLineHandler)(const string_type&);
	typedef CmdLineHandler spCmdLineHandlerT;
	typedef std::shared_ptr<YMainWindow> spMainWindowT;
	typedef std::shared_ptr<CRuntimeState> spRuntimeStateT;
public:
	~CApp();
	static CApp* Instance() {
		static CApp app;
		return &app;
	}
public:
	int Run(
		HINSTANCE hInstance,
		HINSTANCE hPrevInstance,
		LPTSTR    lpCmdLine,
		int       nCmdShow);
	BOOL HandleCmdLine(LPCTSTR lpCmdLine);
	BOOL HandleCmdLine(const string_type& sCmd,const string_type& sParam);
protected:
	BOOL Init();
	BOOL Clear();
	BOOL PreHandleCmdLine(const string_type& sCmd,const string_type& sParam);
protected:
	static bool OnInitAgentClient(const string_type& sParam);
	static bool OnInstall(const string_type& sParam);
	bool OnUnInstall(const string_type& sParam);
	static bool OnPlay(const string_type& sParam);
	bool OnPause(const string_type& sParam);
	bool OnStop(const string_type& sParam);
	bool OnAddSong(const string_type& sParam);
	static bool OnUrl(const string_type& sParam);
public:
	spRuntimeStateT GetRuntimeState() const;
	spMainWindowT mainWindow() const;
public:
	string_type       GetCmdLines() const;
	string_type   GetRunMode() const;
private:
	spRuntimeStateT            _spRuntimeState;
	spMainWindowT              _spMainWin;//Ö÷´°¿Ú
	
	
	typedef std::map<string_type,spCmdLineHandlerT> spCmdLineHanderMapT;
	spCmdLineHanderMapT        _spPreCmdLineHandlerMap;
	spCmdLineHanderMapT        _spCmdLineHandlerMap;

	ULONG_PTR m_pGdiToken;

};


#define  theApp             (CApp::Instance())
#define  theRuntimeState    (theApp->GetRuntimeState())
#define  theLog             (*(theRuntimeState->GetLogMgr()))
#define  theMainWin         (theApp->mainWindow())
#define  theFilePathMgr     (theRuntimeState->GetFilePathMgr())
#define  theCacheMgr        (theRuntimeState->GetCacheMgr())
//#define  thePlayCenter      (theRuntimeState->GetPlayerCenter())
#define  thePlayer          (theRuntimeState->GetPlayer())
#define  thePlayListMgr     (theRuntimeState->GetPlayListMgr())
#define  theDbMgr           (theRuntimeState->GetDbMgr())
#define  theConfigMgr       (theRuntimeState->GetConfigMgr())
#define  theLrcMgr          (theRuntimeState->GetLrcMgr())
#define  theWebServiceMgr   (theRuntimeState->GetWebServiceMgr())
#define  theHttpDownerMgr   (theRuntimeState->GetHttpDonwer())

#define  theNet             (get_plugin_network())
/*

#define  theNet             (get_xiami_network())




#define  theCacheMgr        (theRuntimeState->GetCacheMgr())
#define  theDbMgr           (theRuntimeState->GetDbMgr())
#define  theNotifyCenter    (theRuntimeState->GetNotifyCenter())

#define  theWebServiceMgr   (theRuntimeState->GetWebServiceMgr())
#define  thePlayerCenter    (theRuntimeState->GetPlayerCenter())
#define  theFavMgr          (theRuntimeState->GetFavMgr())

*/





#define  sLogout(lev,fmt,...)  (theLog(lev,fmt,##__VA_ARGS__))

#define  sLog(fmt,...)         (sLogout(CLog::InfoLog,fmt,##__VA_ARGS__))
#define  sLogError(fmt,...)    (sLogout(CLog::ErrorLog,fmt,##__VA_ARGS__))
#define  sLogWarn(fmt,...)     (sLogout(CLog::WarningLog,fmt,##__VA_ARGS__))

