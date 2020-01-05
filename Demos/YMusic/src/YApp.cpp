
#include "stdafx.h"
#include "YApp.h"
#include "Utility/Convert.h"

#include <GdiPlus.h>
using namespace Gdiplus;
#pragma  comment(lib,  "gdiplus.lib")
#include <MyControls/MyControls.h>

CApp::~CApp()
{
	Clear();
}
BOOL CApp::Init()
{
	ASSERT(theApp);

	_spPreCmdLineHandlerMap.insert(spCmdLineHanderMapT::value_type(_T("-agent"),&CApp::OnInitAgentClient));

	_spCmdLineHandlerMap.insert(spCmdLineHanderMapT::value_type(_T("xiating"),&CApp::OnUrl));
	_spCmdLineHandlerMap.insert(spCmdLineHanderMapT::value_type(_T("-install"),&CApp::OnInstall));

	_spCmdLineHandlerMap.insert(spCmdLineHanderMapT::value_type(_T("-play"),&CApp::OnPlay));

	m_pGdiToken = NULL;

	_spRuntimeState = spRuntimeStateT(new CRuntimeState());
	_spRuntimeState->Init(::GetCurrentThreadId());
	ASSERT(theRuntimeState);

	Gdiplus::GdiplusStartupInput GdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_pGdiToken, &GdiplusStartupInput, NULL);

	return TRUE;
}

BOOL CApp::Clear()
{
	Gdiplus::GdiplusShutdown(m_pGdiToken);

	return TRUE;
}

bool OpenP2PStream()
{
	{ 
		HANDLE theMutex = OpenMutex(READ_CONTROL, FALSE, _T("Global\\XIAMI_P2PLIB_MUTEX"));
		assert(theMutex);
		if (theMutex)
		{
			::CloseHandle(theMutex);
			if (ERROR_ALREADY_EXISTS == ::GetLastError())
			{
				return true;
			}
		}
	}

	if(DuiLib::CSystem::Instance()->IsAdmin() || !DuiLib::CSystem::Instance()->IsVista())
	{
		DuiLib::CDuiString sPath = em_utility::mci::get_module_path().c_str();
		DuiLib::CDuiString sP2PStreamName = sPath + _T("p2pstream.exe");

		SHELLEXECUTEINFO ShellInfo = {0};
		ShellInfo.cbSize = sizeof(ShellInfo);
		ShellInfo.hwnd   = NULL;
		ShellInfo.lpVerb = _T("open");
		ShellInfo.lpFile = sP2PStreamName.GetData();
		ShellInfo.nShow  = SW_SHOWNORMAL;
		ShellInfo.fMask  = SEE_MASK_NOCLOSEPROCESS;
		ShellInfo.lpParameters = _T("/install");
		if(::ShellExecuteEx(&ShellInfo))
		{
			system("net start xiamiP2PStream");
			
			return true;
		}
//		return true;
	}

	DuiLib::CSystem::Instance()->ReStartApp(NULL,TRUE);
	return false;
}

CApp::CApp():_spRuntimeState(NULL), m_pGdiToken(NULL)
{
	bIsRunByLua = false;
}

int CApp::Run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{

	if(!Init())
		return 0;

// 	if(!OpenP2PStream())
// 		return 0;
	
	sLog(_T("WinMain hInstance:%d,cmdLine:%s,cmdShow:%d"),hInstance,lpCmdLine,nCmdShow);
	sLog(_T("RunMode %s"),GetRunMode().c_str());

	string_type cmdLine(lpCmdLine);
	string_type::size_type nbegin = cmdLine.find_first_of(_T("��"));
	string_type cmd;
	string_type param;
	if(nbegin  != string_type::npos)
	{
		cmd.swap(cmdLine.substr(0,nbegin));
		if(nbegin < cmdLine.size())
		{
			param.swap(cmdLine.substr(nbegin+1));
		}
	}
	//����������
	sLog(_T("PreHandleCmdLine(%s,%s)"),cmd.c_str(),param.c_str());
	if(PreHandleCmdLine(cmd,param))
		return 1;
	//ȷ��ֻ����һ������
	HANDLE theMutex = ::CreateMutex(NULL, FALSE, _T("YMusic"));
	assert(theMutex);
	//CloseHandle(theMutex);
	if(ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		HWND hwnd = FindWindow(YMainWindow::GetWindowClsss(), NULL);
		ASSERT(hwnd);
		if(hwnd)
		{
			COPYDATASTRUCT cpd;
			cpd.dwData = 0;
			cpd.cbData = (_tcslen(lpCmdLine) + 1)*sizeof(TCHAR);
			cpd.lpData = (void*)lpCmdLine;

			BOOL bret = ::SendMessageTimeout(hwnd,WM_COPYDATA,(WPARAM)hInstance,(LPARAM)&cpd,SMTO_BLOCK,1000*10,NULL);
			ASSERT(bret);

			::ShowWindow(hwnd,SW_SHOWNORMAL);
			::SetActiveWindow(hwnd);
			::BringWindowToTop(hwnd);

			return 1;
		}
		return -1;
	}

	HRESULT Hr = ::CoInitialize(NULL);
	if( FAILED(Hr) ) return 0;

//	theCrashDumper.Enable(_T("xiating"),true);
//	CWSAStartup wsa(2,1);

	

	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	DuiLib::CPaintManagerUI::SetResourceType(DuiLib::UILIB_FILE);
	DuiLib::CPaintManagerUI::SetResourcePath(DuiLib::CPaintManagerUI::GetInstancePath() + _T("/../skin/ymusic"));
	//����������
	HandleCmdLine(lpCmdLine);
	
	_spMainWin = spMainWindowT(new YMainWindow);
	ASSERT(_spMainWin != NULL);
	if(!_spMainWin) return 0;

	_spMainWin->Create(NULL, _T("����"),UI_WNDSTYLE_FRAME,/* WS_EX_STATICEDGE | WS_EX_APPWINDOW*/WS_EX_WINDOWEDGE | WS_EX_OVERLAPPEDWINDOW);
	_spMainWin->CenterWindow();
	_spMainWin->ShowWindow(true);

	sLog(_T("Register Player"));
	thePlayer->SetHWND(_spMainWin->GetHWND());

	sLog(_T("Start MessageLoop"));
	DuiLib::CPaintManagerUI::MessageLoop();

//	_spRuntimeState->UnInit();
//	_spRuntimeState.reset();

	::CoUninitialize();

	return 0;
}

BOOL CApp::PreHandleCmdLine(const string_type& sCmd,const string_type& sParam)
{
	spCmdLineHanderMapT::iterator it = _spPreCmdLineHandlerMap.find(sCmd);
	if(_spPreCmdLineHandlerMap.end() != it && (it->second))
	{
		return it->second(sParam);
	}

	return FALSE;
}
BOOL CApp::HandleCmdLine(LPCTSTR lpCmdLine)
{
	string_type cmdLine(lpCmdLine);
	string_type::size_type nbegin = cmdLine.find_first_of(_T(':'));
	string_type cmd;
	string_type param;
	if(nbegin  != string_type::npos)
	{
		cmd.swap(cmdLine.substr(0,nbegin));
		if(nbegin < cmdLine.size())
		{
			param.swap(cmdLine.substr(nbegin+1));
		}
	}
	return HandleCmdLine(cmd,param);
}
BOOL CApp::HandleCmdLine(const string_type& sCmd,const string_type& sParam)
{
	spCmdLineHanderMapT::iterator it = _spCmdLineHandlerMap.find(sCmd);
	if(_spCmdLineHandlerMap.end() != it && it->second)
	{
		return it->second(sParam);
	}

	return FALSE;
}

CApp::spRuntimeStateT CApp::GetRuntimeState() const
{
	return _spRuntimeState;
}

CApp::spMainWindowT CApp::mainWindow() const
{
	return _spMainWin;
}



CApp::string_type CApp::GetCmdLines() const
{
	return _T("");
}
CApp::string_type CApp::GetRunMode() const
{
	string_type sMode;
#ifdef _DEBUG
	sMode += string_type(_T("Debug|"));
#else 
	sMode += string_type(_T("Release|"));
#endif

#ifdef _UNICODE
	sMode += string_type(_T("Unicode"));
#else 
	sMode += string_type(_T("MultiChar"));
#endif
	return sMode;
}

////
bool CApp::OnInitAgentClient(const string_type& sParam)
{
	int nPort = 0;
	string_type ssParam = string_type(sParam);
	if(1 != _stscanf(ssParam.c_str(),_T("%d"),&nPort))
		return FALSE;
	if(!nPort)
		return FALSE;

	HANDLE theMutex = ::CreateMutex(NULL, TRUE, _T("YMusic_Agent"));
	assert(theMutex);
	::CloseHandle(theMutex);
	if(ERROR_ALREADY_EXISTS == ::GetLastError())
	{
		return TRUE;
	}

//	InitAgentClient(nPort);
	return TRUE;
}
bool CApp::OnInstall(const string_type& sParam)
{

	return TRUE;
}
bool CApp::OnUnInstall(const string_type& sParam)
{
	return FALSE;
}

bool CApp::OnPlay(const string_type& sParam)
{	
	return TRUE;
}
bool CApp::OnPause(const string_type& sParam)
{

	return TRUE;
}
bool CApp::OnStop(const string_type& sParam)
{

	return TRUE;
}
bool CApp::OnAddSong(const string_type& sParam)
{

	return TRUE;
}
bool CApp::OnUrl(const string_type& sParam)
{
	return FALSE;
}

/************************************************************************************************/

