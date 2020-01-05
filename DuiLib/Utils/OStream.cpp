#include "stdafx.h"
#include "OStream.h"

namespace DuiLib
{
	IOStream& IOStreamMaker::operator <<(char c)
	{
		output(to_string(c).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(unsigned char c)
	{
		output(to_string(c).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(short v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(unsigned short v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(int v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(unsigned int v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(long v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(unsigned long v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(int64_t v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(uint64_t v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(float v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(double v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(bool v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(char v[])
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(const char* v)
	{
		output(to_string(v).c_str());
		return *this;
	}
	IOStream& IOStreamMaker::operator <<(void* v)
	{
		output(to_string(v).c_str());
		return *this;
	}

	std::string Convert::MBToUTF8(const char* pmb, int mLen)
	{
		std::string pOut;
		pOut.clear();
		if (mLen == 0) return pOut;
		// convert an MBCS string to widechar   
		int nLen = MultiByteToWideChar(CP_ACP, 0, pmb, mLen, NULL, 0);
		if (nLen <= 0) return pOut;
		wchar_t* lpszW = NULL;
		try
		{
			lpszW = new wchar_t[nLen];
			lpszW[nLen-1] = 0x0;
		}
		catch (bad_alloc)
		{
			return pOut;
		}

		int nRtn = MultiByteToWideChar(CP_ACP, 0, pmb, mLen, lpszW, nLen);

		if (nRtn != nLen)
		{
			delete[] lpszW;
			return pOut;
		}
		// convert an widechar string to utf8  
		int utf8Len = WideCharToMultiByte(CP_UTF8, 0, lpszW, nLen, NULL, 0, NULL, NULL);
		if (utf8Len <= 0)
		{
			delete[] lpszW;
			return pOut;
		}
		
		pOut = UnicodeToUTF8(lpszW, nLen);
		delete[] lpszW;
		return pOut;
	}
	std::string Convert::UTF8ToMB(const char* pu8, int utf8Len)
	{
		std::string pOut;
		pOut.clear();
		if (utf8Len == 0) return pOut;
		// convert an UTF8 string to widechar   
		int nLen = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, NULL, 0);
		if (nLen <= 0) return pOut;
		wchar_t* lpszW = NULL;
		try
		{
			lpszW = new wchar_t[nLen];
			lpszW[nLen-1] = 0x0;
		}
		catch (bad_alloc)
		{
			return pOut;
		}

		int nRtn = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, lpszW, nLen);

		if (nRtn != nLen)
		{
			delete[] lpszW;
			return pOut;
		}

		// convert an widechar string to Multibyte   
		int MBLen = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, NULL, 0, NULL, NULL);
		if (MBLen <= 0)
		{
			delete[] lpszW;
			return pOut;
		}
		pOut.resize(MBLen);
		nRtn = WideCharToMultiByte(CP_ACP, 0, lpszW, nLen, &*pOut.begin(), MBLen, NULL, NULL);
		delete[] lpszW;

		if (nRtn != MBLen)
		{
			pOut.clear();
			return pOut;
		}
		if (pOut.size() > 0 && pOut.back() == 0x0)
		{
			pOut.erase(pOut.size() - 1);
		}
		return pOut;
	}
	std::wstring Convert::MBToUnicode(const char* pmb, int mLen)
	{
		std::wstring pOut;
		pOut.clear();
		if (mLen == 0) return pOut;
		// convert an MBCS string to widechar   
		int uLen = MultiByteToWideChar(CP_ACP, 0, pmb, mLen, NULL, 0);

		if (uLen <= 0)
		{
			return pOut;
		}
		pOut.resize(uLen);

		int nRtn = MultiByteToWideChar(CP_ACP, 0, pmb, mLen, &*pOut.begin(), uLen);

		if (nRtn != uLen)
		{
			pOut.clear();
			return pOut;
		}
		if (pOut.size() > 0 && pOut.back() == 0x0)
		{
			pOut.erase(pOut.size() - 1);
		}
		return pOut;
	}
	std::string Convert::UnicodeToMB(const wchar_t* pun, int uLen)
	{
		std::string pOut;
		pOut.clear();
		if (uLen == 0) return pOut;
		// convert an widechar string to Multibyte   
		int MBLen = WideCharToMultiByte(CP_ACP, 0, pun, uLen, NULL, 0, NULL, NULL);
		if (MBLen <= 0)
		{
			return pOut;
		}
		pOut.resize(MBLen);
		int nRtn = WideCharToMultiByte(CP_ACP, 0, pun, uLen, &*pOut.begin(), MBLen, NULL, NULL);

		if (nRtn != MBLen)
		{
			pOut.clear();
			return pOut;
		}
		if (pOut.size() > 0 && pOut.back() == 0x0)
		{
			pOut.erase(pOut.size() - 1);
		}
		return pOut;
	}
	std::wstring Convert::UTF8ToUnicode(const char* pu8, int utf8Len)
	{
		std::wstring pOut;
		pOut.clear();
		if (utf8Len == 0) return pOut;
		// convert an UTF8 string to widechar   
		int nLen = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, NULL, 0);
		if (nLen <= 0)
		{
			return pOut;
		}
		pOut.resize(nLen);
		int nRtn = MultiByteToWideChar(CP_UTF8, 0, pu8, utf8Len, &*pOut.begin(), nLen);

		if (nRtn != nLen)
		{
			pOut.clear();
			return pOut;
		}
		if (pOut.size() > 0 && pOut.back() == 0x0)
		{
			pOut.erase(pOut.size() - 1);
		}
		return pOut;
	}
	std::string Convert::UnicodeToUTF8(const wchar_t* pun, int uLen)
	{
		std::string pOut;
		pOut.clear();
		if (uLen == 0) return pOut;
		// convert an widechar string to utf8  
		int utf8Len = WideCharToMultiByte(CP_UTF8, 0, pun, uLen, NULL, 0, NULL, NULL);
		if (utf8Len <= 0)
		{
			return pOut;
		}
		pOut.resize(utf8Len);
		int nRtn = WideCharToMultiByte(CP_UTF8, 0, pun, uLen, &*pOut.begin(), utf8Len, NULL, NULL);

		if (nRtn != utf8Len)
		{
			pOut.clear();
			return pOut;
		}
		if (pOut.size() > 0 && pOut.back() == 0x0)
		{
			pOut.erase(pOut.size()-1);
		}
		return pOut;
	}
	std::string Convert::ToUTF8(LPCTSTR pstr)
	{
#ifdef UNICODE
		return UnicodeToUTF8((const wchar_t*)pstr, -1);
#else
		return MBToUTF8((const char*)pstr, -1);
#endif
	}
}