#ifndef HTTPHELPER_H
#define HTTPHELPER_H
#pragma once
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <vector>
#ifdef _MSC_VER
#else
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#endif

typedef void* H_HTTPHANDLE;
#ifdef HTTP_STATIC_EXPORT
#define HTTP_API
#else
#ifdef _MSC_VER
#ifdef HTTP_DLL_EXPORT
#define HTTP_API __declspec(dllexport)
#else
#define HTTP_API __declspec(dllimport)
#pragma comment(lib,"http_plugin.lib")
#endif
#else
#define HTTP_API
#endif
#endif

typedef std::function<void(bool, const std::string&, void* userdata)> ResultCallback;
typedef std::function<int(double, double, void*)> ProgressCallback;

class HTTP_API IRequestHandle
{
public:
    virtual void OnDelete(void* userdata) {}
    virtual void OnRequestFinished(bool success, const std::string&, void* userdata) = 0;
    virtual int  OnProgress(double total_size, double handle_size, void* userdata) = 0;
};

class HTTP_API HTTPRequest
{
    static const int          s_kRetryCount = 3;
    static const int          s_kThreadCount = 4;
    static const long         s_kTimeout = 5000;
    
    friend class HTTPHelper;
    friend class HTTPImpl;
    class HTTPImpl* _impl;
    bool _auto_destroy;
    HTTPRequest(bool init = false);
    ~HTTPRequest();
public:
    void         Init();
    bool         IsAutoDestroy(){ return _auto_destroy; }
    void         SetAutoDestroy(bool autoDestroy) { _auto_destroy = autoDestroy; }
    int          Run(bool download, bool async);
    void         Stop();
    int          Resume();
    bool         IsRuning();
    bool         IsCancle();
    static HTTPRequest* Create(bool init = false);
    static void  Destroy(HTTPRequest*& request);
    static void  Tick(float dt);
public:
    void         SetRetryTimes(int retry_times = s_kRetryCount);
    void         SetTimeout(long time_out = s_kTimeout);
    void         SetUrl(const std::string& url);
    void         SetMovedUrl(bool get_moved_url);
    void         SetUserData(void* userdata);
    void*        GetUserData();
    int          GetRequestId();
    void         SetProgressCallback(ProgressCallback pc);
    void         SetResultCallback(ResultCallback rc);
    void         SetRequestHandle(IRequestHandle* handle);
    void         SetPostMethod(bool post);
    void         SetPostData(const std::string& message);
    void         SetPostData(const void* data, size_t size);
    void         SetSSLVerify(bool enable);
	void		 SetSSLCertificate(const std::string& filename);
    bool         SetDownloadMode(bool is_download);
    void         SetDownloadPath(const std::string& path);
    void         SetDownloadThreadCount(size_t count);
    void         SetDownloadRange(int from, int to = -1);
    
    void         SetRequestHeader(const std::map<std::string, std::string>& headers);
    void         SetRequestHeader(const std::string& header);
    void         SetRequestProxy(const std::string& proxy, long proxy_port);
    
    bool         GetHeader(std::string* header);
    bool         GetContent(std::string* receive);
    bool         GetErrorString(std::string* error_string);
    long         GetHttpCode();
};

#endif//HTTPHELPER_H
