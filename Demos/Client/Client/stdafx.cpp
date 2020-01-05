// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// Client.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO: �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������

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
