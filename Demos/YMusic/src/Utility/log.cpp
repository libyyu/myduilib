#include "stdafx.h"
#include "log.h"
#include <algorithm>
#include <cassert>
#include <locale>
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <sys/syscall.h>
#endif
#include "Convert.h"

CLog::CLog(const char_type* filename,Log_Level lev /* = InfoLog */,Log_Type type /* = DAY */,size_t maxSaveLogCnt /* = 3 */)
	:_pfile(NULL),_level(lev),_type(type),_maxSaveCnt(maxSaveLogCnt)
{
	assert(filename);
	string_type s(filename);
	std::replace(s.begin(),s.end(), _T('\\'), _T('/'));
	string_type::size_type index = s.find_last_of(_T('/'));
	if(std::string::npos != index)
	{
		_dir  = s.substr(0,index+1);
		_name = s.substr(index+1);

		assert(!_dir.empty());
		assert(!_name.empty());
	}

	
	if(!em_utility::mci::is_dir_exist(_dir.c_str()))
	{
		if(!em_utility::mci::confirm_dir(_dir.c_str(), _T('/')))
		{
//			assert(false);
		}
	}


	memset(_timename,0,sizeof(_timename));

	std::locale::global( std::locale("chs") );

	(*this)(SysLog, _T("LogBegin"));
}
CLog::CLog(const CLog::self_type& ths)
	:_pfile(ths._pfile), _level(ths._level), _type(ths._type), _maxSaveCnt(ths._maxSaveCnt)
{	
	memset(_timename, 0, sizeof(_timename));

	std::locale::global(std::locale("chs"));

	(*this)(SysLog, _T("LogBegin"));
}

CLog::~CLog()
{
	(*this)(SysLog, _T("LogEnd"));
	_Close();
}

bool CLog::_Open()
{
	if(_IsOpen())
		return true;

	this->_Clear();

	string_type filename(_dir);
	filename += _timename;
	filename += _name;

	_pfile = _tfopen(filename.c_str(), _T("a"));
	return _IsOpen();
}

void CLog::_Close()
{
	if(_IsOpen())
	{	
		fclose(_pfile);		
		_pfile = NULL;
	}	
}

void CLog::_NewLog(tm* atm)
{
	const static  char_type sformat[7][24] = 
	{ _T("%Y"),_T("%Y%m"),_T("%Y%m%d"),_T("%Y%m%d_%H"),_T("%Y%m%d_%H_%M"),_T("%Y%m%d_%H_%M_%S") };

	char_type buffer[24] = {0};	
	strftime(buffer,24,sformat[_type],atm);

	if(_tcscmp(buffer,_timename) != 0)
	{
		strcpy(_timename,buffer);
		_Close();
		_Open();
	}
}

 unsigned long CLog::_GetCurrentThreadID() const
{
#ifdef _WIN32
	return ::GetCurrentThreadId();
#else
	return (unsigned long)syscall(SYS_thread_selfid);
#endif
}

 void CLog::_ScanAllFiles(const string_type& rootPath,std::vector<string_type>& container,bool isRecursive /* = true */)
{

}
 void CLog::_Clear()
{

}
 void CLog::_OutTitle(Log_Level &lev)
{
	const static char_type* pLevel[] ={ _T("Sys"),_T("Info"),_T("Warn"),_T("Error") };

	time_t t = time(NULL);
	tm* aTm = localtime(&t);

	_NewLog(aTm);  

	//       YYYY   year
	//       MM     month (2 digits 01-12)
	//       DD     day (2 digits 01-31)
	//       HH     hour (2 digits 00-23)
	//       MM     minutes (2 digits 00-59)
	//       SS     seconds (2 digits 00-59)
	if(!_IsOpen()) return;

	_ftprintf(_pfile, _T("\n[%-4d-%02d-%02d %02d:%02d:%02d][%s][%lX]| "),
		aTm->tm_year+1900,
		aTm->tm_mon+1,
		aTm->tm_mday,
		aTm->tm_hour,
		aTm->tm_min,
		aTm->tm_sec,
		pLevel[lev],
		_GetCurrentThreadID());
#ifdef _CONSOLE
	_ftprintf(stderr, _T("\n[%-4d-%02d-%02d %02d:%02d:%02d][%s][%lX]| "),
		aTm->tm_year+1900,
		aTm->tm_mon+1,
		aTm->tm_mday,
		aTm->tm_hour,
		aTm->tm_min,
		aTm->tm_sec,
		pLevel[lev],
		_GetCurrentThreadID());
#endif

#ifdef _DEBUG
	char_type buff[2049] = {0};
	_stprintf(buff, _T("\n[%-4d-%02d-%02d %02d:%02d:%02d][%s][%lX]| "),
		aTm->tm_year + 1900,
		aTm->tm_mon + 1,
		aTm->tm_mday,
		aTm->tm_hour,
		aTm->tm_min,
		aTm->tm_sec,
		pLevel[lev],
		_GetCurrentThreadID());
	::OutputDebugString(buff);
#endif
}


 template<typename charT>
void CLog::_Vfprintf(const charT* fmt,va_list& va)
{
#ifdef _CONSOLE
#ifdef _WIN32
	_vftprintf(stderr,fmt,va); //only win
#else
	va_list newva;
	va_copy(newva,va);
	_vftprintf(stderr,fmt,newva);
	va_end(newva);
#endif
#endif

	_vftprintf(_pfile,fmt,va);
	fflush(_pfile);

#ifdef _DEBUG
	char_type buff[2049] = { 0 };
	_vstprintf(buff, fmt, va);
	::OutputDebugString(buff);
#endif
}


template<typename charT>
CLog& CLog::operator()(const charT* fmt,...)
{
	va_list va;
	va_start(va,fmt);
	(*this)(InfoLog,fmt,va);
	va_end(va);

	return *this;
}

template<typename charT>
CLog& CLog::operator()(Log_Level lev,const charT* fmt,...)
{
	va_list va;
	va_start(va,fmt);
	(*this)(lev,fmt,va);
	va_end(va);

	return *this;
}

template<typename charT>
CLog& CLog::operator()(Log_Level lev,const charT* fmt,va_list& va)
{
	assert(fmt);
	if(!fmt)
		return *this;

	if(lev < _level && SysLog != lev)
		return *this;

	DuiLib::CDuiAutoLocker gd(&_lock);
	{
		_OutTitle(lev);
		if(_IsOpen())
			this->_Vfprintf<charT>(fmt,va);
	}

	return *this;
}
