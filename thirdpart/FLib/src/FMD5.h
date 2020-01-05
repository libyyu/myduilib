#ifndef _FLIB_MD5_H__
#define _FLIB_MD5_H__
#pragma once
#include "FType.h"

_FStdBegin

void F_DLL_API FMD5String(const char* string, char* output);
void F_DLL_API FMD5Binary(const char* string, unsigned char* output);
void F_DLL_API FMD5Salt(unsigned int len, char* output);

_FStdEnd

#endif//_FLIB_MD5_H__