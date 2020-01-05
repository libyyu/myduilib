#ifndef __MINIDUMP_H__
#define __MINIDUMP_H__

#ifdef _MSC_VER
#pragma once

#include <windows.h>
#include <dbghelp.h>
#include <TlHelp32.h>

#pragma optimize("y", off)		//generate stack frame pointers for all functions - same as /Oy- in the project
#pragma warning(disable: 4200)	//nonstandard extension used : zero-sized array in struct/union
#pragma warning(disable: 4100)	//unreferenced formal parameter
#pragma warning(disable: 4996)

class CMiniDumper
{
public:
	typedef	BOOL(WINAPI * MINIDUMP_WRITE_DUMP)(
		IN HANDLE			hProcess,
		IN DWORD			ProcessId,
		IN HANDLE			hFile,
		IN MINIDUMP_TYPE	DumpType,
		IN CONST PMINIDUMP_EXCEPTION_INFORMATION	ExceptionParam, OPTIONAL
		IN PMINIDUMP_USER_STREAM_INFORMATION		UserStreamParam, OPTIONAL
		IN PMINIDUMP_CALLBACK_INFORMATION			CallbackParam OPTIONAL
		);
	// Tool Help functions.
	typedef	HANDLE(WINAPI * CREATE_TOOL_HELP32_SNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);
	typedef	BOOL(WINAPI * MODULE32_FIRST)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
	typedef	BOOL(WINAPI * MODULE32_NEST)(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
	CMiniDumper(TCHAR* szDir = NULL);
	~CMiniDumper();
	static TCHAR* GetAppPath();
	static TCHAR* GetAppName();
	static TCHAR* Convert(const char* s);
	static char* Convert(const wchar_t* s);
	void   AttachBugly();
	void   InitAppPath(TCHAR* szDir = NULL);
protected:
	static LONG WINAPI CrashReportEx(LPEXCEPTION_POINTERS ExceptionInfo);
	BOOL GetModuleByAddr(PBYTE Ret_Addr, TCHAR* Module_Name, PBYTE & Module_Addr);
	int  GetCallStackAddr(PEXCEPTION_POINTERS pException, TCHAR* Str);
	int  GetVersionStr(TCHAR* Str);
	TCHAR* GetExceptionInfo(PEXCEPTION_POINTERS pException, TCHAR* dumpfile);
	void  CreateDump(PEXCEPTION_POINTERS pException, BOOL File_Flag, BOOL Show_Flag);
private:
	TCHAR szDirPath[MAX_PATH];
	HMODULE	hDbgHelp;
	MINIDUMP_WRITE_DUMP	MiniDumpWriteDump_;
	CREATE_TOOL_HELP32_SNAPSHOT	CreateToolhelp32Snapshot_;
	MODULE32_FIRST	Module32First_;
	MODULE32_NEST	Module32Next_;
};

extern CMiniDumper __crash_dumper;


#endif//_MSC_VER

#endif//__MINIDUMP_H__