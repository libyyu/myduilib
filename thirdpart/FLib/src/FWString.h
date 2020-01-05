#ifndef __FLIB_WSTRING_H__
#define __FLIB_WSTRING_H__
#pragma once
#include "FType.h"

_FStdBegin

class F_DLL_API FWString
{
public:
    enum { MAX_LOCAL_STRING_LEN = 63 };
	FWString();
	FWString(const wchar_t ch);
	FWString(const wchar_t* lpsz, int nLen = -1);
	FWString(const FWString& src);
	virtual ~FWString();
public:
	void Empty();
	size_t GetLength() const;
	bool IsEmpty() const;
	wchar_t GetAt(int nIndex) const;
	wchar_t operator[] (int nIndex) const;

	void SetAt(int nIndex, wchar_t ch);

	void Append(const wchar_t* pstr);

	void Assign(const wchar_t* pstr, int cchMax = -1);
	int Compare(const wchar_t* lpsz) const;

	int CompareNoCase(const wchar_t* lpsz) const;

	FWString Left(int iLength) const;

	FWString Mid(int iPos, int iLength = -1) const;

	FWString Right(int iLength) const;

	int Find(wchar_t ch, int iPos = 0) const;

	int Find(const wchar_t* pstrSub, int iPos = 0) const;

	int ReverseFind(wchar_t ch) const;

	int Replace(const wchar_t* pstrFrom, const wchar_t* pstrTo);

	int Format(const wchar_t* pstrFormat, ...);

	int SmallFormat(const wchar_t* pstrFormat, ...);

	static FWString Printf(const wchar_t* pstrFormat, ...);

	static FWString SmallPrintf(const wchar_t* pstrFormat, ...);

	void SplitToArray(std::vector<FWString>& OutArr, const wchar_t* pattern);

	void TrimLeft();
    
	void TrimRight();
    
	void Trim();

	virtual operator wchar_t* () const;

	const FWString& operator=(const wchar_t ch);

	const FWString& operator=(const FWString& src);
	const FWString& operator=(const wchar_t* lpStr);

	FWString operator+(const FWString& src) const;
	FWString operator+(const wchar_t* lpStr) const;
	FWString operator+(const wchar_t ch) const;

	const FWString& operator+=(const FWString& src);
	const FWString& operator+=(const wchar_t* lpStr);
	const FWString& operator+=(const wchar_t ch);

	bool operator == (const wchar_t* str) const;
	bool operator != (const wchar_t* str) const;
	bool operator <= (const wchar_t* str) const;
	bool operator <  (const wchar_t* str) const;
	bool operator >= (const wchar_t* str) const;
	bool operator >  (const wchar_t* str) const;

	template<typename T>
    FWString& operator<<(T v); // will generate link error
    FWString& operator<<(int8 v);
    FWString& operator<<(int16 v);
    FWString& operator<<(int32 v);
    FWString& operator<<(int64 v);
    FWString& operator<<(uint8 v);
    FWString& operator<<(uint16 v);
    FWString& operator<<(uint32 v);
    FWString& operator<<(uint64 v);
#if FLIB_COMPILER_64BITS
    FWString& operator<<(int v);
    FWString& operator<<(uint v);
#else
	FWString& operator<<(long v);
	FWString& operator<<(ulong v);
#endif
    FWString& operator<<(bool v);
    FWString& operator<<(float v);
    FWString& operator<<(double v);
    FWString& operator<<(const wchar_t *str);
	FWString& operator<<(wchar_t v[]);
	FWString& operator<<(std::wstring& str);
    FWString& operator<<(FWString &v);
	FWString& operator<< (FWString& (*_f)(FWString&));

	friend FWString& endl(FWString& v);
	friend FWString& operator<<(FWString& str, const std::wstring& v);
protected:
	template<typename T>
	void Write(const T &src);
private:
	wchar_t* m_pstr;
	wchar_t m_szBuffer[MAX_LOCAL_STRING_LEN + 1];
};


_FStdEnd

#endif//__FLIB_WSTRING_H__
