#ifdef _WIN32
#	include <winsock2.h>
#	include <windows.h>
#endif
#include "log.h"
#include <stdarg.h>

_FStd(FAutoFile)* getLog()
{
	static _FStd(FAutoFile)* pLog = NULL;
	if (!pLog)
	{
		const char* module_path = _FStd(FGetModulePath)();
		std::string module_dir = _FStd(FGetDirectory)(module_path);
		_FStd(FUnitPath)(module_dir);
		std::string log_path = _FStd(FJoinPath)(module_dir, "log");
		if (!_FStd(FMakeDir)(log_path.c_str()))
		{
			assert(false && "Create Player Log Directory Failed!");
			return NULL;
		}
		pLog = new _FStd(FAutoFile)((log_path + "/player.log").c_str());
#if FLIB_COMPILER_WINDOWS
		::OutputDebugStringA((std::string("write log to ") + log_path).c_str());
		::OutputDebugStringA("\n");
#endif
	}
	return pLog;
}

void player_log(_FStd(FLIB_LOGLEVEL) level, const char* format, ...)
{
	va_list va;
	va_start(va, format);
	const int MAX_BUFFLEN = 20480;
	char buff[MAX_BUFFLEN + 1] = { 0 };
#if FLIB_COMPILER_CYGWIN
	vsnprintf(buff, MAX_BUFFLEN, format, va);
#elif FLIB_COMPILER_MSVC
	_vsnprintf_s(buff, MAX_BUFFLEN, format, va);
#else
	vsnprintf(buff, MAX_BUFFLEN, format, va);
#endif
	va_end(va);
	
//#ifdef _DEBUG
	std::cout << buff << std::endl;

#if FLIB_COMPILER_WINDOWS
	::OutputDebugStringA(buff);
	::OutputDebugStringA("\n");
#endif

//#endif

	if (!getLog())
		return;

	F_LOG_TO_FILE(level, *getLog()) << buff << "\n";
}