// Client.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Client.h"
#include "RichEditUtil.h"

namespace DuiLib
{
	extern IClassFactory* g_pcf;
}

int RunApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CDuiString szRootPath = TODUISTRING(ROOT_DIR).c_str();
	CPaintManagerUI::SetInstance(hInstance);
#ifdef _DEBUG
	CPaintManagerUI::SetResourcePath(Path::CombinePath(szRootPath.GetData(), _T("skin\\Cli")));
	CPaintManagerUI::SetResourceType(UILIB_RESOURCETYPE::UILIB_FILE);
#else
	CPaintManagerUI::SetResourcePath(Path::CombinePath(szRootPath.GetData(), _T("skin")));
	CPaintManagerUI::SetResourceZip(_T("Cli.dat"));
	CPaintManagerUI::SetResourceType(UILIB_RESOURCETYPE::UILIB_ZIP);
#endif

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return 0;
	Hr = ::OleInitialize(NULL);
	if (FAILED(Hr)) return 0;
	DuiLib::FGetClassObject pFunc = RegisterComDll(CPaintManagerUI::GetInstancePath() + _T("\\ImageOleCtrl.dll"));
	if (!pFunc)
	{
		pFunc = RegisterComDll(CPaintManagerUI::GetInstancePath() + _T("\\plugins\\ImageOleCtrl.dll"));
	}
	if(!pFunc)
		MessageBox(NULL, _T("加载ImageOleCtrl.dll出错"), _T("Error"), MB_OK | MB_ICONERROR);
	else
	{
		HRESULT hr = pFunc(CLSID_ImageOle, IID_IClassFactory, (void **)&g_pcf);
	}

	if (!AfxSocketInit())
	{
		MessageBox(NULL, _T("加载Com出错"), _T("Error"), MB_OK | MB_ICONERROR);
		return 0;
	}
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		MessageBox(NULL, _T("无法提供主线程窗口"), _T("Error"), MB_OK | MB_ICONERROR);
		return 0;
	}
	theApp = new CMainWindow();
	if (theApp == NULL) return 0;
	theApp->Create(NULL, _T("虾聊2013"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	theApp->CenterWindow();
	theApp->ShowWindow(true);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	::OleUninitialize();
	return 0;
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	return RunApp(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}

extern "C"
{
	int __declspec(dllexport) RunClientDemo(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		return RunApp(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	}
}