#define FLIB_DLL
#include "FLogFile.h"
#include "FFunc.h"
#include <iostream>
#include <stdarg.h>
#include <sstream> 
#include <time.h>
#if FLIB_DEBUG && FLIB_COMPILER_WINDOWS
#include <windows.h>
#endif
_FStdBegin

FAutoFile::FAutoFile(FILE* fp) :_file(fp) {}
FAutoFile::FAutoFile(const char* path)
{
	_file = fopen(path, "w");
	assert(_file && "create log file failed!");
}
FAutoFile::~FAutoFile()
{
	if (_file) {
		fflush(_file);
		fclose(_file);
	}
	_file = nullptr;
}


static FLIB_LOGLEVEL _logfile_level = FLIB_LOGLEVEL::FLIB_LOGLEVEL_DEBUG;

void FSetLogFileLevel(FLIB_LOGLEVEL level)
{
	_logfile_level = level;
}

FLogFile::FLogFile(FAutoFile& fp, FLIB_LOGLEVEL level) : m_level(level), m_flog(fp)
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
FLogFile::FLogFile(FAutoFile& fp, FLIB_LOGLEVEL level, const char* filename, int32 line/* = -1*/)
	:m_level(level), m_flog(fp)
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
		<< (int32)(line)
		<< "] ";
}
FLogFile::~FLogFile() { m_message.clear(); }

void FLogFile::_Logout(const char* str) { m_message += str; }

void FLogFile::Finish()
{
	_LogImpl();
	m_message.clear();
}

void FLogFile::_LogImpl()
{
#if FLIB_DEBUG && FLIB_COMPILER_WINDOWS
	::OutputDebugStringA(m_message.c_str());
#endif

	assert(m_flog && "log file handle is null.");
	if (!m_flog) return;
	lock_wrapper lock(m_flog);
	FILE* fp = m_flog;
	if (!fp) return;
	fwrite(m_message.c_str(), m_message.size(), sizeof(char), fp);
	fflush(fp);
}

void FLogFile::output(const char* data)
{
	_Logout(data);
}

/////////////////////////////////////////////////////////////////////////////////////////
////
void FLogFileFinisher::operator=(FOStream& other)
{
	FLogFile* pFile = static_cast<FLogFile*>(&other);
	pFile->Finish();
}

FLogFileTraceFunction::FLogFileTraceFunction(FOStream& other, const char* func, const char* file, int32 line)
	: _log(other)
	, _func(func)
	, _file(file)
	, _line(line) 
{
	time_t tt;
	time(&tt);
	_begin_time = tt;
}
FLogFileTraceFunction::~FLogFileTraceFunction()
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
	FLogFile* pFile = static_cast<FLogFile*>(&_log);
	_log << "["
		<< FLIB_LogLevelName[pFile->m_level] << "|"
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
void FLogFileTraceFunction::operator=(FOStream& other)
{
	FLogFile* pFile = static_cast<FLogFile*>(&other);
	pFile->Finish();
}



_FStdEnd
