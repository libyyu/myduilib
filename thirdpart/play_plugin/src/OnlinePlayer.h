#pragma once
#include "LocalPlayer.h"
#include "ResDownloader.h"
#include "utility/P2PInterface.h"
#include <memory>
#include <mutex>

class COnlinePlayer;
struct P2PDownLoad : public P2PLibInterface::DownUserInfo
{
	COnlinePlayer*            _pOnLinePlayer;
	decoder_plugin*			  _pDecoderPlugin;
	std::shared_ptr<void>     _hDecoder;
	std::string               _sHash;
	size_t                    _nRedownCnt;   /* 重试次数 */
	size_t                    _nDownloadSize;/* 实际接收到的文件大小  */
	size_t                    _nFileSize;    /* 文件总大小  */
	std::string               _SongID;       /* 仅为打印日志需要  */
};


class COnlinePlayer :
	public CLocalPlayer,
	public CResDownloaderDelegate,
	public P2PLibInterface::DownUserInfo

{
public:
 	explicit COnlinePlayer();
 	~COnlinePlayer();
public:
	virtual void OnRecvHeader(CResDownloader* resDownloader,size_t fileSize);
	virtual void OnRecvData(CResDownloader* resDownloader,const	char* buf,size_t bytes,size_t fileSize);
	virtual void OnFinished(CResDownloader* resDownloader);
	virtual bool OnFailed(CResDownloader* resDownloader);
	virtual void OnCancled(CResDownloader* resDownloader);
public:
	virtual unsigned long GetFileBytes() const;
	virtual unsigned long GetDownloadBytes() const;
	virtual void PreCacheNextSong(const char* pwsUrl, void* pUserData);
protected:
	virtual void OnPlay(
		const std::string& wsFile,
		const std::string& wsUrl,
		int nFileType,
		int nBegin,
		int nEnd);

	bool IsDownFailed() const;
	std::string SubStrSongID(const std::string& wsFile);
	void BeginP2PService();
private:
	static void ResInfoCallBack(DownUserInfo* info,size_t resLength);
	static void DownloadCallBack(DownUserInfo* info, size_t begin, size_t length, void* buffer);
	static void EndCallBack(DownUserInfo* info, P2PLibInterface::CallBackData* pStatus);

	/*预缓存处理部分*/
	static void PreCacheEndCallBack(DownUserInfo* info, P2PLibInterface::CallBackData* pStatus);
protected:
	decoder_plugin*  _pDecoderPlugin;
	decoder_handle   _hDecoder;

	bool                   _isDownFailed;

	bool                   _isOnline;
	size_t                 _nFileBytes;
	size_t                 _nDownBytes;
	std::mutex		       _mutex;

	typedef std::shared_ptr<P2PDownLoad> spP2PDownloaderT;
	typedef std::set<spP2PDownloaderT>  setT;
	setT                   _set;

	void*                 _hP2PServ;
};

