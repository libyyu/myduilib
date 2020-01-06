#ifndef BASE_ALL_H
#define BASE_ALL_H

/*****************************************************************************************
Cross platform building switch
*****************************************************************************************/
//#define BUILD_CROSS_PLATFORM

/*****************************************************************************************
Unicode
*****************************************************************************************/
#ifdef _UNICODE

#else

#endif // #ifdef _UNICODE

/*****************************************************************************************
Global includes
*****************************************************************************************/
#ifdef _WIN32
	#include <windows.h>
    #include <mmsystem.h>
    #include <atlstr.h>
#else
    #include <unistd.h>
    #include <time.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "SmartPtr.h"

namespace em_mp3_tag
{
#define MY_MIN(a, b) ((a) < (b) ? (a) : (b))
#define MY_MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef _WIN32
    typedef unsigned __int32                            uint32;
    typedef __int32                                     int32;
    typedef unsigned __int16                            uint16;
    typedef __int16                                     int16;
    typedef unsigned __int8                             uint8;
    typedef __int8                                      int8;
    typedef char                                        str_ansi;
    typedef unsigned char                               str_utf8;
    typedef wchar_t                                     str_utf16;

    #define IO_USE_FILE_IO
    #define IO_HEADER_FILE                              "FileIO.h"
    #define IO_CLASS_NAME                               file_io
    #define DLLEXPORT                                   __declspec(dllexport)
    #define SLEEP(MILLISECONDS)                         ::Sleep(MILLISECONDS)
    #define MESSAGEBOX(PARENT, TEXT, CAPTION, TYPE)     ::MessageBox(PARENT, TEXT, CAPTION, TYPE)
    #define PUMP_MESSAGE_LOOP                           { MSG Msg; while (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE) != 0) { TranslateMessage(&Msg); DispatchMessage(&Msg); } }
    #define ODS                                         OutputDebugString
    #define TICK_COUNT_TYPE                             unsigned long
    #define TICK_COUNT_READ(VARIABLE)                   VARIABLE = GetTickCount()
    #define TICK_COUNT_FREQ                             1000
#else
    typedef char                                        str_ansi;
    typedef unsigned char                               str_utf8;
    typedef wchar_t                                     str_utf16;

    #define IO_USE_FILE_IO
    #define IO_HEADER_FILE                              "FileIO.h"
    #define IO_CLASS_NAME                               file_io
    #define DLLEXPORT
    #define SLEEP(MILLISECONDS)                         { struct timespec t; t.tv_sec = (MILLISECONDS) / 1000; t.tv_nsec = (MILLISECONDS) % 1000 * 1000000; nanosleep(&t, NULL); }
    #define MESSAGEBOX(PARENT, TEXT, CAPTION, TYPE)
    #define PUMP_MESSAGE_LOOP
    #define ODS                                         printf
    #define TICK_COUNT_TYPE                             unsigned long long
    #define TICK_COUNT_READ(VARIABLE)                   { struct timeval t; gettimeofday(&t, NULL); VARIABLE = t.tv_sec * 1000000LLU + t.tv_usec; }
    #define TICK_COUNT_FREQ                             1000000
#endif

/*****************************************************************************************
Byte order
*****************************************************************************************/
#define __LITTLE_ENDIAN     1234
#define __BIG_ENDIAN        4321
#define __BYTE_ORDER        __LITTLE_ENDIAN

/*****************************************************************************************
Macros
*****************************************************************************************/
#define SAFE_DELETE(POINTER) if (POINTER) { delete POINTER; POINTER = NULL; }
#define SAFE_ARRAY_DELETE(POINTER) if (POINTER) { delete [] POINTER; POINTER = NULL; }
#define SAFE_VOID_CLASS_DELETE(POINTER, Class) { Class *pClass = (Class *) POINTER; if (pClass) { delete pClass; POINTER = NULL; } }
#ifdef _WIN32
#define SAFE_FILE_CLOSE(HANDLE) if (HANDLE != INVALID_HANDLE_VALUE) { CloseHandle(HANDLE); HANDLE = INVALID_HANDLE_VALUE; }
#else
#define SAFE_FILE_CLOSE(HANDLE) if (HANDLE != -1) { ::close(HANDLE); HANDLE = -1; }
#endif

#define ODN(NUMBER) { TCHAR cNumber[16]; _stprintf(cNumber, _T("%d\n"), int(NUMBER)); ODS(cNumber); }

#define CATCH_ERRORS(CODE) try { CODE } catch(...) { }

#define RETURN_ON_ERROR(FUNCTION) {    int nRetVal = FUNCTION; if (nRetVal != 0) { return nRetVal; } }
#define RETURN_VALUE_ON_ERROR(FUNCTION, VALUE) { int nRetVal = FUNCTION; if (nRetVal != 0) { return VALUE; } }
#define RETURN_ON_EXCEPTION(CODE, VALUE) { try { CODE } catch(...) { return VALUE; } }
#define THROW_ON_ERROR(CODE) { int nRetVal = CODE; if (nRetVal != 0) throw(nRetVal); }

/*****************************************************************************************
Error Codes
*****************************************************************************************/

// success
#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS                                   0
#endif

// file and i/o errors (1000's)
#define ERROR_IO_READ                                   1000
#define ERROR_IO_WRITE                                  1001
#define ERROR_INVALID_INPUT_FILE                        1002
#define ERROR_INVALID_OUTPUT_FILE                       1003
#define ERROR_INPUT_FILE_TOO_LARGE                      1004
#define ERROR_INPUT_FILE_UNSUPPORTED_BIT_DEPTH          1005
#define ERROR_INPUT_FILE_UNSUPPORTED_SAMPLE_RATE        1006
#define ERROR_INPUT_FILE_UNSUPPORTED_CHANNEL_COUNT      1007
#define ERROR_INPUT_FILE_TOO_SMALL                      1008
#define ERROR_INVALID_CHECKSUM                          1009
#define ERROR_DECOMPRESSING_FRAME                       1010
#define ERROR_INITIALIZING_UNMAC                        1011
#define ERROR_INVALID_FUNCTION_PARAMETER                1012
#define ERROR_UNSUPPORTED_FILE_TYPE                     1013
#define ERROR_UPSUPPORTED_FILE_VERSION                  1014

// memory errors (2000's)
#define ERROR_INSUFFICIENT_MEMORY                       2000

// dll errors (3000's)
#define ERROR_LOADINGAPE_DLL                            3000
#define ERROR_LOADINGAPE_INFO_DLL                       3001
#define ERROR_LOADING_UNMAC_DLL                         3002

// general and misc errors
#define ERROR_USER_STOPPED_PROCESSING                   4000
#define ERROR_SKIPPED                                   4001

// programmer errors
#define ERROR_BAD_PARAMETER                             5000

// unknown error
#define ERROR_UNDEFINED                                -1

#define ERROR_EXPLANATION \
    { ERROR_IO_READ                               , "I/O read error" },                         \
    { ERROR_IO_WRITE                              , "I/O write error" },                        \
    { ERROR_INVALID_INPUT_FILE                    , "invalid input file" },                     \
    { ERROR_INVALID_OUTPUT_FILE                   , "invalid output file" },                    \
    { ERROR_INPUT_FILE_TOO_LARGE                  , "input file file too large" },              \
    { ERROR_INPUT_FILE_UNSUPPORTED_BIT_DEPTH      , "input file unsupported bit depth" },       \
    { ERROR_INPUT_FILE_UNSUPPORTED_SAMPLE_RATE    , "input file unsupported sample rate" },     \
    { ERROR_INPUT_FILE_UNSUPPORTED_CHANNEL_COUNT  , "input file unsupported channel count" },   \
    { ERROR_INPUT_FILE_TOO_SMALL                  , "input file too small" },                   \
    { ERROR_INVALID_CHECKSUM                      , "invalid checksum" },                       \
    { ERROR_DECOMPRESSING_FRAME                   , "decompressing frame" },                    \
    { ERROR_INITIALIZING_UNMAC                    , "initializing unmac" },                     \
    { ERROR_INVALID_FUNCTION_PARAMETER            , "invalid function parameter" },             \
    { ERROR_UNSUPPORTED_FILE_TYPE                 , "unsupported file type" },                  \
    { ERROR_INSUFFICIENT_MEMORY                   , "insufficient memory" },                    \
    { ERROR_LOADINGAPE_DLL                        , "loading MAC.dll" },                        \
    { ERROR_LOADINGAPE_INFO_DLL                   , "loading MACinfo.dll" },                    \
    { ERROR_LOADING_UNMAC_DLL                     , "loading UnMAC.dll" },                      \
    { ERROR_USER_STOPPED_PROCESSING               , "user stopped processing" },                \
    { ERROR_SKIPPED                               , "skipped" },                                \
    { ERROR_BAD_PARAMETER                         , "bad parameter" },                          \
    { ERROR_UNDEFINED                             , "undefined" },                              \

}			//end namespace em_mp3_tag

#endif // #ifndef BASE_ALL_H
