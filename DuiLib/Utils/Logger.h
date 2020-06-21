#ifndef _LOGGER_H_
#define _LOGGER_H_
#pragma once

namespace DuiLib
{
	class UILIB_API ILogger
	{
	public:
		virtual void flush() = 0;

		virtual void logMessage(const char* lpstrLog) = 0;

		class Stream : public IOStreamMaker
		{
			ILogger* m_target;
		public:
			Stream(ILogger* target) : m_target(target) {}

			void output(const char* lpstrLog) 
			{
				m_target->logMessage(lpstrLog);
			}
		};
		Stream stream() { return Stream(this); }
	};

	class UILIB_API CLogger : public ILogger
	{
	public:
		typedef CLogger             self_type;
#ifdef _UNICODE
		typedef wchar_t				char_type;
		typedef std::wstring        string_type;
#else
		typedef char				char_type;
		typedef std::string         string_type;
#endif
		enum Log_Level
		{
			SysLog = 0,
			InfoLog = 1,
			WarningLog = 2,
			ErrorLog = 3,
		};
		enum Log_Type
		{
			YEAR = 0,
			MONTH = 1,
			DAY = 2,
			HOUR = 3,
			MIN = 4,
			SECOND = 5,
		};

	public:
		virtual void flush();
		virtual void logMessage(const char* lpstrLog);
		virtual void logMessage(const wchar_t* lpstrLog);
		virtual void formatLog(const char_type* fmt, ...);
		virtual void formatLog(const char_type* srcfile, unsigned int line, Log_Level lvl, const char_type* lpstrLog, ...);
	public:
		void Init(const char_type* filename = _T("duilib.log"), Log_Level lev = Log_Level::InfoLog, Log_Type type = Log_Type::DAY, size_t maxSaveLogCnt = 3);
		CLogger();
		virtual ~CLogger();
		bool isValid()const { return _IsOpen(); }
	protected:
		CLogger(const self_type&) {}
		self_type& operator = (const self_type&) { return *this; }

		template<typename charT>
		void _Vfprintf(const charT* fmt, va_list& va, Log_Level lev = Log_Level::InfoLog);
	private:
		void write(const wchar_t* lpstrLog, Log_Level lev = Log_Level::InfoLog);
		void write(const char* lpstrLog, Log_Level lev = Log_Level::InfoLog);
	private:
		void					 _NewLog(tm* atm);
		void                     _OutTitle(Log_Level& lev, const char_type* srcfile = NULL, unsigned int line = 0);
		bool                     _Open();
		void                     _Close();
		bool                     _IsOpen() const;
		unsigned long            _GetCurrentThreadID() const;
		//void                     _ScanAllFiles(const string_type& rootPath, std::vector<string_type>& container, bool isRecursive = true);
		void                     _Clear();
	private:
		Log_Level                _level;
		Log_Type                 _type;
		size_t                   _maxSaveCnt;
		char_type				 _filename[512];
		char_type				 _timename[256];
		FILE*                    _file;
		CDuiLock*                _plock;
	};

	UILIB_API extern CLogger* duiLogger; 

	UILIB_API void CreateLogger(const TCHAR* filename = NULL);
}
#define  DuiLogInfo(fmt,...)  { if(DuiLib::duiLogger){DuiLib::duiLogger->formatLog(__FILET__,__LINE__,DuiLib::CLogger::Log_Level::InfoLog,fmt,##__VA_ARGS__);} else {Console::WriteLine(fmt,##__VA_ARGS__);} }
#define  DuiLogWarning(fmt,...) { if(DuiLib::duiLogger){DuiLib::duiLogger->formatLog(__FILET__,__LINE__,DuiLib::CLogger::Log_Level::WarningLog,fmt,##__VA_ARGS__);} else {Console::WarningLine(fmt,##__VA_ARGS__);} }
#define  DuiLogError(fmt,...) { if(DuiLib::duiLogger){DuiLib::duiLogger->formatLog(__FILET__,__LINE__,DuiLib::CLogger::Log_Level::ErrorLog,fmt,##__VA_ARGS__);} else {Console::ErrorLine(fmt,##__VA_ARGS__);} }

#define GLog(a, lev, fmt, ...) { a.formatLog(__FILET__,__LINE__,lev,fmt,##__VA_ARGS__); }
#define GInfo(a, fmt, ...) GLog(a, DuiLib::CLogger::Log_Level::InfoLog, fmt, ##__VA_ARGS__)
#define GWarn(a, fmt, ...) GLog(a, DuiLib::CLogger::Log_Level::WarningLog, fmt, ##__VA_ARGS__)
#define GError(a, fmt, ...) GLog(a, DuiLib::CLogger::Log_Level::ErrorLog, fmt, ##__VA_ARGS__)

#endif// _LOGGER_H_