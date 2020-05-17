#define FLIB_DLL
#include "FConsole.h"
#include "FFunc.h"
#include <iostream>
#include <time.h>
#include <sstream> 
#if FLIB_COMPILER_WINDOWS
#include <io.h>
#include <fcntl.h>
#include <Windows.h>
#endif

_FStdBegin

#if FLIB_COMPILER_WINDOWS
class _FConsoleHandle
{
	bool 	  m_isAlloced;
	FLock     m_lock;

	FLIB_HANDLE 	 	m_stdOutputHandle;
	FLIB_HANDLE 	 	m_stdErrHandle;
protected:
	_FConsoleHandle() : m_isAlloced(false)
	{
		m_stdOutputHandle = 0;
		m_stdErrHandle = 0;
	}

	void RedirectIOToConsole()
	{
		lock_wrapper lock(&m_lock);
		if (m_isAlloced)
			return;

		// 分配一个控制台，以便于输出一些有用的信息
		AllocConsole();
		// 取得 STDOUT 的文件系统
		m_stdOutputHandle = (FLIB_HANDLE)GetStdHandle(STD_OUTPUT_HANDLE);
		m_stdErrHandle = (FLIB_HANDLE)GetStdHandle(STD_ERROR_HANDLE);
		// Redirect STDOUT to the new console by associating STDOUT's file 
		// descriptor with an existing operating-system file handle.
		if (m_stdOutputHandle)
		{
			int hConsoleHandle = _open_osfhandle((intptr_t)m_stdOutputHandle, _O_TEXT);
			FILE *pFile = _fdopen(hConsoleHandle, "w");
			*stdout = *pFile;
			setvbuf(stdout, NULL, _IONBF, 0);
		}
		if (m_stdErrHandle)
		{
			int hConsoleErrHandle = _open_osfhandle((intptr_t)m_stdErrHandle, _O_TEXT);
			FILE *pFileErr = _fdopen(hConsoleErrHandle, "w");
			*stderr = *pFileErr;
			setvbuf(stderr, NULL, _IONBF, 0);
		}
		// 这个调用确保 iostream 和 C run-time library 的操作在源代码中有序。 
		std::ios::sync_with_stdio();

		m_isAlloced = true;
	}
public:
	static _FConsoleHandle* get()
	{
		static _FConsoleHandle hdl;
		return &hdl;
	}

	void ColorOutput(const char* message, WORD color, bool iserror = false)
	{
		RedirectIOToConsole();
		if (iserror)
		{
			if (!m_stdErrHandle) return;
		}
		else
		{
			if (!m_stdOutputHandle) return;
		}

		if (iserror)
			::SetConsoleTextAttribute(m_stdErrHandle, color);
		else if(color != 0)
			::SetConsoleTextAttribute(m_stdOutputHandle, color);

		if (iserror)
			std::cerr << message;
		else
			std::cout << message;

		if (iserror)
			::SetConsoleTextAttribute(m_stdErrHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		else if(color != 0)
			::SetConsoleTextAttribute(m_stdOutputHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
};
#endif

static FLIB_LOGLEVEL _console_level = FLIB_LOGLEVEL::FLIB_LOGLEVEL_DEBUG;

void FSetLogConsoleLevel(FLIB_LOGLEVEL level)
{
	_console_level = level;
}

FConsole::FConsole(FLIB_LOGLEVEL level) :m_level(level)
{
	char buff[200] = { 0 };
	tm* aTm = FGetNowTime();
	sprintf(buff, "%-4d-%02d-%02d %02d:%02d:%02d",
		aTm->tm_year + 1900,
		aTm->tm_mon + 1,
		aTm->tm_mday,
		aTm->tm_hour,
		aTm->tm_min,
		aTm->tm_sec);
	*this << "["
		<< FLIB_LogLevelName[m_level] << "|"
		<< FGetCurrentThreadId() << "|"
		<< buff
		<< "] ";
}
FConsole::FConsole(FLIB_LOGLEVEL level, const char* filename, int32 line/* = -1*/) :m_level(level)
{
	char buff[200] = { 0 };
	tm* aTm = FGetNowTime();
	sprintf(buff, "%-4d-%02d-%02d %02d:%02d:%02d",
		aTm->tm_year + 1900,
		aTm->tm_mon + 1,
		aTm->tm_mday,
		aTm->tm_hour,
		aTm->tm_min,
		aTm->tm_sec);
	*this << "["
		<< FLIB_LogLevelName[m_level] << "|"
		<< FGetCurrentThreadId() << "|"
		<< buff << "|"
		<< (filename ? FGetFilename(filename).c_str() : "<unknow source>") << ":"
		<< line
		<< "] ";
}
FConsole::~FConsole() { m_message.clear(); }

void FConsole::_Logout(const char* str) { m_message += str; }
void FConsole::Finish()
{
	_LogImpl();
	m_message.clear();
}
void FConsole::_LogImpl()
{
	lock_wrapper lock(&m_lock);	
#if FLIB_COMPILER_WINDOWS
	if (m_level <= FLIB_LOGLEVEL::FLIB_LOGLEVEL_WARN)
	{
		_FConsoleHandle::get()->ColorOutput(m_message.c_str(), m_level == FLIB_LOGLEVEL::FLIB_LOGLEVEL_WARN ? (FOREGROUND_RED | FOREGROUND_GREEN) : 0, false);
	}
	else if (m_level > FLIB_LOGLEVEL::FLIB_LOGLEVEL_WARN)
	{
		_FConsoleHandle::get()->ColorOutput(m_message.c_str(), FOREGROUND_RED, true);
	}
#else
	const int MAX_BUFFLEN = 20480;
	char outbuff[MAX_BUFFLEN + 1] = { 0 };
	if (m_level == FLIB_LOGLEVEL::FLIB_LOGLEVEL_WARN)
	{
		sprintf(outbuff, "\e[1;33m%s\e[0m", m_message.c_str());
		std::cout << outbuff;
	}
	else if (m_level > FLIB_LOGLEVEL::FLIB_LOGLEVEL_WARN)
	{
		sprintf(outbuff, "\e[1;31m%s\e[0m", m_message.c_str());
		std::cerr << outbuff;
	}
	else
	{
		std::cout << m_message.c_str();
	}
#endif
}

void FConsole::output(const char* data)
{
	_Logout(data);
}

/////////////////////////////////////////////////////////////////////////////////////////
////
void FConsoleFinisher::operator=(FOStream&other)
{
	FConsole* pTarget = static_cast<FConsole*>(&other);
	pTarget->Finish();
}

FConsoleTraceFunction::FConsoleTraceFunction(FOStream& other, const char *func, const char *file, int32 line)
	: _log(other)
	, _func(func)
	, _file(file)
	, _line(line) 
{
	time_t tt;
	time(&tt);
	_begin_time = tt;
}
FConsoleTraceFunction::~FConsoleTraceFunction()
{
	char buff[200] = { 0 };
	time_t tt;
	time(&tt);
	tm* aTm = localtime(&tt);
	double cost_time = difftime(tt, (time_t)_begin_time);
	//tm* aTm = FGetNowTime();
	sprintf(buff, "%-4d-%02d-%02d %02d:%02d:%02d",
		aTm->tm_year + 1900,
		aTm->tm_mon + 1,
		aTm->tm_mday,
		aTm->tm_hour,
		aTm->tm_min,
		aTm->tm_sec);
	FConsole* pTarget = static_cast<FConsole*>(&_log);
	_log << "["
		<< FLIB_LogLevelName[pTarget->m_level] << "|"
		<< FGetCurrentThreadId() << "|"
		<< buff << "|"
		<< (_file ? FGetFilename(_file).c_str() : "<unknow source>") << ":"
		<< _line
		<< "] "
		<< _func << "() leave "
		<< "cost time " << cost_time
		<< "\n";

	*this = _log;
}
void FConsoleTraceFunction::operator=(FOStream&other)
{
	FConsole* pTarget = static_cast<FConsole*>(&other);
	pTarget->Finish();
}


_FStdEnd

