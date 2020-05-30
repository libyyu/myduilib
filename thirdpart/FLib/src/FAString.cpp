#define FLIB_DLL
#include "FAString.h"
#include <locale.h>
#include <assert.h>
#include <iostream>
#include <sstream> 
#include <stdarg.h>
#include <string.h>

_FStdBegin

FAString::FAString() : m_pstr(m_szBuffer)
{
	m_szBuffer[0] = '\0';
}
FAString::FAString(const char ch) : m_pstr(m_szBuffer)
{
	m_szBuffer[0] = ch;
	m_szBuffer[1] = '\0';
}
FAString::FAString(const char* lpsz, int nLen/* = -1*/) : m_pstr(m_szBuffer)
{
	assert(lpsz);
	m_szBuffer[0] = '\0';
	Assign(lpsz, nLen);
}
FAString::FAString(const FAString& src) : m_pstr(m_szBuffer)
{
	m_szBuffer[0] = '\0';
	Assign(src.m_pstr);
}
FAString::~FAString()
{
	if (m_pstr != m_szBuffer && m_pstr != NULL) free(m_pstr);
}

void FAString::Empty()
{
	if (m_pstr != m_szBuffer) free(m_pstr);
	m_pstr = m_szBuffer;
	m_szBuffer[0] = '\0';
}
size_t FAString::GetLength() const
{
	return strlen(m_pstr);
}
bool FAString::IsEmpty() const
{
	return m_pstr[0] == '\0';
}
char FAString::GetAt(int nIndex) const
{
	return m_pstr[nIndex];
}
char FAString::operator[] (int nIndex) const
{
	return m_pstr[nIndex];
}

void FAString::SetAt(int nIndex, char ch)
{
	assert(nIndex >= 0 && nIndex < (int)GetLength());
	m_pstr[nIndex] = ch;
}

void FAString::Append(const char* pstr)
{
	assert(pstr);
	size_t nNewLength = GetLength() + strlen(pstr);
	if (nNewLength >= MAX_LOCAL_STRING_LEN) {
		if (m_pstr == m_szBuffer) {
			m_pstr = static_cast<char*>(malloc((nNewLength + 1) * sizeof(char)));
			strcpy(m_pstr, m_szBuffer);
			strcat(m_pstr, pstr);
		}
		else {
			m_pstr = static_cast<char*>(realloc(m_pstr, (nNewLength + 1) * sizeof(char)));
			strcat(m_pstr, pstr);
		}
	}
	else {
		if (m_pstr != m_szBuffer) {
			free(m_pstr);
			m_pstr = m_szBuffer;
		}
		strcat(m_szBuffer, pstr);
	}
}

void FAString::Assign(const char* pstr, int cchMax/* = -1*/)
{
	if (pstr == NULL) pstr = "";
	cchMax = (cchMax < 0 ? (int)strlen(pstr) : cchMax);
	if (cchMax < MAX_LOCAL_STRING_LEN) {
		if (m_pstr != m_szBuffer) {
			free(m_pstr);
			m_pstr = m_szBuffer;
		}
	}
	else if (cchMax > (int)GetLength() || m_pstr == m_szBuffer) {
		if (m_pstr == m_szBuffer) m_pstr = NULL;
		m_pstr = static_cast<char*>(realloc(m_pstr, (cchMax + 1) * sizeof(char)));
	}
	strncpy(m_pstr, pstr, cchMax);
	m_pstr[cchMax] = '\0';
}
int FAString::Compare(const char* lpsz) const
{
	return strcmp(m_pstr, lpsz);
}

int FAString::CompareNoCase(const char* lpsz) const
{
#if FLIB_COMPILER_WINDOWS
	return _strnicmp(m_pstr, lpsz, strlen(lpsz));
#else
	return strncasecmp(m_pstr, lpsz, strlen(lpsz));
#endif
}

FAString FAString::Left(int iLength) const
{
	if (iLength < 0) iLength = 0;
	if (iLength > (int)GetLength()) iLength = (int)GetLength();
	return FAString(m_pstr, iLength);
}

FAString FAString::Mid(int iPos, int iLength/* = -1*/) const
{
	if (iLength < 0) iLength = (int)GetLength() - iPos;
	if (iPos + iLength > (int)GetLength()) iLength = (int)GetLength() - iPos;
	if (iLength <= 0) return FAString();
	return FAString(m_pstr + iPos, iLength);
}

FAString FAString::Right(int iLength) const
{
	int iPos = (int)GetLength() - iLength;
	if (iPos < 0) {
		iPos = 0;
		iLength = (int)GetLength();
	}
	return FAString(m_pstr + iPos, iLength);
}

int FAString::Find(char ch, int iPos/* = 0*/) const
{
	assert(iPos >= 0 && iPos <= (int)GetLength());
	if (iPos != 0 && (iPos < 0 || iPos >= (int)GetLength())) return -1;
	const char* p = strrchr(m_pstr + iPos, ch);
	if (p == NULL) return -1;
	return (int)(p - m_pstr);
}

int FAString::Find(const char* pstrSub, int iPos/* = 0*/) const
{
	assert(iPos >= 0 && iPos <= (int)GetLength());
	if (iPos != 0 && (iPos < 0 || iPos >(int)GetLength())) return -1;
	const char* p = strstr(m_pstr + iPos, pstrSub);
	if (p == NULL) return -1;
	return (int)(p - m_pstr);
}

int FAString::ReverseFind(char ch) const
{
	const char* p = strrchr(m_pstr, ch);
	if (p == NULL) return -1;
	return (int)(p - m_pstr);
}

int FAString::Replace(const char* pstrFrom, const char* pstrTo)
{
	FAString sTemp;
	int nCount = 0;
	int iPos = Find(pstrFrom);
	if (iPos < 0) return 0;
	int cchFrom = (int)strlen(pstrFrom);
	int cchTo = (int)strlen(pstrTo);
	while (iPos >= 0) {
		sTemp = Left(iPos);
		sTemp += pstrTo;
		sTemp += Mid(iPos + cchFrom);
		Assign(sTemp);
		iPos = Find(pstrFrom, iPos + cchTo);
		nCount++;
	}
	return nCount;
}

int FAString::Format(const char* pstrFormat, ...)
{
	// Do ordinary printf replacements
	// NOTE: Documented max-length of wvsprintf() is 2048
	char szBuffer[20480] = { 0 };
	va_list argList;
	va_start(argList, pstrFormat);

	int iRet = vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]) - 2, pstrFormat, argList);

	va_end(argList);
	Assign(szBuffer);
	return iRet;
}

int FAString::SmallFormat(const char* pstrFormat, ...)
{
	// Do ordinary printf replacements
	// NOTE: Documented max-length of wvsprintf() is 2048
	char szBuffer[256] = { 0 };
	va_list argList;
	va_start(argList, pstrFormat);

	int iRet = vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]) - 2, pstrFormat, argList);

	va_end(argList);
	Assign(szBuffer);
	return iRet;
}

FAString FAString::Printf(const char* pstrFormat, ...)
{
	FAString strOut;
	char szBuffer[2048] = { 0 };
	va_list argList;
	va_start(argList, pstrFormat);

	vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]) - 2, pstrFormat, argList);

	va_end(argList);
	strOut.Assign(szBuffer);
	return strOut;
}

FAString FAString::SmallPrintf(const char* pstrFormat, ...)
{
	FAString strOut;
	char szBuffer[256] = { 0 };
	va_list argList;
	va_start(argList, pstrFormat);

	vsnprintf(szBuffer, sizeof(szBuffer) / sizeof(szBuffer[0]) - 2, pstrFormat, argList);

	va_end(argList);
	strOut.Assign(szBuffer);
	return strOut;
}

void FAString::SplitToArray(std::vector<FAString>& OutArr, const char* pattern)
{
	if (IsEmpty() || !pattern || pattern[0] == 0x0)
		return;

	FAString temp;
	int pos = 0, offset = 0;

	// 分割第1~n-1个
	while ((pos = Find(pattern, offset)) != -1)
	{
		temp = Mid(offset, pos - offset);
		if (temp.GetLength() > 0)
		{
			OutArr.push_back(temp);
		}
		offset = pos + 1;
	}

	// 分割第n个
	temp = Mid(offset, (int)GetLength() - offset);
	if (temp.GetLength() > 0)
	{
		OutArr.push_back(temp);
	}
}

void FAString::TrimLeft()
{
	const char* p = m_pstr;
	while (*p != '\0')
	{
		if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
		{
			++p;
		}
		else
		{
			break;
		}
	}
	Assign(p);
}

void FAString::TrimRight()
{
	size_t len = GetLength();
	char* p = m_pstr + len - 1;
	while (p >= m_pstr)
	{
		if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n')
		{
			*p = '\0';
			--p;
		}
		else
		{
			break;
		}
	}
}

void FAString::Trim()
{
	TrimLeft();
	TrimRight();
}

FAString::operator char* () const
{
	return m_pstr;
}

const FAString& FAString::operator=(const char ch)
{
	Empty();
	m_szBuffer[0] = ch;
	m_szBuffer[1] = '\0';
	return *this;
}

const FAString& FAString::operator=(const FAString& src)
{
	Assign(src);
	return *this;
}
const FAString& FAString::operator=(const char* lpStr)
{
	if (lpStr)
	{
		Assign(lpStr);
	}
	else
	{
		Empty();
	}
	return *this;
}

FAString FAString::operator+(const FAString& src) const
{
	FAString sTemp = *this;
	sTemp.Append(src);
	return sTemp;
}
FAString FAString::operator+(const char* lpStr) const
{
	if (lpStr)
	{
		FAString sTemp = *this;
		sTemp.Append(lpStr);
		return sTemp;
	}

	return *this;
}
FAString FAString::operator+(const char ch) const
{
	FAString sTemp = *this;
	char str[] = { ch, '\0' };
	sTemp.Append(str);
	return sTemp;
}

const FAString& FAString::operator+=(const FAString& src)
{
	Append(src);
	return *this;
}
const FAString& FAString::operator+=(const char* lpStr)
{
	if (lpStr)
	{
		Append(lpStr);
	}

	return *this;
}
const FAString& FAString::operator+=(const char ch)
{
	char str[] = { ch, '\0' };
	Append(str);
	return *this;
}

bool FAString::operator == (const char* str) const { return (Compare(str) == 0); };
bool FAString::operator != (const char* str) const { return (Compare(str) != 0); };
bool FAString::operator <= (const char* str) const { return (Compare(str) <= 0); };
bool FAString::operator <  (const char* str) const { return (Compare(str) < 0); };
bool FAString::operator >= (const char* str) const { return (Compare(str) >= 0); };
bool FAString::operator >  (const char* str) const { return (Compare(str) > 0); };

FAString& operator<<(FAString& str,const std::string &v)
{
	str << (v.c_str());
    return str;
}

template<typename T>
void FAString::Write(const T &src)
{
	std::stringstream str;
	str << src;
	Append(str.str().c_str());
}
FAString& FAString::operator<<(int8 v)
{
	Write<int8>(v);
	return (*this);
}
FAString& FAString::operator<<(int16 v)
{
	Write<int16>(v);
	return *this;
}
FAString& FAString::operator<<(int32 v)
{	
	Write<int32>(v);
	return *this;
}
FAString& FAString::operator<<(int64 v)
{
	Write<int64>(v);
	return *this;
}
FAString& FAString::operator<<(uint8 v)
{
	Write<uint8>(v);
	return *this;
}
FAString& FAString::operator<<(uint16 v)
{
	Write<uint16>(v);
	return *this;
}
FAString& FAString::operator<<(uint32 v)
{
	Write<uint32>(v);
	return *this;
}
FAString& FAString::operator<<(uint64 v)
{
	Write<uint64>(v);
	return *this;
}
#if FLIB_COMPILER_64BITS
FAString& FAString::operator<<(int v)
{
	Write<int>(v);
	return *this;
}
FAString& FAString::operator<<(uint v)
{
	Write<uint>(v);
	return *this;
}
#else
FAString& FAString::operator<<(long v)
{
	Write<long>(v);
	return *this;
}
FAString& FAString::operator<<(ulong v)
{
	Write<ulong>(v);
	return *this;
}
#endif//FLIB_COMPILER_64BITS
FAString& FAString::operator<<(bool v)
{
	Write<bool>(v);
	return *this;
}
FAString& FAString::operator<<(float v)
{
	Write<float>(v);
	return *this;
}
FAString& FAString::operator<<(double v)
{
	Write<double>(v);
	return *this;
}
FAString& FAString::operator<<(const char *str)
{
	Append(str);
	return *this;
}
FAString& FAString::operator<<(char str[])
{
	Append(str);
	return *this;
}
FAString& FAString::operator<<(const std::string& str)
{
	(*this) << (str.c_str());
	return *this;
}
FAString& FAString::operator<<(FAString &v)
{
	(*this) += v;
	return *this;
}
FAString& FAString::operator<< (FAString& (*_f)(FAString&))
{
	return _f(*this);
}

_FStdEnd
