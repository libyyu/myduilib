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
	FConsole& operator<<(char v[]);
	FConsole& operator<< (FConsole& (*_f)(FConsole&));
	friend FConsole& endl(FConsole& v);
	
	template<typename T>
    inline FConsole& operator<< (T v); // will generate link error
#define TRMPLATE_DECLARE(T) \
    inline FConsole& operator<< (T v);

    TRMPLATE_DECLARE(int8)
    TRMPLATE_DECLARE(int16)
    TRMPLATE_DECLARE(int32)
    TRMPLATE_DECLARE(int64)
    TRMPLATE_DECLARE(uint8)
    TRMPLATE_DECLARE(uint16)
    TRMPLATE_DECLARE(uint32)
    TRMPLATE_DECLARE(uint64)
#if FLIB_COMPILER_64BITS
    TRMPLATE_DECLARE(int)
    TRMPLATE_DECLARE(uint)
#else
	TRMPLATE_DECLARE(long)
	TRMPLATE_DECLARE(ulong)
#endif
    TRMPLATE_DECLARE(bool)
    TRMPLATE_DECLARE(float)
    TRMPLATE_DECLARE(double)
    TRMPLATE_DECLARE(const char *)
    TRMPLATE_DECLARE(void *)
    TRMPLATE_DECLARE(const std::string&)
#undef TRMPLATE_DECLARE
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
	f_consoleTraceFunction = f_console_trace << __FUNCTION__ << "() enter " << endl;

#endif//__FLIB_CONSOLE_H_
