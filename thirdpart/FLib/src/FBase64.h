#ifndef _FLIB_BASE64_H__
#define _FLIB_BASE64_H__
#pragma once
#include "FType.h"

_FStdBegin

F_DLL_API bool Fis_base64(unsigned char c);

F_DLL_API std::string Fbase64_encode(char const* bytes_to_encode, unsigned int in_len);

F_DLL_API std::string Fbase64_decode(char const* bytes_encoded, unsigned int in_len);

_FStdEnd

#define F_BASE64_ENCODE(str) _FStd(Fbase64_encode)(str, strlen(str))
#define F_BASE64_DECODE(str) _FStd(Fbase64_decode)(str)

#endif//_FLIB_BASE64_H__
