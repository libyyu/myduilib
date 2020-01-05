#pragma once
#include <string>
#include <map>
#ifdef _WIN32
#else
#include <pthread.h>
#endif

class CResDownloader;
class CResDownloaderDelegate
{
public:
	virtual void OnRecvHeader(CResDownloader* resDownloader,size_t fileSize) = 0;
	virtual void OnRecvData(CResDownloader* resDownloader,const	char* buf,size_t bytes,size_t fileSize) = 0;
	virtual void OnFinished(CResDownloader* resDownloader) = 0;
	virtual bool OnFailed(CResDownloader* resDownloader) = 0;
	virtual void OnCancled(CResDownloader* resDownloader) = 0;
};


class CResDownloader
{
	static const int MAX_ERROR_CNT = 3;
	static const int TIMEOUT = 5;
	static const unsigned short	SERVER_PORT = 80;
public:
	explicit CResDownloader(const char* psUrl,CResDownloaderDelegate* delegate);
	virtual ~CResDownloader();
public:
	void start();
	void stop();
	bool isStopped() const;
	bool isRunning() const;
protected:
#ifdef _WIN32
	static unsigned long __stdcall OnDownThread(void* p_void);
#else
	static void* OnDownThread(void* p_void);
#endif

	void OnDownPro() const;
protected:
	typedef CResDownloaderDelegate   DelegateT;
	DelegateT*    _delegate;
	bool          _isRunning;
	std::string   _sUrl;
#ifdef _WIN32
	void*        _hThread;
#else
	pthread_t    _thread;
#endif
};