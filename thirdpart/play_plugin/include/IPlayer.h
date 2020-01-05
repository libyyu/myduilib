#ifndef _IPLAYER_H_
#define _IPLAYER_H_
#pragma once

#ifdef _MSC_VER
#ifdef PLAYER_PLUGIN_DLL_EXPORT
#define PLAYER_PLUGIN_DLL_API __declspec(dllexport)
#else
#define PLAYER_PLUGIN_DLL_API __declspec(dllimport)
#pragma comment(lib,"play_plugin.lib")
#endif
#else
#define PLAYER_PLUGIN_DLL_API
#endif

#include "DecoderType.h"

typedef enum 
{
	XPET_ERROR = 0,
	XPET_STATE_CHANGED,
    XPET_PLAY_DATA,
	XPET_PLAY_FINISHED,
	XPET_PLAY_NEXT,
	XPET_DOWN_DATA,
	XPET_DOWN_FAILED,
	XPET_DOWN_FINISHED,
}enumPlayerEventType;

typedef enum
{
	XPS_INIT = 0,
	XPS_PLAYING,
	XPS_PAUSE,
	XPS_STOP
}enumPlayerState;

struct AUDIO_INFO
{
	int nBitPerSample;
	int nChannel;
	int nSampleRate;
	int nSampleCnt;

	AUDIO_INFO()
	{
		nBitPerSample = 0;
		nChannel      = 0;
		nSampleRate   = 0;
		nSampleCnt    = 0;
	}
};

class IPlayer;
class IPlayerCallBack
{
public:
	virtual void OnPlayerCallBack(IPlayer* player,unsigned int ntype,void* lp,void* wp,void* pUserData) = 0;
};

class  IPlayer
{
public:
	IPlayer(void){};
	virtual ~IPlayer(void){};
	virtual void SetCallBack(IPlayerCallBack* pCallbackObj) = 0;
	virtual IPlayerCallBack* GetCallBack() const = 0;

	virtual enumPlayerState GetState() const = 0;
	virtual void Continue()  = 0;
	virtual void Pause()     = 0;
	virtual void Stop()      = 0;
	virtual float GetVolume() const = 0;
	virtual void SetVolume(float fVolume) = 0;

	virtual unsigned long GetFileBytes() const    = 0;
	virtual unsigned long GetDownloadBytes() const  = 0;

	virtual float GetPlayMSec() const = 0;
	virtual float GetCurrentPlayMSec() const = 0;
	virtual float GetBufPercent() const = 0;
	virtual void SeekPlayPosition(float fPercent) = 0;
	
	virtual int GetBitRate()  const   = 0;
	virtual int GetSampleRate() const = 0;
	virtual int GetChannelNum() const = 0;
	virtual int GetChannelType() const = 0;
	virtual int GetReadPosition() const = 0;

	virtual bool StartToPlay(
		const char* pwsFile,
		const char* pwsUrl,
		int nFileType,
		int nBegin,
		int nEnd,
		int nMSec,
		void* pUserData) = 0;
	virtual void PreCacheNextSong(const char* pwsUrl, void* pUserData) = 0;
};

#ifdef   __cplusplus
extern "C" {
#endif

	PLAYER_PLUGIN_DLL_API IPlayer*  create_player();
	PLAYER_PLUGIN_DLL_API void destroy_player(IPlayer* pPlayer);

#ifdef   __cplusplus
}
#endif

#endif//_IPLAYER_H_