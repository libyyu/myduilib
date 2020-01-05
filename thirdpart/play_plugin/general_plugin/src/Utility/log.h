#ifndef play_plugin_log_h
#define play_plugin_log_h
#pragma once
#include "flib.h"

void player_log(_FStd(FLIB_LOGLEVEL) level, const char* format, ...);

//#ifdef DEBUG
//#define  sLogout(lev,fmt,...)  (player_log(lev,"[%s][%d]"fmt,__FILE__,__LINE__,##__VA_ARGS__))
//#else
#define  sLogout(lev,fmt,...)  (player_log(lev,fmt,##__VA_ARGS__))
//#endif // DEBUG

#define  sLog(fmt,...)          (sLogout(_FStd(FLIB_LOGLEVEL)::FLIB_LOGLEVEL_INFO, fmt, ##__VA_ARGS__))
#define  sLogWarn(fmt,...)      (sLogout(_FStd(FLIB_LOGLEVEL)::FLIB_LOGLEVEL_WARN, fmt, ##__VA_ARGS__))
#define  sLogError(fmt,...)     (sLogout(_FStd(FLIB_LOGLEVEL)::FLIB_LOGLEVEL_ERROR, fmt, ##__VA_ARGS__))
#define  sLogFatal(fmt,...)     (sLogout(_FStd(FLIB_LOGLEVEL)::FLIB_LOGLEVEL_FATAL, fmt, ##__VA_ARGS__))



#endif//play_plugin_log_h
