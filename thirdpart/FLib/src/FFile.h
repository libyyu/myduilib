#ifndef __FLIB_FILE_H__
#define __FLIB_FILE_H__
#pragma once
#include "FType.h"

_FStdBegin

class F_DLL_API FFile
{
public:
	enum ENUM_SEEK { SEEK_FILE_BEGIN = 0, SEEK_FILE_CURRENT = 1, SEEK_FILE_END = 2 };
	FFile();
	virtual ~FFile();
public:
    int Open(const char* filename, bool readonly = true);
    int Close();
    int Flush();
    long GetSize();
    long GetOffset();	
    bool IsEOF();
    int SetEOF();
    int Seek(int offset, unsigned int mode);
    int Create(const char* filename);
    int Delete();

    long ReadAll(void* p_buffer);
    long Read(void* p_buffer, unsigned long n_bytes_2_read);
    long Write(const void* p_buffer, unsigned long n_bytes_2_write);

	operator bool() const;
private:
    bool _readonly;
    char _filename[256];
#if FLIB_COMPILER_WINDOWS
    FLIB_HANDLE _file;
#else
    int _file;
#endif
};


_FStdEnd
#endif//__FLIB_FILE_H__
