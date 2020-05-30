#ifndef __FLIB_LOGFILE_H__
#define __FLIB_LOGFILE_H__
#pragma once
#include "FLock.h"
#include "FIOStream.h"

_FStdBegin
class F_DLL_API FAutoFile
{
public:
	FAutoFile(FILE* fp);
	FAutoFile(const char* path);
	~FAutoFile();

    inline operator FILE*(){ return _file; }
    inline operator FLock*() { return &_lock; }
    inline operator bool() { return !!_file; }
private:
    FILE* _file;
    FLock _lock;
};

class FLogFileFinisher;
class FLogFileTraceFunction;
class F_DLL_API FLogFile : public FOStreamMaker
{
	virtual void output(const char* data);
public:
	FLogFile(FAutoFile& fp, FLIB_LOGLEVEL level);
	FLogFile(FAutoFile& fp, FLIB_LOGLEVEL level, const char* filename, int32 line = -1);
	virtual ~FLogFile();
protected:
	void _Logout(const char* str);
	void Finish();
	void _LogImpl();
protected:
    FAutoFile& m_flog;
    FLock      m_lock;
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
	void operator=(FOStream& other);
};

class F_DLL_API FLogFileTraceFunction
{
public:
	FLogFileTraceFunction(FOStream& other, const char* func, const char* file, int32 line);
	~FLogFileTraceFunction();
	void operator=(FOStream& other);
private:
	FOStream& _log;
	const char *_func;
	const char *_file;
	int32 _line;
	int64 _begin_time;
};

F_DLL_API void FSetLogFileLevel(FLIB_LOGLEVEL level);

_FStdEnd

//////////////////////////////////////////////////////////////////////
#define F_LOG_TO_FILE(LEVEL, file) \
	_FStd(FLogFileFinisher)() = _FStd(FLogFile)(file, LEVEL, __FILE__, __LINE__)

#define F_LOGFILE(LEVEL, file) F_LOG_TO_FILE(_FStd(FLIB_LOGLEVEL)::FLIB_LOGLEVEL_##LEVEL, file)

#define F_LOGFILE_TRACE(file)  \
    _FStd(FLogFile) f_logfile_trace(file, _FStd(FLIB_LOGLEVEL)::FLIB_LOGLEVEL_TRACE, __FILE__, __LINE__);  \
	_FStd(FLogFileTraceFunction) f_logfileTraceFunction(f_logfile_trace, __FUNCTION__, __FILE__, __LINE__); \
	f_logfileTraceFunction = f_logfile_trace << __FUNCTION__ << "() enter \n";

#endif//__FLIB_LOGFILE_H__