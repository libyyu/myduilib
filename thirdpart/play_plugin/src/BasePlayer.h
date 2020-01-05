#pragma once
#include "IPlayer.h"
#include "operationQueue/operationQueue.h"
#include "playerQ/playerQueue.h"
#include "decoder_plugin.h"
#include "DecoderPluginMgr.h"
#include <memory>
#include <mutex>

class CBasePlayer : 
	public IPlayer,
	public std::enable_shared_from_this<CBasePlayer>
{
	static const int MIN_PCM_BUFSIZE = 640*1024; //1024*187.5 = 192000
	static const int PCM_BUFSIZE = MIN_PCM_BUFSIZE*2;
public:
	typedef std::shared_ptr<void>                  spDecoderHandleT;
public:
	CBasePlayer(void);
	~CBasePlayer(void);

	virtual void SetCallBack(IPlayerCallBack* pCallbackObj);
	virtual IPlayerCallBack* GetCallBack() const;

	virtual enumPlayerState GetState() const;
	virtual void Continue();
	virtual void Pause();
	virtual void Stop();
	virtual float GetVolume() const;
	virtual void SetVolume(float fVolume);

	virtual unsigned long GetFileBytes() const;
	virtual unsigned long GetDownloadBytes() const;

	virtual float GetPlayMSec() const;
	virtual float GetCurrentPlayMSec() const;
	virtual float GetBufPercent() const;
	virtual void SeekPlayPosition(float fPercent);

	virtual int GetBitRate()  const;
	virtual int GetSampleRate() const;
	virtual int GetChannelNum() const;
	virtual int GetChannelType() const;
	virtual int GetReadPosition() const;

	virtual bool StartToPlay(
		const char* pwsFile,
		const char* pwsUrl,
		int nFileType,
		int nBegin,
		int nEnd,
		int nMSec,
		void* pUserData);
	virtual void PreCacheNextSong(const char* pwsUrl, void* pUserData);
protected:
	void OnIPlayerCallBack(enumPlayerEventType type,void* lp,void* wp) const;
	void SyncPlayerState(HPlayerQueue hPlayerQueue);
	bool CheckFileExist(const char*pszFileName) const;
	bool IsNeedExitPlayPro() const;
	void FadInOutVolume(HPlayerQueue hPlayerQueue,float fIn,float fOut) const;

	virtual void OnPlayBegin(){};
	virtual void OnPlay(
		const std::string& wsFile,
		const std::string& wsUrl,
		int nFileType,
		int nBegin,
		int nEnd) = 0;
	virtual void OnPlayEnd(){};

	void DefaultPlayPro(
		const std::string& wsFile,
		decoder_plugin* pPlugin,
		spDecoderHandleT spDecoder,
		int nBegin,
		int nEnd,
		std::function<bool (void)> isExitLoopPred = NULL);

	decoder_handle OpenDecoder(
		const std::string& wsFile,
		bool isOnline,
		decoder_plugin **pPlugin,
		int nFileType = 0,
		int nBegin = 0,
		int nEnd = 0);

private:
	void Play(
		const std::string& wsFile,
		const std::string& wsUrl,
		int nFileType,
		int nBegin,
		int nEnd,
		int nMSec,
		void* pUserData);
private:
	typedef std::mutex          LockT;
#if _HAS_CXX17
	typedef std::scoped_lock						ScopedLockT;
#else
	typedef std::unique_lock<LockT>					ScopedLockT;
#endif

	LockT                         _lock;
	IPlayerCallBack*              _pIPlayerCallBack;

	spOperationQueueT             _spOpQueue;
	HPlayerQueue                  _hPlayerQueue;

	enumPlayerState               _state;

	CXiamiDecoderPluginMgr        _decoderPluginMgr;

	float                         _fSeekPercent;
	float                         _fNewSeekPercent;
	float                         _fCurTime; //ms
	float                         _fTotalTime;
	
	float                         _fVolume;
	
	player_audio_info			  _audioInfo;

	void*                         _pUserData;
	bool                          _isNewSeek;
	bool                          _bOnLine;
	float                         _fBufPer;
	int                           _nReadPos;
};

