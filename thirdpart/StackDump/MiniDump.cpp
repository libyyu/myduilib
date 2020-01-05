
#ifdef _MSC_VER
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <vector>
#include <string>
#include "MiniDump.h"
#include "StackWalker.h"
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")

#define	DUMP_SIZE_MAX	8000	//max size of our dump
#define	CALL_TRACE_MAX	((DUMP_SIZE_MAX - 2000) / (MAX_PATH + 40))	//max number of traced calls
#define	NL				"\r\n"	//new line

///////////////////////////////////////////////////////////////
class SimpleStackWalker : public StackWalker
{
	std::vector<std::string> stack_calls;
public:
	SimpleStackWalker() : StackWalker() {}
	SimpleStackWalker(DWORD dwProcessId, HANDLE hProcess) : StackWalker(dwProcessId, hProcess) {}
	virtual void OnStackTrace(LPCSTR szText)
	{
		stack_calls.push_back(std::string(szText));
	}
	std::vector<std::string> GetStackCalls(HANDLE hThread = NULL, const CONTEXT *context = NULL)
	{
		stack_calls.clear();
		ShowCallstack(hThread, context);
		return stack_calls;
	}
};
static SimpleStackWalker __stack_walker;
CMiniDumper __crash_dumper;
////////////////////////////////////////////////////////////////////////////////
TCHAR* CMiniDumper::GetAppPath()
{
	TCHAR s_buf[MAX_PATH];
	s_buf[0] = 0x0;
	DWORD n_result = ::GetModuleFileName(NULL, s_buf, sizeof(TCHAR)* MAX_PATH);
	static TCHAR	s_dir[MAX_PATH];
	s_dir[0] = 0x0;
	_tsplitpath(s_buf, NULL, s_dir, NULL, NULL);
	return s_dir;
}
TCHAR* CMiniDumper::GetAppName()
{
	TCHAR s_buf[MAX_PATH];
	s_buf[0] = 0x0;
	DWORD n_result = ::GetModuleFileName(NULL, s_buf, sizeof(TCHAR)* MAX_PATH);
	static TCHAR	s_name[MAX_PATH];
	s_name[0] = 0x0;
	_tsplitpath(s_buf, NULL, NULL, s_name, NULL);
	return s_name;
}
TCHAR* CMiniDumper::Convert(const char* s)
{
#ifdef _UNICODE
	static wchar_t s_buf[1024];
	s_buf[0] = 0x0;
	int length = strlen(s) + 1;
	wchar_t* t = (wchar_t*)malloc(sizeof(wchar_t)*length);
	memset(t, 0, length*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, s, strlen(s), t, length);
	lstrcpyW(s_buf, t);
	free(t);
	return s_buf;
#else
	return (TCHAR*)s;
#endif
}
char* CMiniDumper::Convert(const wchar_t* s)
{
	int len = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, NULL);
	if (len == 0)
		return NULL;
	static char s_buff[1024];
	s_buff[0] = 0x0;
	char* t = new char[len];
	WideCharToMultiByte(CP_ACP, 0, s, -1, t, len, NULL, NULL);
	strcpy(s_buff, t);
	delete[]t;
	return s_buff;
}
void   CMiniDumper::InitAppPath(TCHAR* szDir)
{
	szDirPath[0] = 0x0;
	if (NULL == szDir)
	{
		wsprintf(szDirPath, _T("%s"), GetAppPath());
	}
	else
	{
		wsprintf(szDirPath, _T("%s"), szDir);
	}
}
CMiniDumper::CMiniDumper(TCHAR* szDir) : hDbgHelp(NULL), MiniDumpWriteDump_(NULL), CreateToolhelp32Snapshot_(NULL), Module32First_(NULL), Module32Next_(NULL) 
{
	InitAppPath(szDir);
}
CMiniDumper::~CMiniDumper()
{
	if (NULL != hDbgHelp)
	{
		FreeLibrary(hDbgHelp);
		hDbgHelp = NULL;
	}
	MiniDumpWriteDump_ = NULL;
	CreateToolhelp32Snapshot_ = NULL;
	Module32First_ = NULL;
	Module32Next_ = NULL;
}
BOOL CMiniDumper::GetModuleByAddr(PBYTE Ret_Addr, TCHAR* Module_Name, PBYTE & Module_Addr)
{
	MODULEENTRY32	M = { sizeof(M) };
	HANDLE	hSnapshot;
	Module_Name[0] = 0;
	if (CreateToolhelp32Snapshot_)
	{
		hSnapshot = CreateToolhelp32Snapshot_(TH32CS_SNAPMODULE, 0);

		if ((hSnapshot != INVALID_HANDLE_VALUE) &&
			Module32First_(hSnapshot, &M))
		{
			do
			{
				if (DWORD(Ret_Addr - M.modBaseAddr) < M.modBaseSize)
				{
					lstrcpyn(Module_Name, M.szExePath, MAX_PATH);
					Module_Addr = M.modBaseAddr;
					break;
				}
			} while (Module32Next_(hSnapshot, &M));
		}

		CloseHandle(hSnapshot);
	}

	return !!Module_Name[0];
}
int  CMiniDumper::GetCallStackAddr(PEXCEPTION_POINTERS pException, TCHAR* Str)
{
	TCHAR s_traceback[1024];
	s_traceback[0] = 0x0;
	std::vector<std::string> call_stacks = __stack_walker.GetStackCalls(GetCurrentThread(), pException->ContextRecord);

	Str[0] = 0;
	int Str_Len = 0;
	/*
	TCHAR	Module_Name[MAX_PATH];
	PBYTE	Module_Addr = 0;
	PBYTE	Module_Addr_1;

	typedef struct STACK
	{
		STACK *	Ebp;
		PBYTE	Ret_Addr;
		DWORD	Param[0];
	} STACK, *PSTACK;

	STACK	Stack = { 0, 0 };
	PSTACK	Ebp;

	if (pException)		//fake frame for exception address
	{
		Stack.Ebp = (PSTACK)pException->ContextRecord->Ebp;
		Stack.Ret_Addr = (PBYTE)pException->ExceptionRecord->ExceptionAddress;
		Ebp = &Stack;
	}
	else
	{
		Ebp = (PSTACK)&pException - 1;	//frame addr of Get_Call_Stack()

		// Skip frame of Get_Call_Stack().
		if (!IsBadReadPtr(Ebp, sizeof(PSTACK)))
			Ebp = Ebp->Ebp;		//caller ebp
	}

	// Trace CALL_TRACE_MAX calls maximum - not to exceed DUMP_SIZE_MAX.
	// Break trace on wrong stack frame.
	for (int Ret_Addr_I = 0;
		(Ret_Addr_I < CALL_TRACE_MAX) && !IsBadReadPtr(Ebp, sizeof(PSTACK)) && !IsBadCodePtr(FARPROC(Ebp->Ret_Addr));
		Ret_Addr_I++, Ebp = Ebp->Ebp)
	{
		// If module with Ebp->Ret_Addr found.
		if (GetModuleByAddr(Ebp->Ret_Addr, Module_Name, Module_Addr_1))
		{
			if (Module_Addr_1 != Module_Addr)	//new module
			{
				// Save module's address and full path.
				Module_Addr = Module_Addr_1;
				Str_Len += wsprintf(Str + Str_Len, _T("\r\n%08X  %s"), Module_Addr, Module_Name);
			}

			// Save call offset.
			Str_Len += wsprintf(Str + Str_Len, _T("\r\n  +%08X"), Ebp->Ret_Addr - Module_Addr);

			// Save 5 params of the call. We don't know the real number of params.
			if (pException && !Ret_Addr_I)	//fake frame for exception address
				Str_Len += wsprintf(Str + Str_Len, _T("  Exception Offset"));
			else if (!IsBadReadPtr(Ebp, sizeof(PSTACK)+5 * sizeof(DWORD)))
			{
				Str_Len += wsprintf(Str + Str_Len, _T("  (%X, %X, %X, %X, %X)"),
					Ebp->Param[0], Ebp->Param[1], Ebp->Param[2], Ebp->Param[3], Ebp->Param[4]);
			}
		}
		else
			Str_Len += wsprintf(Str + Str_Len, _T("\r\n%08X"), Ebp->Ret_Addr);
	}*/

	for (std::vector<std::string>::iterator it = call_stacks.begin(); it != call_stacks.end(); ++it)
	{
		Str_Len += wsprintf(Str + Str_Len, _T("%s\r\n"), Convert((*it).c_str()));
	}

	return Str_Len;

}
int  CMiniDumper::GetVersionStr(TCHAR* Str)
{
	OSVERSIONINFOEX	V = {sizeof(OSVERSIONINFOEX)};	//EX for NT 5.0 and later
	if (!GetVersionEx((POSVERSIONINFO)&V))
	{
		ZeroMemory(&V, sizeof(V));
		V.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx((POSVERSIONINFO)&V);
	}

	if (V.dwPlatformId != VER_PLATFORM_WIN32_NT)
		V.dwBuildNumber = LOWORD(V.dwBuildNumber);	//for 9x HIWORD(dwBuildNumber) = 0x04xx

	return wsprintf(Str, _T("\r\nWindows:  %d.%d.%d, SP %d.%d, Product Type %d"),	//SP - service pack, Product Type - VER_NT_WORKSTATION,...
		V.dwMajorVersion, V.dwMinorVersion, V.dwBuildNumber, V.wServicePackMajor, V.wServicePackMinor/*, V.wProductType*/);
}
TCHAR* CMiniDumper::GetExceptionInfo(PEXCEPTION_POINTERS pException, TCHAR* dumpfile)
{
	TCHAR*		Str;
	int			Str_Len;
	int			i;
	TCHAR		Module_Name[MAX_PATH];
	PBYTE		Module_Addr;
	HANDLE		hFile;
	FILETIME	Last_Write_Time;
	FILETIME	Local_File_Time;
	SYSTEMTIME	T;

	Str = new TCHAR[DUMP_SIZE_MAX];

	if (!Str)
		return NULL;

	Str_Len = 0;
	Str_Len += GetVersionStr(Str + Str_Len);

	Str_Len += wsprintf(Str + Str_Len, _T("\r\nProcess:  "));
	GetModuleFileName(NULL, Str + Str_Len, MAX_PATH);
	Str_Len = lstrlen(Str);

	// If exception occurred.
	if (pException)
	{
		EXCEPTION_RECORD &	E = *pException->ExceptionRecord;
		CONTEXT &			C = *pException->ContextRecord;

		// If module with E.ExceptionAddress found - save its path and date.
		if (GetModuleByAddr((PBYTE)E.ExceptionAddress, Module_Name, Module_Addr))
		{
			Str_Len += wsprintf(Str + Str_Len, _T("\r\nModule:  %s"), Module_Name);

			if ((hFile = CreateFile(Module_Name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
			{
				if (GetFileTime(hFile, NULL, NULL, &Last_Write_Time))
				{
					FileTimeToLocalFileTime(&Last_Write_Time, &Local_File_Time);
					FileTimeToSystemTime(&Local_File_Time, &T);

					Str_Len += wsprintf(Str + Str_Len, _T("\r\nDate Modified:  %02d/%02d/%d"), T.wMonth, T.wDay, T.wYear);
				}
				CloseHandle(hFile);
			}
		}
		else
		{
			Str_Len += wsprintf(Str + Str_Len, _T("\r\nException Addr:  %08X"), E.ExceptionAddress);
		}

		Str_Len += wsprintf(Str + Str_Len, _T("\r\nException Code:  %08X"), E.ExceptionCode);

		if (E.ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
		{
			// Access violation type - Write/Read.
			Str_Len += wsprintf(Str + Str_Len,
				_T("\r\n%s Address:  %08X"),
				(E.ExceptionInformation[0]) ? _T("Write") : _T("Read"), E.ExceptionInformation[1]);
		}

		// Save instruction that caused exception.
		Str_Len += wsprintf(Str + Str_Len, _T("\r\nInstruction: "));
		for (i = 0; i < 16; i++)
			Str_Len += wsprintf(Str + Str_Len, _T(" %02X"), PBYTE(E.ExceptionAddress)[i]);

		// Save registers at exception.
#ifdef _WIN64
#else
		Str_Len += wsprintf(Str + Str_Len, _T("\r\nRegisters:"));
		Str_Len += wsprintf(Str + Str_Len, _T("\r\nEAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X"), C.Eax, C.Ebx, C.Ecx, C.Edx);
		Str_Len += wsprintf(Str + Str_Len, _T("\r\nESI: %08X  EDI: %08X  ESP: %08X  EBP: %08X"), C.Esi, C.Edi, C.Esp, C.Ebp);
		Str_Len += wsprintf(Str + Str_Len, _T("\r\nEIP: %08X  EFlags: %08X"), C.Eip, C.EFlags);
#endif
	} //if (pException)

	// Save call stack info.
	Str_Len += wsprintf(Str + Str_Len, _T("\r\nDUMPFILE: %s"), dumpfile);
	Str_Len += wsprintf(Str + Str_Len, _T("\r\nCall Stack:\r\n"));
	GetCallStackAddr(pException, Str + Str_Len);

	if (Str[0] == NL[0])
		lstrcpy(Str, Str + sizeof(NL)-1);

	return Str;
}

void  CMiniDumper::CreateDump(PEXCEPTION_POINTERS pException, BOOL File_Flag, BOOL Show_Flag)
{
	HANDLE	hDump_File;
	TCHAR*	Str;
	DWORD	Bytes;
	DWORD	nLen = 0;

	TCHAR strDir[MAX_PATH] = { 0 }, strTXTFile[MAX_PATH] = { 0 }, strDMPFile[MAX_PATH] = { 0 };
	TCHAR strDate[MAX_PATH] = { 0 }, strTotal[MAX_PATH] = { 0 } ;
	time_t n_now = time(NULL);
	struct tm* p_time = localtime(&n_now);
	TCHAR s_format[256];
	s_format[0] = 0x0;
	_tcsftime(s_format, 255, _T("%Y.%m.%d %H-%M-%S"), p_time);

	wsprintf(strDir, _T("%s\\Log"), szDirPath);
	wsprintf(strTXTFile, _T("%s\\Log\\%s.txt"), szDirPath, s_format);
	wsprintf(strDMPFile, _T("%s\\Log\\%s.dmp"), szDirPath, s_format);

	if (!PathFileExists(strDir))
		CreateDirectory(strDir, NULL);

	Str = GetExceptionInfo(pException, strDMPFile);

	if (Show_Flag && Str)
	{
		TCHAR s_title[256];
		s_title[0] = 0x0;
		wsprintf(s_title, _T("%s-MiniDump"), GetAppName());
		MessageBox(NULL, Str, s_title, MB_ICONHAND | MB_OK);
	}

	if (File_Flag)
	{
		if (Str)
		{
			hDump_File = CreateFile(strTXTFile,
				GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			nLen = lstrlen(Str);
			Str[nLen] = '\0';
#ifndef _UNICODE
			WriteFile(hDump_File, Str, lstrlen(Str) + 1, &Bytes, NULL);
#else
			char* text = Convert(Str);
			WriteFile(hDump_File, text, strlen(text) + 1, &Bytes, NULL);
#endif

			CloseHandle(hDump_File);
		}

		// If MiniDumpWriteDump() of DbgHelp.dll available.
		if (MiniDumpWriteDump_)
		{
			MINIDUMP_EXCEPTION_INFORMATION	M;

			M.ThreadId = GetCurrentThreadId();
			M.ExceptionPointers = pException;
			M.ClientPointers = 0;

			hDump_File = CreateFile(strDMPFile,
				GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			MiniDumpWriteDump_(GetCurrentProcess(), GetCurrentProcessId(), hDump_File,
				MiniDumpNormal, (pException) ? &M : NULL, NULL, NULL);

			CloseHandle(hDump_File);
		}
	} //if (File_Flag)

	delete Str;
}

///////////////////////////////////////////////////////////////////
LONG WINAPI CMiniDumper::CrashReportEx(LPEXCEPTION_POINTERS ExceptionInfo)
{
	// 创建DUMP文件
	__crash_dumper.CreateDump(ExceptionInfo, 1, 1);

	return EXCEPTION_EXECUTE_HANDLER;
}

void CMiniDumper::AttachBugly()
{
	// Try to get MiniDumpWriteDump() address.
	hDbgHelp = LoadLibrary(_T("DBGHELP.DLL"));
	if (NULL == hDbgHelp)
	{
		printf("DBGHELP.DLL not found. Please install a DBGHELP.DLL.\n");
		return;
	}
	MiniDumpWriteDump_ = (MINIDUMP_WRITE_DUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
	if (NULL == MiniDumpWriteDump_)
	{
		printf("DBGHELP.DLL found is too old. Please upgrade to a newer version of DBGHELP.DLL.\n");
		return;
	}
	// Try to get Tool Help library functions.
	HMODULE	hKernel32 = GetModuleHandle(_T("KERNEL32"));
	if (NULL == hKernel32)
	{
		printf("Module hKernel32 not found. Please install a KERNEL32.DLL.\n");
		return;
	}
	CreateToolhelp32Snapshot_ = (CREATE_TOOL_HELP32_SNAPSHOT)GetProcAddress(hKernel32, "CreateToolhelp32Snapshot");
	if (NULL == CreateToolhelp32Snapshot_)
	{
		printf("KERNEL32.DLL found is too old. Please upgrade to a newer version of KERNEL32.DLL.\n");
		return;
	}
	Module32First_ = (MODULE32_FIRST)GetProcAddress(hKernel32, "Module32First");
	if (NULL == Module32First_)
	{
		printf("KERNEL32.DLL found is too old. Please upgrade to a newer version of KERNEL32.DLL.\n");
		return;
	}
	Module32Next_ = (MODULE32_NEST)GetProcAddress(hKernel32, "Module32Next");
	if (NULL == Module32Next_)
	{
		printf("KERNEL32.DLL found is too old. Please upgrade to a newer version of KERNEL32.DLL.\n");
		return;
	}

	// TODO: Add extra initialization here
	SetUnhandledExceptionFilter(CrashReportEx);
	
}

#endif//_MSC_VER

extern "C"
{
	__declspec(dllexport) void AttachBugly(const char* szPath, int len)
	{
#ifdef _MSC_VER

		if (!szPath || len == 0)
			__crash_dumper.InitAppPath(NULL);
		else
		{
			char sz[1024];
			strncpy(sz, szPath, len);
			sz[len] = 0;
			__crash_dumper.InitAppPath(CMiniDumper::Convert(sz));
		}
		
		__crash_dumper.AttachBugly();
#endif
	}
}