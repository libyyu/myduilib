#include "StdAfx.h"

namespace DuiLib
{
	ErrRet DisplayError(const TCHAR* errorTitle,
		const TCHAR* errorText,
		const TCHAR* errorDescription,
		const TCHAR* fileName,
		int lineNumber)
	{
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN64__)
		const int MODULE_NAME_SIZE = 255;
		TCHAR moduleName[MODULE_NAME_SIZE] = { 0 };

		// attempt to get the module name
		if (!GetModuleFileName(NULL, moduleName, MODULE_NAME_SIZE))
		{
			TCHAR *msg = _T("<unknown application>");
			_tcscpy_s(moduleName, _tcsclen(msg), msg);
		}

		// build a collosal string containing the entire asster message
		const	int		MAX_BUFFER_SIZE = 1024;
		TCHAR	buffer[MAX_BUFFER_SIZE] = { 0 };

		int Size = _stprintf(buffer,
			_T("%s\n\nProgram : %s\nFile : %s\nLine : %d\nError: %s\nComment: %s\n \
			Abort to exit (or debug), Retry to continue,\n \
			Ignore to disregard all occurances of this error\n"),
			errorTitle,
			moduleName,
			fileName,
			lineNumber,
			errorText,
			errorDescription);


		// place a copy of the message into the clipboard
		if (OpenClipboard(NULL))
		{
			
			size_t bufferLength = _tcsclen(buffer);
			HGLOBAL hMem = GlobalAlloc(GHND|GMEM_DDESHARE, bufferLength+1);

			if (hMem)
			{
				BYTE* pMem = (BYTE*)GlobalLock(hMem);
				memcpy(pMem, buffer, bufferLength);
				GlobalUnlock(hMem);
				EmptyClipboard();
				SetClipboardData(CF_TEXT, hMem);
			}

			CloseClipboard();
		}


		// find the top most window of the current application
		HWND hWndParent = GetActiveWindow();
		if (NULL != hWndParent)
		{
			hWndParent = GetLastActivePopup(hWndParent);
		}

		// put up a message box with the error
		int iRet = MessageBox ( hWndParent,
			buffer,
			_T( "ERROR NOTIFICATION..." ),
			MB_TASKMODAL|MB_SETFOREGROUND|MB_ABORTRETRYIGNORE|MB_ICONERROR);

		// Figure out what to do on the return.
		if (iRet == IDRETRY)
		{
			// ignore this error and continue
			return (ERRRET_CONTINUE);
		}
		if (iRet == IDIGNORE)
		{
			// ignore this error and continue,
			// plus never stop on this error again (handled by the caller)
			return (ERRRET_IGNORE);
		}

		// The return has to be IDABORT, but does the user want to enter the debugger
		// or just exit the application?
		iRet = MessageBox(	hWndParent,
			_T("Do you wish to debug the last error?"),
			_T( "DEBUG OR EXIT?" ),
			MB_TASKMODAL|MB_SETFOREGROUND|MB_YESNO|MB_ICONQUESTION);

		if (iRet == IDYES)
		{
			// inform the caller to break on the current line of execution
			return (ERRRET_BREAKPOINT);
		}

		// must be a full-on termination of the app
		ExitProcess ( (UINT)-1 ) ;
		return (ERRRET_ABORT);

#else

		return (ERRRET_BREAKPOINT);
#endif
	}

	ErrRet NotifyAssert(const TCHAR* condition, const TCHAR* fileName, int lineNumber, const TCHAR* formats, ...)
	{
		TCHAR szBuffer[4096] = { 0 };

		va_list args;
		va_start(args, formats);
#ifdef _UNICODE
		vswprintf(szBuffer, formats, args);
#else
		vsprintf(szBuffer, formats, args);
#endif
		va_end(args);

		// pass the data on to the message box
		ErrRet result = DisplayError(_T("Assert Failed!"),
			condition,
			szBuffer,
			fileName,
			lineNumber);
		return result;
	}
}