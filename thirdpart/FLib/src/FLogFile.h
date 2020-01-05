#ifndef __FLIB_LOGFILE_H__
#define __FLIB_LOGFILE_H__
#pragma once
#include "FLock.h"

_FStdBegin
class F_DLL_API FAutoFile
{
public:
	FAutoFile(FILE* fp);
	FAutoFile(const char* path);
	~FAutoFile();

    inline operator FILE*(){ return _file; }
    inline operator FLock*() { return &_lock; }
private:
    FILE* _file;
    FLock _lock;
};

class FLogFileFinisher;
class FLogFileTraceFunction;
class F_DLL_API FLogFile
{
public:
	FLogFile(FAutoFile* fp, FLIB_LOGLEVEL level);
	FLogFile(FAutoFile* fp, FLIB_LOGLEVEL level, const char* filename, int32 line = -1);
	virtual ~FLogFile();
public:
    FLogFile& operator<<(char v[]);
	FLogFile& operator<< (FLogFile& (*_f)(FLogFile&));
	friend FLogFile& endl(FLogFile& v);

	template<typename T>
    FLogFile& operator<< (T v); // will generate link error
#define TRMPLATE_DECLARE(T) \
    FLogFile& operator<< (T v);

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
protected:
	void _Logout(const char* str);
	void Finish();
	void _LogImpl();
protected:
    FAutoFile* m_flog;
    FLock     m_lock;
	std::string m_message;
    FLIB_LOGLEVEL m_level;
    friend class FLogFileFinisher;
    friend class FLogFileTraceFunction;
};

/////////////////////////////////////////////////////////////////////////////////////////
////
class F_DLL_API FLogFileFinisher
{
public:
	void operator=(FLogFile& other);
};

class F_DLL_API FLogFileTraceFunction
{
public:
	FLogFileTraceFunction(FLogFile& other, const char* func, const char* file, int32 line);
	~FLogFileTraceFunction();
	void operator=(FLogFile& other);
private:
    FLogFile& _log;
	const char *_func;
	const char *_file;
	int32 _line;
};

F_DLL_API void FSetLogFileLevel(FLIB_LOGLEVEL level);

_FStdEnd

//////////////////////////////////////////////////////////////////////
#define F_LOG_TO_FILE(LEVEL, file) \
	FStd::FLogFileFinisher() = FStd::FLogFile(file, LEVEL)

#define F_LOGFILE(LEVEL, file) F_LOG_TO_FILE(FStd::FLIB_LOGLEVEL::FLIB_LOGLEVEL_##LEVEL, file)

#define F_LOGFILE_TRACE(file)  \
	FStd::FLogFile f_logfile_trace(file, FStd::FLIB_LOGLEVEL::FLIB_LOGLEVEL_TRACE, __FILE__, __LINE__);  \
	FStd::FLogFileTraceFunction f_logfileTraceFunction(f_logfile_trace, __FUNCTION__, __FILE__, __LINE__); \
	f_logfileTraceFunction = f_logfile_trace << __FUNCTION__ << "() enter " << endl;

#endif//__FLIB_LOGFILE_H__