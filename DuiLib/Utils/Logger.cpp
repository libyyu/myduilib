#include "StdAfx.h"
#include <time.h>
#include <direct.h>
#include <algorithm>
#include <locale>

namespace DuiLib
{
	const static CLogger::char_type* pLevel[] = { _T("Sys"), _T("Info"), _T("Warn"), _T("Error") };

	void (*LogWrapFunc)(int, const char*) = NULL;

	void CLogger::Init(const char_type* filename, Log_Level lev, Log_Type type, size_t maxSaveLogCnt)
	{
		std::locale::global(std::locale("chs"));
		assert(filename);
		string_type s(filename);
		std::replace(s.begin(), s.end(), _T('\\'), _T('/'));
		string_type::size_type index = s.find_last_of(_T('/'));
		string_type _dir, _name;
		if (string_type::npos != index)
		{
			_dir = s.substr(0, index + 1);
			_name = s.substr(index + 1);

			assert(!_dir.empty());
			assert(!_name.empty());

			if (!Path::IsDirectoryExist(_dir.c_str()))
			{
				bool ret = Path::CreateDirectory(_dir.c_str(), NULL);
				assert(ret && "create log directory failed!");
			}
		}
		_level = lev;
		_type = type;
		_maxSaveCnt = maxSaveLogCnt;

		memset(_filename, 0, sizeof(_filename));
		_tcscpy(_filename, filename);

		memset(_timename, 0, sizeof(_timename));

		//bool b = _Open();
		//assert(b && "open log file error!");
		
		time_t t = time(NULL);
		tm* aTm = localtime(&t);
		_NewLog(aTm);
	}

	CLogger::CLogger()
		: _file(NULL)
		, _plock(NULL)
	{
		memset(_filename, 0, sizeof(_filename));
		memset(_timename, 0, sizeof(_timename));
		_plock = new CDuiLock;
	}

	CLogger::~CLogger()
	{
		_Close();
		if (_plock)
			delete _plock;
		_plock = NULL;
	}

	void CLogger::_NewLog(tm* atm)
	{
		static  char_type sformat[7][24] = { _T("%Y"),_T("%Y%m"),_T("%Y%m%d"),_T("%Y%m%d_%H"),
			_T("%Y%m%d_%H_%M"),_T("%Y%m%d_%H_%M_%S") };

		char_type buffer[24] = { 0 };
		_tcsftime(buffer, 24, sformat[_type], atm);

		if (_tcscmp(buffer, _timename) != 0)
		{
			_tcscpy_s(_timename, 256, buffer);
			_Close();
			_Open();
		}
	}

	void CLogger::_OutTitle(Log_Level& lev, const char_type* srcfile, unsigned int line)
	{
		time_t t = time(NULL);
		tm* aTm = localtime(&t);
		_NewLog(aTm);
		
		if (_IsOpen())
		{
			char_type szTitle[2048] = { 0 };
			if (srcfile)
			{
				_stprintf(szTitle, _T("\r\n[%s][%-4d-%02d-%02d %02d:%02d:%02d][%lX][%s:%d]\r\n\t"),
					pLevel[lev],
					aTm->tm_year + 1900,
					aTm->tm_mon + 1,
					aTm->tm_mday,
					aTm->tm_hour,
					aTm->tm_min,
					aTm->tm_sec,
					_GetCurrentThreadID(),
					Path::GetFileName(srcfile).GetData(),
					line);
			}
			else
			{
				_stprintf(szTitle, _T("\r\n[%s][%-4d-%02d-%02d %02d:%02d:%02d][%lX]\r\n\t"),
					pLevel[lev],
					aTm->tm_year + 1900,
					aTm->tm_mon + 1,
					aTm->tm_mday,
					aTm->tm_hour,
					aTm->tm_min,
					aTm->tm_sec,
					_GetCurrentThreadID());
			}
			write(szTitle);
		}
	}

	bool CLogger::_Open()
	{
		if (_IsOpen())
			return true;

		this->_Clear();

		string_type stimename(_timename);
		string_type sfilename(_filename);	
		int nCursor = 1;
		string_type ext = Path::GetExtension(sfilename.c_str());
		string_type s(sfilename);
		std::replace(s.begin(), s.end(), _T('\\'), _T('/'));
		string_type::size_type index = s.find_last_of(_T('.'));
		string_type _name;
		if (string_type::npos != index)
		{
			_name = s.substr(0, index);
		}
		else
			_name = s;
		_name += "_" + stimename;

		sfilename = _name + "." + ext;

		while (true)
		{
			if (nCursor > 1)
			{
				sfilename = _name + string_type("_.") + TODUISTRING(nCursor);
				sfilename += ext;
			}

			if(!Path::IsFileExist(sfilename.c_str()))
				break;

			
			if (!CSystem::Instance()->IsFileUsed(sfilename.c_str()))
			{
				break;
			}

			nCursor++;
		}
		_file = _tfopen(sfilename.c_str(), "wb");
		if (!_file)
		{
			DuiException(_T("Failed to create log file %s, err = %d\r\n"), sfilename.c_str(), ::GetLastError());
		}
		else
		{
			time_t t = time(NULL);
			tm* aTm = localtime(&t);
			char_type szTime[2048] = { 0 };
			_stprintf(szTime, _T("\r\n--------------------------[%-4d-%02d-%02d %02d:%02d:%02d]--------------------------\r\n"),
				aTm->tm_year + 1900,
				aTm->tm_mon + 1,
				aTm->tm_mday,
				aTm->tm_hour,
				aTm->tm_min,
				aTm->tm_sec);
			write(szTime);
#ifdef _UNICODE
#ifdef _DEBUG
			write( _T("        Target = WIN32, CharSet = UNICODE, Mode = Debug\r\n"));
#else
			write( _T("        Target = WIN32, CharSet = UNICODE, Mode = Release\r\n"));
#endif
#else
#ifdef _DEBUG
			write( _T("        Target = WIN32, CharSet = MutiChar, Mode = Debug\r\n"));
#else
			write( _T("        Target = WIN32, CharSet = MutiChar, Mode = Release\r\n"));
#endif
#endif
			write(_T("---------------------------------LogBegin--------------------------------\r\n"));
		}

		return _IsOpen();
	}

	void CLogger::_Close()
	{
		if (_IsOpen())
		{
			CDuiAutoLocker lock(_plock);
			{
				this->write(_T("\r\n\n-----------------------------------LogEnd--------------------------------\r\n"));
				fclose(_file);
				_file = NULL;
			}
		}
	}
	void CLogger::_Clear()
	{

	}
	bool CLogger::_IsOpen() const
	{
		if (!_file)
			return false;
		return _file != NULL;
	}
	
	unsigned long CLogger::_GetCurrentThreadID() const
	{
#ifdef _WIN32
		return ::GetCurrentThreadId();
#else
		return (unsigned long)syscall(SYS_thread_selfid);
#endif
	}

	template<>
	void CLogger::_Vfprintf(const char* fmt, va_list& va, Log_Level lev)
	{
		int len = ::_vscprintf(fmt, va); // _vscprintf doesn't count, terminating '\0'
		
		char* szBuffer = new char[len+1];
		int iRet = ::vsprintf(szBuffer, fmt, va);
		szBuffer[len] = '\0';
		if (LogWrapFunc != NULL)
		{
			LogWrapFunc(lev, szBuffer);
		}
		else
		{
			write(szBuffer, lev);
		}
		delete[] szBuffer;
	}

	template<>
	void CLogger::_Vfprintf(const wchar_t* fmt, va_list& va, Log_Level lev)
	{
		int len = ::_vscwprintf(fmt, va);
		wchar_t* szBuffer = new wchar_t[len+1];
		int iRet = ::vswprintf(szBuffer, fmt, va);
		szBuffer[len] = L'\0';
		if (LogWrapFunc != NULL)
		{
			std::string s = to_string(szBuffer);
			LogWrapFunc(lev, s.c_str());
		}
		else
		{
			write(szBuffer, lev);
		}

		delete[] szBuffer;
	}

	void CLogger::flush()
	{
		assert(_IsOpen());
		fflush(_file);
	}

	void CLogger::write(const char* lpstrLog, Log_Level lev)
	{
		assert(_IsOpen());
		unsigned int nWrite = 0;
		unsigned int nLen = strlen(lpstrLog);
		nWrite = fwrite(lpstrLog, 1, nLen * sizeof(char), _file);
#ifdef UNICODE	
		CDuiString s = lpstrLog;
#else
		CDuiString s = lpstrLog;
#endif
		switch (lev)
		{
		case Log_Level::ErrorLog:
			Console::Error(s);
			break;
		case Log_Level::WarningLog:
			Console::Warning(s);
			break;
		default:
			Console::Write(s);
			break;
		}
	}
	void CLogger::write(const wchar_t* lpstrLog, Log_Level lev)
	{
		std::string s = to_string(lpstrLog);
		write(s.c_str(), lev);
	}

	void CLogger::logMessage(const char* lpstrLog)
	{
		this->write(lpstrLog);
	}

	void CLogger::logMessage(const wchar_t* lpstrLog)
	{
		this->write(lpstrLog);
	}

	void CLogger::formatLog(const char_type* fmt, ...)
	{
		assert(fmt);
		if (!fmt)
			return;

		CDuiAutoLocker lock(_plock);
		{
			_OutTitle(_level, NULL, 0);
			if (_IsOpen())
			{
				va_list va;
				va_start(va, fmt);
				this->_Vfprintf<char_type>(fmt, va);
				va_end(va);
			}
			this->flush();
		}
	}

	void CLogger::formatLog(const char_type* srcfile, unsigned int line, Log_Level lev, const char_type* fmt, ...)
	{
		assert(fmt);
		if (!fmt)
			return;

		if (lev < _level && SysLog != lev)
			return;

		CDuiAutoLocker lock(_plock);
		{
			_OutTitle(lev, srcfile, line);

			if (!_IsOpen()) return;

			if (this->_IsOpen())
			{
				va_list va;
				va_start(va, fmt);
				this->_Vfprintf<char_type>(fmt, va, lev);
				va_end(va);
			}
			this->flush();
		}
	}

	CLogger* duiLogger = NULL;
	void CreateLogger(const TCHAR* filename)
	{
		if(duiLogger)
			delete duiLogger;
		duiLogger = new CLogger();
		if(!filename)
		{
			CDuiString sLogName = Path::GetCurAppName(TRUE);
			sLogName.Append(_T(".log"));
			duiLogger->Init(sLogName);
		}
		else
		{
			duiLogger->Init(filename);
		}
	}

	void SetLogCallback(void (*LogFunc)(int, const char*))
	{
		LogWrapFunc = LogFunc;
	}
}