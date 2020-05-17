#ifndef __FLIB_CONSOLE_H_
#define __FLIB_CONSOLE_H_
#pragma once
#include "FLock.h"
#include "FIOStream.h"

_FStdBegin
//static FLIB_LOGLEVEL _console_level = FLIB_LOGLEVEL::FLIB_LOGLEVEL_DEBUG;

class FConsoleFinisher;
class FConsoleTraceFunction;
class F_DLL_API FConsole : public FOStreamMaker
{
	virtual void output(const char* data);
public:
	FConsole(FLIB_LOGLEVEL level);
	FConsole(FLIB_LOGLEVEL level, const char* filename, int32 line = -1);
	virtual ~FConsole();

private:
	FLock     m_lock;
	std::string m_message;
    FLIB_LOGLEVEL m_level;
	friend class FConsoleFinisher;
    friend class FConsoleTraceFunction;
protected:
	void _Logout(const char* str);
	inline void Finish();
	virtual void _LogImpl();
};

/////////////////////////////////////////////////////////////////////////////////////////
////
class F_DLL_API FConsoleFinisher
{
public:
	void operator=(FOStream&other);
};

class F_DLL_API FConsoleTraceFunction
{
public:
	FConsoleTraceFunction(FOStream& other, const char *func, const char *file, int32 line);
	~FConsoleTraceFunction();
	void operator=(FOStream&other);
private:
	FOStream& _log;
	const char *_func;
	const char *_file;
	int32 _line;
};

F_DLL_API void FSetLogConsoleLevel(FLIB_LOGLEVEL level);

_FStdEnd

/////////////////////////////////////////////////////////////////////////////////////////
////
#define F_CONSOLE(LEVEL) \
	FStd::FConsoleFinisher() = FStd::FConsole(FStd::FLIB_LOGLEVEL::FLIB_LOGLEVEL_##LEVEL, __FILE__, __LINE__)

#define F_CONSOLE_TRACE  \
	FStd::FConsole f_console_trace(FStd::FLIB_LOGLEVEL::FLIB_LOGLEVEL_TRACE, __FILE__, __LINE__);  \
	FStd::FConsoleTraceFunction f_consoleTraceFunction(f_console_trace, __FUNCTION__, __FILE__, __LINE__); \
	f_consoleTraceFunction = f_console_trace << __FUNCTION__ << "() enter \n";

#endif//__FLIB_CONSOLE_H_
