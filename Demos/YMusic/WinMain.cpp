#include "stdafx.h"
#include "YApp.h"


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	theApp->Run(hInstance,
				hPrevInstance,
				lpCmdLine,
				nCmdShow);

	return 0;
}