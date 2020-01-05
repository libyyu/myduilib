// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ScrCaptureWnd.h"


int RunApp(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);

    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("\\..\\skin\\ScrCapture"));
    CPaintManagerUI::SetResourceType(UILIB_RESOURCETYPE::UILIB_FILE);

    HRESULT Hr = ::CoInitialize(NULL);
    if( FAILED(Hr) ) return 0;

    CScrCaptureWnd* pFrame = CScrCaptureWnd::Instance();
    if( pFrame == NULL ) return 0;
    pFrame->Create(NULL, _T(""), WS_VISIBLE | WS_POPUP, /*WS_EX_TOOLWINDOW|WS_EX_TOPMOST*/0);
    pFrame->ShowWindow(true);
    CPaintManagerUI::MessageLoop();

    ::CoUninitialize();
    return 0;
}

#ifndef PLUGIN_MODULE
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    return RunApp(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
#else
extern "C"
{
    int __declspec(dllexport) RunScrCaptureDemo(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
    {
        return RunApp(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }
}
#endif

