#ifndef _FLIB_H_
#define _FLIB_H_
#pragma once
#include "FFunc.h"
#include "FBuffer.h"
#include "FAString.h"
#include "FWString.h"
#include "FConvert.h"
#include "FCounter.h"
#include "FEncoding.h"
#include "FFile.h"
#include "FIni.h"
#include "FLock.h"
#include "FMD5.h"
#include "FMemory.h"
#include "FMemPool.h"
#include "FAlloctor.h"
#include "FPool.h"
#include "FSemaphore.h"
#include "FThread.h"
#include "FPlugin.h"
#include "FProcess.h"
#include "FBase64.h"
#include "FDataTable.h"
#include "FLogFile.h"
#include "FConsole.h"
#include "FValue.h"
#include "FOStream.h"
#include "FSocket.h"
////////////////////////////////////
////
_FStdBegin
#if defined( UNICODE ) || defined( _UNICODE )
typedef  FWString  FString;
#else
typedef  FAString  FString;
#endif

typedef std::basic_string<char, std::char_traits<char>, FSTLAllocator<char> > fstring;
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, FSTLAllocator<wchar_t> > fwstring;

#if defined( UNICODE ) || defined( _UNICODE )
#define FLIB_TOSTRING  to_wstring
#else
#define FLIB_TOSTRING  to_string
#endif // UNICODE

_FStdEnd

#endif//_FLIB_H_
