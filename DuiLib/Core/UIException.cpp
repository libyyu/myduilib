#include "StdAfx.h"

namespace DuiLib
{
	CException::CException()
		: m_lineNum(0)
	{
		m_msg[0] = 0x0;
		m_filename[0] = 0x0;
	}

	CException::CException(const TCHAR* msg)
		:m_lineNum(0)
	{
		m_msg[0] = 0x0;
		m_filename[0] = 0x0;
		lstrcpy(m_msg, msg);
	}

	CException::CException(const TCHAR* msg, const TCHAR* filename, unsigned int lineNum)
		: m_lineNum(0)
	{
		m_msg[0] = 0x0;
		m_filename[0] = 0x0;
		lstrcpy(m_msg, msg);
		lstrcpy(m_filename, filename);
	}

	CException::~CException()
	{
	}

	const TCHAR* CException::getMessage() const
	{
		return m_msg;
	}

	void CException::setMessage(const TCHAR* msg)
	{
		m_msg[0] = 0x0;
		lstrcpy(m_msg, msg);
	}

	const TCHAR* CException::getFilename() const
	{
		return m_filename;
	}

	unsigned int CException::getLineNum() const
	{
		return m_lineNum;
	}
}