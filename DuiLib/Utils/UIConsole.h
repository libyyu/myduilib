#ifndef _UICONSOLE_H_
#define _UICONSOLE_H_
#pragma once

namespace DuiLib
{
	class DUILIB_API Console : public Singleton<Console>
	{
		DECLARE_SINGLETON_CLASS(Console)
		enum e_Flag
		{
			e_Error,
			e_Warn,
			e_Info
		};
	protected:
		Console();
		virtual ~Console();
	private:
		bool        m_bConsoleOpen;
		bool		m_bConsoleInit;
		HANDLE 	 	m_stdOutputHandle;
		HANDLE 	 	m_stdErrHandle;
		CDuiLock*  	m_pLock;
	protected:
		void _RedirectIOToConsole();
		void _Print(const TCHAR* szBuffer, e_Flag flag);
	public:
		static void Write(const TCHAR* format, ...);
		static void WriteLine(const TCHAR* format, ...);
		static void Warning(const TCHAR* format, ...);
		static void Error(const TCHAR* format, ...);
	};
}

#endif//_UICONSOLE_H_