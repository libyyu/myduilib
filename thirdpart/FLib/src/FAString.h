#ifndef __FLIB_STRING_H__
#define __FLIB_STRING_H__
#pragma once
#include "FType.h"

_FStdBegin

class F_DLL_API FAString
{
public:
    enum { MAX_LOCAL_STRING_LEN = 63 };
	FAString();
	FAString(const char ch);
	FAString(const char* lpsz, int nLen = -1);
	FAString(const FAString& src);
	~FAString();
public:
	void Empty();
	size_t GetLength() const;
	bool IsEmpty() const;
	char GetAt(int nIndex) const;
	char operator[] (int nIndex) const;

	void SetAt(int nIndex, char ch);

	void Append(const char* pstr);

	void Assign(const char* pstr, int cchMax = -1);

	int Compare(const char* lpsz) const;

	int CompareNoCase(const char* lpsz) const;

	FAString Left(int iLength) const;

	FAString Mid(int iPos, int iLength = -1) const;

	FAString Right(int iLength) const;

	int Find(char ch, int iPos = 0) const;
	int Find(const char* pstrSub, int iPos = 0) const;

	int ReverseFind(char ch) const;

	int Replace(const char* pstrFrom, const char* pstrTo);

	int Format(const char* pstrFormat, ...);

	int SmallFormat(const char* pstrFormat, ...);

	static FAString Printf(const char* pstrFormat, ...);

	static FAString SmallPrintf(const char* pstrFormat, ...);
	
	void SplitToArray(std::vector<FAString>& OutArr, const char* pattern);

	void TrimLeft();
    
	void TrimRight();
    
	void Trim();

	virtual operator char* () const;

	const FAString& operator=(const char ch);

	const FAString& operator=(const FAString& src);

	const FAString& operator=(const char* lpStr);

	FAString operator+(const FAString& src) const;

	FAString operator+(const char* lpStr) const;

	FAString operator+(const char ch) const;

	const FAString& operator+=(const FAString& src);

	const FAString& operator+=(const char* lpStr);

	const FAString& operator+=(const char ch);


	bool operator == (const char* str) const;
	bool operator != (const char* str) const;
	bool operator <= (const char* str) const;
	bool operator <  (const char* str) const;
	bool operator >= (const char* str) const;
	bool operator >  (const char* str) const;

	template<typename T>
    FAString& operator<<(T v); // will generate link error
    FAString& operator<<(int8 v);
    FAString& operator<<(int16 v);
    FAString& operator<<(int32 v);
    FAString& operator<<(int64 v);
    FAString& operator<<(uint8 v);
    FAString& operator<<(uint16 v);
    FAString& operator<<(uint32 v);
    FAString& operator<<(uint64 v);
#if FLIB_COMPILER_64BITS
    FAString& operator<<(int v);
    FAString& operator<<(uint v);
#else
	FAString& operator<<(long v);
	FAString& operator<<(ulong v);
#endif
    FAString& operator<<(bool v);
    FAString& operator<<(float v);
    FAString& operator<<(double v);
    FAString& operator<<(const char *str);
	FAString& operator<<(char v[]);
	FAString& operator<<(std::string& str);
    FAString& operator<<(FAString &v);
	FAString& operator<< (FAString& (*_f)(FAString&));

	friend FAString& endl(FAString& v);
	friend FAString& operator<<(FAString& str, const std::string& v);
protected:
	template<typename T>
	void Write(const T &src);
private:
	char* m_pstr;
	char m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
};


_FStdEnd

#endif//__FLIB_STRING_H__
