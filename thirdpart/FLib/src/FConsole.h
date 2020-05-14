#ifndef __FLIB_CONSOLE_H_
#define __FLIB_CONSOLE_H_
#pragma once
#include "FLock.h"

_FStdBegin
//static FLIB_LOGLEVEL _console_level = FLIB_LOGLEVEL::FLIB_LOGLEVEL_DEBUG;

class FConsoleFinisher;
class FConsoleTraceFunction;
class F_DLL_API FConsole
{
public:
	FConsole(FLIB_LOGLEVEL level);
	FConsole(FLIB_LOGLEVEL level, const char* filename, int32 line = -1);
	virtual ~FConsole();
public:	

	template<typename T>
    FConsole& operator<<(T v); // will generate link error
	FConsole& operator<<(int8 v);
	FConsole& operator<<(int16 v);
	FConsole& operator<<(int32 v);
	FConsole& operator<<(int64 v);
	FConsole& operator<<(uint8 v);
	FConsole& operator<<(uint16 v);
	FConsole& operator<<(uint32 v);
	FConsole& operator<<(uint64 v);

#if FLIB_COMPILER_64BITS
	FConsole& operator<<(int v);
	FConsole& operator<<(uint v);
#else
	FConsole& operator<<(long v);
	FConsole& operator<<(ulong v);
#endif
	FConsole& operator<<(void* v);
	FConsole& operator<<(bool v);
	FConsole& operator<<(float v);
	FConsole& operator<<(double v);
	FConsole& operator<<(const char* v);
	FConsole& operator<<(char v[]);
	FConsole& operator<<(const std::string& v);
	FConsole& operator<< (FConsole& (*_f)(FConsole&));

	friend FConsole& operator<<(FConsole& str, const std::string& v);
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
	void operator=(FConsole &other);
};

class F_DLL_API FConsoleTraceFunction
{
public:
	FConsoleTraceFunction(FConsole& other, const char *func, const char *file, int32 line);
	~FConsoleTraceFunction();
	void operator=(FConsole &other);
private:
    FConsole& _log;
	const char *_func;
	const char *_file;
	int32 _line;
};

F_DLL_API void FSetLogConsoleLevel(FLIB_LOGLEVEL level);

_FStdEnd

/////////////////////////////////////////////////////////////////////////////////////////
////
#define F_CONSOLE(LEVEL) \
	FStd::FConsoleFinisher() = FStd::FConsole(FStd::FLIB_LOGLEVEL::FLIB_LOGLEVEL_##LEVEL)

#define F_CONSOLE_TRACE  \
	FStd::FConsole f_console_trace(FStd::FLIB_LOGLEVEL::FLIB_LOGLEVEL_TRACE, __FILE__, __LINE__);  \
	FStd::FConsoleTraceFunction f_consoleTraceFunction(f_console_trace, __FUNCTION__, __FILE__, __LINE__); \
	f_consoleTraceFunction = f_console_trace << __FUNCTION__ << "() enter \n";

#endif//__FLIB_CONSOLE_H_
