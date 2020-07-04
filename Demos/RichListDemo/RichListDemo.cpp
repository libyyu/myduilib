// RichListDemo.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <exdisp.h>
#include <comdef.h>
#include "RichListWnd.h"

int RunApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	DuiLib::CDuiString szRootPath = TODUISTRING(ROOT_DIR).c_str();
	DuiLib::CPaintManagerUI::SetInstance(hInstance);
	DuiLib::CPaintManagerUI::SetResourcePath(DuiLib::Path::CombinePath(szRootPath.GetData(), _T("skin")));
	DuiLib::CPaintManagerUI::SetResourceType(UILIB_RESOURCETYPE::UILIB_ZIP);
	DuiLib::CPaintManagerUI::SetResourceZip(_T("RichListRes.zip"));

	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr)) return 0;

	CRichListWnd* pFrame = new CRichListWnd();
	if (pFrame == NULL) return 0;
	pFrame->Create(NULL, _T("DuiLib Demo from mbstudio.cn / meineson"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 500, 400);
	pFrame->CenterWindow();
	::ShowWindow(*pFrame, SW_SHOW);

	CPaintManagerUI::MessageLoop();

	::CoUninitialize();
	return 0;
}

#ifndef PLUGIN_MODULE
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return RunApp(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#else
extern "C"
{
	int __declspec(dllexport) RunRichListDemo(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	{
		return RunApp(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	}
}
#endif

