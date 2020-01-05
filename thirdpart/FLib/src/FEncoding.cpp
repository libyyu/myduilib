#define FLIB_DLL
#include "FEncoding.h"
#include <locale.h>
#include <iostream>
#include <sstream> 


_FStdBegin


static const unsigned char TextEncodingDetect_UTF16_BOM_LE[] = { static_cast<unsigned char>(0xFF), static_cast<unsigned char>(0xFE) };
static const unsigned char TextEncodingDetect_UTF16_BOM_BE[] = { static_cast<unsigned char>(0xFE), static_cast<unsigned char>(0xFF) };
static const unsigned char TextEncodingDetect_UTF8_BOM[] = { static_cast<unsigned char>(0xEF), static_cast<unsigned char>(0xBB), static_cast<unsigned char>(0xBF) };

static const unsigned char* utf16_bom_le_ = TextEncodingDetect_UTF16_BOM_LE;
static const unsigned char* utf16_bom_be_ = TextEncodingDetect_UTF16_BOM_BE;
static const unsigned char* utf8_bom_ = TextEncodingDetect_UTF8_BOM;

FEncoding::FEncoding()
{
	// By default, assume nulls can't appear in ANSI/ASCII/UTF8 text files
	null_suggests_binary_ = true;

	// Set defaults for utf16 detection based the use of odd/even nulls
	utf16_expected_null_percent_ = 70;
	utf16_unexpected_null_percent_ = 10;
}

FEncoding::Encoding FEncoding::CheckBOM(const unsigned char *pBuffer, size_t size)		// Just check if there is a BOM and return
{
	// Check for BOM
	if (size >= 2 && pBuffer[0] == utf16_bom_le_[0] && pBuffer[1] == utf16_bom_le_[1])
	{
		return Encoding::UTF16_LE_BOM;
	}
	else if (size >= 2 && pBuffer[0] == utf16_bom_be_[0] && pBuffer[1] == utf16_bom_be_[1])
	{
		return Encoding::UTF16_BE_BOM;
	}
	else if (size >= 3 && pBuffer[0] == utf8_bom_[0] && pBuffer[1] == utf8_bom_[1] && pBuffer[2] == utf8_bom_[2])
	{
		return Encoding::UTF8_BOM;
	}
	else
	{
		return Encoding::None;
	}
}
int FEncoding::GetBOMLengthFromEncodingMode(FEncoding::Encoding encoding)			// Just return the BOM length of a given mode
{
	int length = 0;

	if (encoding == Encoding::UTF16_BE_BOM || encoding == Encoding::UTF16_LE_BOM)
		length = 2;
	else if (encoding == Encoding::UTF8_BOM)
		length = 3;

	return length;
}
FEncoding::Encoding FEncoding::DetectEncoding(const unsigned char *pBuffer, size_t size) const	// Check BOM and also guess if there is no BOM
{
	// First check if we have a BOM and return that if so
	Encoding encoding = CheckBOM(pBuffer, size);
	if (encoding != Encoding::None)
		return encoding;

	// Now check for valid UTF8
	encoding = CheckUTF8(pBuffer, size);
	if (encoding != Encoding::None)
		return encoding;

	// Now try UTF16 
	encoding = CheckUTF16NewlineChars(pBuffer, size);
	if (encoding != Encoding::None)
		return encoding;

	encoding = CheckUTF16ASCII(pBuffer, size);
	if (encoding != Encoding::None)
		return encoding;

	// ANSI or None (binary) then
	if (!DoesContainNulls(pBuffer, size))
		return Encoding::ANSI;
	else
	{
		// Found a null, return based on the preference in null_suggests_binary_
		if (null_suggests_binary_)
			return Encoding::None;
		else
			return Encoding::ANSI;
	}
}

void FEncoding::SetNullSuggestsBinary(bool null_suggests_binary) { null_suggests_binary_ = null_suggests_binary; }
void FEncoding::SetUtf16UnexpectedNullPercent(int percent)
{
	if (percent > 0 && percent < 100)
		utf16_expected_null_percent_ = percent;
}
void FEncoding::SetUtf16ExpectedNullPercent(int percent)
{
	if (percent > 0 && percent < 100)
		utf16_unexpected_null_percent_ = percent;
}

FEncoding::Encoding FEncoding::CheckUTF8(const unsigned char *pBuffer, size_t size) const				// Check for valid UTF8 with no BOM
{
	// UTF8 Valid sequences
	// 0xxxxxxx  ASCII
	// 110xxxxx 10xxxxxx  2-byte
	// 1110xxxx 10xxxxxx 10xxxxxx  3-byte
	// 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  4-byte
	//
	// Width in UTF8
	// Decimal		Width
	// 0-127		1 byte
	// 194-223		2 bytes
	// 224-239		3 bytes
	// 240-244		4 bytes
	//
	// Subsequent chars are in the range 128-191

	bool	only_saw_ascii_range = true;
	size_t	pos = 0;
	int		more_chars;

	while (pos < size)
	{
		unsigned char ch = pBuffer[pos++];

		if (ch == 0 && null_suggests_binary_)
		{
			return None;
		}
		else if (ch <= 127)
		{
			// 1 byte
			more_chars = 0;
		}
		else if (ch >= 194 && ch <= 223)
		{
			// 2 Byte
			more_chars = 1;
		}
		else if (ch >= 224 && ch <= 239)
		{
			// 3 Byte
			more_chars = 2;
		}
		else if (ch >= 240 && ch <= 244)
		{
			// 4 Byte
			more_chars = 3;
		}
		else
		{
			return Encoding::None;						// Not utf8
		}

		// Check secondary chars are in range if we are expecting any
		while (more_chars && pos < size)
		{
			only_saw_ascii_range = false;		// Seen non-ascii chars now

			ch = pBuffer[pos++];
			if (ch < 128 || ch > 191)
				return Encoding::None;					// Not utf8

			--more_chars;
		}
	}

	// If we get to here then only valid UTF-8 sequences have been processed

	// If we only saw chars in the range 0-127 then we can't assume UTF8 (the caller will need to decide)
	if (only_saw_ascii_range)
		return Encoding::ASCII;
	else
		return Encoding::UTF8_NOBOM;
}

FEncoding::Encoding FEncoding::CheckUTF16NewlineChars(const unsigned char *pBuffer, size_t size)	// Check for valid UTF16 with no BOM via control chars
{
	if (size < 2)
		return Encoding::None;

	// Reduce size by 1 so we don't need to worry about bounds checking for pairs of bytes
	size--;

	int le_control_chars = 0;
	int be_control_chars = 0;
	unsigned char ch1, ch2;

	size_t pos = 0;
	while (pos < size)
	{
		ch1 = pBuffer[pos++];
		ch2 = pBuffer[pos++];

		if (ch1 == 0)
		{
			if (ch2 == 0x0a || ch2 == 0x0d)
				++be_control_chars;
		}
		else if (ch2 == 0)
		{
			if (ch1 == 0x0a || ch1 == 0x0d)
				++le_control_chars;
		}

		// If we are getting both LE and BE control chars then this file is not utf16
		if (le_control_chars && be_control_chars)
			return Encoding::None;
	}

	if (le_control_chars)
		return Encoding::UTF16_LE_NOBOM;
	else if (be_control_chars)
		return Encoding::UTF16_BE_NOBOM;
	else
		return Encoding::None;
}

FEncoding::Encoding FEncoding::CheckUTF16ASCII(const unsigned char *pBuffer, size_t size) const		// Check for valid UTF16 with no BOM via null distribution
{
	int num_odd_nulls = 0;
	int num_even_nulls = 0;

	// Get even nulls
	size_t pos = 0;
	while (pos < size)
	{
		if (pBuffer[pos] == 0)
			num_even_nulls++;

		pos += 2;
	}

	// Get odd nulls
	pos = 1;
	while (pos < size)
	{
		if (pBuffer[pos] == 0)
			num_odd_nulls++;

		pos += 2;
	}

	double even_null_threshold = (num_even_nulls * 2.0) / size;
	double odd_null_threshold = (num_odd_nulls * 2.0) / size;
	double expected_null_threshold = utf16_expected_null_percent_ / 100.0;
	double unexpected_null_threshold = utf16_unexpected_null_percent_ / 100.0;

	// Lots of odd nulls, low number of even nulls
	if (even_null_threshold < unexpected_null_threshold	&& odd_null_threshold > expected_null_threshold)
		return Encoding::UTF16_LE_NOBOM;

	// Lots of even nulls, low number of odd nulls
	if (odd_null_threshold < unexpected_null_threshold && even_null_threshold > expected_null_threshold)
		return Encoding::UTF16_BE_NOBOM;

	// Don't know
	return Encoding::None;
}

bool FEncoding::DoesContainNulls(const unsigned char *pBuffer, size_t size)			// Check for nulls
{
	size_t pos = 0;
	while (pos < size)
	{
		if (pBuffer[pos++] == 0)
			return true;
	}

	return false;
}

_FStdEnd