#define FLIB_DLL
#include "FLogFile.h"
#include "FFunc.h"
#include <iostream>
#include <stdarg.h>
#include <sstream> 

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
		<< "]";
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
		<< "]";
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
	assert(m_flog && "log file handle is null.");
	if (!m_flog) return;
	lock_wrapper lock(m_flog);
	FILE* fp = m_flog;
	if (!fp) return;
	fwrite(m_message.c_str(), m_message.size(), sizeof(char), fp);
}

FLogFile& FLogFile::operator<<(const char* str)
{
	_Logout(str);
	return *this;
}
FLogFile& FLogFile::operator<<(const std::string& str)
{
	_Logout(str.c_str());
	return *this;
}
FLogFile& FLogFile::operator<<(char v[])
{
    _Logout((char*)v);
	return *this;
}
FLogFile& FLogFile::operator<<(void *p)
{
	_Logout(FFormat("%p", p).c_str());
	return *this;
}
FLogFile& FLogFile::operator<< (FLogFile& (*_f)(FLogFile&))
{
	return _f(*this);
}

#define TRMPLATE_METHOD(T) \
	FLogFile& FLogFile::operator<<(T v) \
	{ \
		std::stringstream str;	\
		str << v; \
        _Logout(str.str().c_str()); \
		return *this; \
	}
TRMPLATE_METHOD(int8)
TRMPLATE_METHOD(int16)
TRMPLATE_METHOD(int32)
TRMPLATE_METHOD(int64)
TRMPLATE_METHOD(uint8)
TRMPLATE_METHOD(uint16)
TRMPLATE_METHOD(uint32)
TRMPLATE_METHOD(uint64)
#if FLIB_COMPILER_64BITS
TRMPLATE_METHOD(int)
TRMPLATE_METHOD(uint)
#else
TRMPLATE_METHOD(long)
TRMPLATE_METHOD(ulong)
#endif
TRMPLATE_METHOD(float)
TRMPLATE_METHOD(double)
TRMPLATE_METHOD(bool)
#undef TRMPLATE_METHOD
/////////////////////////////////////////////////////////////////////////////////////////
////
void FLogFileFinisher::operator=(FLogFile& other)
{
	other.Finish();
}

FLogFileTraceFunction::FLogFileTraceFunction(FLogFile& other, const char* func, const char* file, int32 line)
	: _log(other)
	, _func(func)
	, _file(file)
	, _line(line) 
{}
FLogFileTraceFunction::~FLogFileTraceFunction()
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

	_log << "["
		<< FLIB_LogLevelName[_log.m_level] << "|"
		<< FGetCurrentThreadId() << "|"
		<< buff << "|"
		<< (_file ? FGetFilename(_file).c_str() : "<unknow source>") << ":"
		<< _line
		<< "]"
		<< _func << "() leave "
		<< "\n";

	*this = _log;
}
void FLogFileTraceFunction::operator=(FLogFile& other)
{
	other.Finish();
}



_FStdEnd
