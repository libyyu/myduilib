#ifndef __FLIB_ENCODING_H__
#define __FLIB_ENCODING_H__
#pragma once
#include "FType.h"

_FStdBegin


class F_DLL_API FEncoding
{
public:
    enum Encoding
    {
        None = 0,			// Unknown or binary
        ANSI,				// 0-255
        ASCII,				// 0-127
        UTF8_BOM,			// UTF8 with BOM
        UTF8_NOBOM,			// UTF8 without BOM
        UTF16_LE_BOM,		// UTF16 LE with BOM
        UTF16_LE_NOBOM,		// UTF16 LE without BOM
        UTF16_BE_BOM,		// UTF16-BE with BOM
        UTF16_BE_NOBOM,		// UTF16-BE without BOM
    };

	FEncoding();
	~FEncoding() {}

	static Encoding CheckBOM(const unsigned char *pBuffer, size_t size);
	static int GetBOMLengthFromEncodingMode(Encoding encoding);
	Encoding DetectEncoding(const unsigned char *pBuffer, size_t size) const;

	void SetNullSuggestsBinary(bool null_suggests_binary);
	void SetUtf16UnexpectedNullPercent(int percent);
	void SetUtf16ExpectedNullPercent(int percent);
private:
    FEncoding(const FEncoding&);
    const FEncoding& operator=(const FEncoding&);

    bool	null_suggests_binary_;
    int		utf16_expected_null_percent_;
    int		utf16_unexpected_null_percent_;
    ///////////////////////////////////////////////////////////////////////////////
    // Checks if a buffer contains valid utf8. Returns:
    // None - not valid utf8
    // UTF8_NOBOM - valid utf8 encodings and multibyte sequences
    // ASCII - Only data in the 0-127 range. 
    ///////////////////////////////////////////////////////////////////////////////
	Encoding CheckUTF8(const unsigned char *pBuffer, size_t size) const;

    ///////////////////////////////////////////////////////////////////////////////
    // Checks if a buffer contains text that looks like utf16 by scanning for 
    // newline chars that would be present even in non-english text.
    // Returns:
    // None - not valid utf16
    // UTF16_LE_NOBOM - looks like utf16 le
    // UTF16_BE_NOBOM - looks like utf16 be
    ///////////////////////////////////////////////////////////////////////////////
	static Encoding CheckUTF16NewlineChars(const unsigned char *pBuffer, size_t size);

    ///////////////////////////////////////////////////////////////////////////////
    // Checks if a buffer contains text that looks like utf16. This is done based
    // the use of nulls which in ASCII/script like text can be useful to identify.
    // Returns:
    // None - not valid utf16
    // UTF16_LE_NOBOM - looks like utf16 le
    // UTF16_BE_NOBOM - looks like utf16 be
    ///////////////////////////////////////////////////////////////////////////////
	Encoding CheckUTF16ASCII(const unsigned char *pBuffer, size_t size) const;

    ///////////////////////////////////////////////////////////////////////////////
    // Checks if a buffer contains any nulls. Used to check for binary vs text data.
    ///////////////////////////////////////////////////////////////////////////////
	static bool DoesContainNulls(const unsigned char *pBuffer, size_t size);
};

_FStdEnd

#endif//__FLIB_ENCODING_H__