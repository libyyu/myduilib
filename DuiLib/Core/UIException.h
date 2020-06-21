#ifndef __UILIB_EXCEPTION_H__
#define __UILIB_EXCEPTION_H__

#ifdef _MSC_VER
#pragma once
#endif

namespace DuiLib
{
	class UILIB_API CException
	{
	public:
		CException();
		CException(const TCHAR* msg);
		CException(const TCHAR* msg, const TCHAR* filename, unsigned int lineNum);
		virtual ~CException();

		//Methods
	public:
		virtual const TCHAR*	getMessage() const;
		virtual void			setMessage(const TCHAR*msg);

		virtual const TCHAR*    getFilename() const;
		virtual unsigned int    getLineNum() const;

	protected:
		TCHAR			m_msg[4096];
		TCHAR			m_filename[512];
		unsigned int	m_lineNum;
	};

	inline void __DuiThrowException(const TCHAR* msg, const TCHAR* filename, unsigned int lineNum)
	{
		throw CException(msg, filename, lineNum);
	};
}

#define DuiException(formats, ...) DuiLib::__DuiThrowException(DuiLib::CDuiString::FormatString(formats, ##__VA_ARGS__), __FILET__, __LINE__)

#endif