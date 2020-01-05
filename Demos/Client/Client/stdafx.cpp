// stdafx.cpp : 只包括标准包含文件的源文件
// Client.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中
// 引用任何所需的附加头文件，而不是在此文件中引用

namespace DuiLib
{
	CDuiString toDuiString(const char* arg)
	{
		CDuiString s_out;
#ifdef _UNICODE
		std::wstring puni = Convert::UTF8ToUnicode(arg, strlen(arg));
		s_out.Assign((LPCTSTR)puni.c_str(), puni.size());
#else
		std::string pmb = Convert::UTF8ToMB(arg, strlen(arg));
		s_out.Assign((LPCTSTR)pmb.c_str(), pmb.size());
#endif
		return s_out;
	}

	std::string toStdString(const LPCTSTR str)
	{
		std::string s_out;
#ifdef _UNICODE
		s_out = DuiLib::Convert::UnicodeToUTF8(str);
#else
		s_out = str;
#endif
		return s_out;
	}
}
