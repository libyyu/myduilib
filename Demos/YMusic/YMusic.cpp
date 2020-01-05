// YMusic.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "YMusic.h"

DuiLib::CDuiString TransToDuiString(const char* u8)
{
	assert(u8);
	DuiLib::CDuiString s_out;
#ifdef _UNICODE
	std::wstring puni = DuiLib::Convert::UTF8ToUnicode(u8, strlen(u8));
	s_out.Assign((LPCTSTR)puni.c_str(), puni.size());
#else
	s_out.Assign((LPCTSTR)u8, strlen(u8));
#endif
	return s_out;
}

std::string TransToStdString(const DuiLib::CDuiString& ds)
{
	std::string s_out;// = DuiLib::Convert::ToUTF8(ds.GetData());
#ifdef _UNICODE
	s_out = DuiLib::Convert::UnicodeToUTF8(ds.GetData());
#else
	s_out = ds.GetData();
#endif
	return s_out;
}
