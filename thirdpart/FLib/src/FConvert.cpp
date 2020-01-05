#define FLIB_DLL
#include "FConvert.h"
#include <locale.h>
#if FLIB_COMPILER_WINDOWS
#include <Windows.h>
#endif

_FStdBegin

bool FConvert::IsTextUTF8(const char* str)
{
	assert(str);
	unsigned long nBytes = 0; //UFT8可用1-6个字节编码,ASCII用一个字节 
	unsigned char chr;
	bool bAllAscii = true;
	for (unsigned int i = 0; str[i] != '\0'; ++i)
	{
		chr = *(str + i);
		//判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx 
		if (nBytes == 0 && (chr & 0x80) != 0)
			bAllAscii = false;
		if (nBytes == 0)
		{
			//如果不是ASCII码,应该是多字节符,计算字节数
			if (chr >= 0x80)
			{
				if (chr >= 0xFC && chr <= 0xFD)
					nBytes = 6;
				else if (chr >= 0xF8)
					nBytes = 5;
				else if (chr >= 0xF0)
					nBytes = 4;
				else if (chr >= 0xE0)
					nBytes = 3;
				else if (chr >= 0xC0)
					nBytes = 2;
				else
				{
					return false;
				}
				nBytes--;
			}
		}
		else
		{
			//多字节符的非首字节,应为 10xxxxxx 
			if ((chr & 0xC0) != 0x80)
			{
				return false;
			}
			//减到为零为止
			nBytes--;
		}
	}
	//违返UTF8编码规则 
	if (nBytes > 0)
	{
		return false;
	}
	if (bAllAscii) //如果全部都是ASCII, 也是UTF8
	{
		return true;
	}
	return true;
}

bool FConvert::IsTextGBK(const char* str)
{
	unsigned int nBytes = 0;//GBK可用1-2个字节编码,中文两个 ,英文一个 
	unsigned char chr = *str;
	bool bAllAscii = true; //如果全部都是ASCII,  

	for (unsigned int i = 0; str[i] != '\0'; ++i)
	{
		chr = *(str + i);
		if ((chr & 0x80) != 0 && nBytes == 0)
		{// 判断是否ASCII编码,如果不是,说明有可能是GBK
			bAllAscii = false;
		}

		if (nBytes == 0)
		{
			if (chr >= 0x80)
			{
				if (chr >= 0x81 && chr <= 0xFE)
				{
					nBytes = +2;
				}
				else {
					return false;
				}

				nBytes--;
			}
		}
		else
		{
			if (chr < 0x40 || chr>0xFE)
			{
				return false;
			}
			nBytes--;
		}//else end
	}

	if (nBytes != 0)
	{		//违返规则 
		return false;
	}

	if (bAllAscii)
	{ //如果全部都是ASCII, 也是GBK
		return true;
	}

	return true;
}

FConvert::stringtype FConvert::UTF16ToUTF8(const wchar_t* ptext)
{
	stringtype sResult;
#if FLIB_COMPILER_WINDOWS
	int nBufSize = ::WideCharToMultiByte(CP_UTF8, 0, ptext, -1, NULL, 0, 0, FALSE);
	char *szBuf = new char[nBufSize];
	::WideCharToMultiByte(CP_UTF8, 0, ptext, -1, szBuf, nBufSize, 0, FALSE);
	sResult = szBuf;
	delete[]szBuf;
	szBuf = NULL;
#else
	size_t len = wcslen(ptext);
	for (int i = 0; i < len; ++i)
	{
		char c;
		_UTF16ToUTF8ofChar(&c, &(ptext[i]));
		sResult += c;
	}
#endif
	return sResult;
}

FConvert::stringtype FConvert::UTF16ToGB2312(const wchar_t* ptext)
{
	stringtype sResult;

	stringtype curLocale = setlocale(LC_ALL, NULL);// curLocale = "C";
	setlocale(LC_ALL, "chs");

	size_t len = wcslen(ptext);
	size_t size = 2 * len + 1;
	char* p = new char[size];
	memset(p, 0, size);
	wcstombs(p, ptext, size);
	sResult = p;
	delete[]p;

	setlocale(LC_ALL, curLocale.c_str());

	return sResult;
}

FConvert::wstringtype FConvert::GB2312ToUTF16(const char* ptext)
{
	wstringtype sResult;

	stringtype curLocale = setlocale(LC_ALL, NULL);// curLocale = "C";
	setlocale(LC_ALL, "chs");

	size_t len = strlen(ptext);
	size_t size = len + 1;
	wchar_t *p = new wchar_t[size];
	wmemset(p, 0, size);
	mbstowcs(p, ptext, size);
	sResult = p;
	delete[]p;

	setlocale(LC_ALL, curLocale.c_str());

	return sResult;
}

FConvert::stringtype FConvert::GB2312ToUTF8(const char* ptext)
{
	stringtype sResult;
	char buf[4];
	memset(buf, 0, 4);

	size_t i = 0;
	size_t len = strlen(ptext);
	while (i < len)
	{
		if (ptext[i] >= 0)
		{
			char asciistr[2] = { 0 };
			asciistr[0] = (ptext[i++]);
			sResult.append(asciistr);
		}
		else
		{
			wchar_t pbuffer;
			_Gb2312ToUTF16ofChar(&pbuffer, *(ptext + i));

			_UTF16ToUTF8ofChar(buf, &pbuffer);

			sResult.append(buf);

			i += 2;
		}
	}

	return sResult;
}

FConvert::wstringtype FConvert::UTF8ToUTF16(const char* ptext)  //?
{
	stringtype stmp = UTF8ToGB2312(ptext);
	return GB2312ToUTF16(stmp.c_str());
}

FConvert::stringtype FConvert::UTF8ToGB2312(const char* ptext)
{
	stringtype sResult;
	size_t nlen = strlen(ptext);
	char buf[4];
	memset(buf, 0, 4);
	char* rst = new char[nlen + (nlen >> 2) + 2];
	memset(rst, 0, nlen + (nlen >> 2) + 2);

	size_t i = 0;
	size_t j = 0;

	while (i < nlen)
	{
		if (*(ptext + i) >= 0)
		{
			rst[j++] = ptext[i++];
		}
		else
		{
			wchar_t Wtemp;

			_UTF8ToUTF16ofChar(&Wtemp, ptext + i);
			_UTF16ToGB2312ofChar(buf, Wtemp);

			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j + 1] = buf[1];
			tmp = rst[j + 2] = buf[2];

			i += 3;
			j += 2;
		}
	}
	rst[j] = '\0';
	sResult = rst;
	delete[]rst;

	return sResult;
}

void FConvert::_UTF16ToUTF8ofChar(char* pOut, const wchar_t* pText)
{
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));
}
void FConvert::_UTF8ToUTF16ofChar(wchar_t* pOut, const char *pText)
{
	char* uchar = (char *)pOut;

	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);
}
void FConvert::_Gb2312ToUTF16ofChar(wchar_t* pOut, const char &gbBuffer)
{
#if FLIB_COMPILER_WINDOWS
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &gbBuffer, 2, pOut, 1);
#else
	char* pcurLocale = setlocale(LC_ALL, NULL);// curLocale = "C";
	setlocale(LC_ALL, "chs");
	mbstowcs(pOut, &gbBuffer, sizeof(gbBuffer));
	setlocale(LC_ALL, pcurLocale);
#endif
}
void FConvert::_UTF16ToGB2312ofChar(char* pOut, const wchar_t& uData)
{
#if FLIB_COMPILER_WINDOWS
	::WideCharToMultiByte(CP_ACP, NULL, &uData, 1, pOut, sizeof(uData), NULL, NULL);
#else
	char* pcurLocale = setlocale(LC_ALL, NULL);// curLocale = "C";
	setlocale(LC_ALL, "chs");
	wcstombs(pOut, &uData, sizeof(uData));
	setlocale(LC_ALL, pcurLocale);
#endif
}

_FStdEnd
