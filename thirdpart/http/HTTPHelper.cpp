
#include <list>
#include <set>
#include <regex>
#include <sstream>
#include <exception>
#include <cassert>
#include <fstream>
#include <iostream>
#include "curl/curl.h"        //libcurl interface
#include "HTTPHelper.h"
#ifdef _MSC_VER
#include <Windows.h>
#endif

class HTTPImpl;

class HTTPLock
{
public:
#ifdef _MSC_VER
    HTTPLock() { InitializeCriticalSection(&m_cs); }
    ~HTTPLock() { DeleteCriticalSection(&m_cs); }
    
    void Lock() { EnterCriticalSection(&m_cs); }
    void UnLock() { LeaveCriticalSection(&m_cs); }
#else
    HTTPLock() { pthread_mutex_init(&m_lock, NULL); }
    ~HTTPLock() { pthread_mutex_destroy(&m_lock); }
    
    int Lock(){ return pthread_mutex_lock(&m_lock); }
    int UnLock() { return pthread_mutex_unlock(&m_lock); }
#endif
    
private:
#ifdef _MSC_VER
    CRITICAL_SECTION m_cs;
#else
    pthread_mutex_t    m_lock;
#endif
};

class DoHTTPLock
{
public:
    DoHTTPLock(HTTPLock* lock)
    : m_lock(lock)
    {
        assert(m_lock);
        m_lock->Lock();
    }
    
    ~DoHTTPLock()
    {
        if(m_lock) m_lock->UnLock();
    }
    
private:
    HTTPLock* m_lock;
};

class HTTPHelper
{
private:
    std::set<HTTPRequest*>          s_async_requests;
    std::shared_ptr<HTTPLock>       s_request_lock;
    CURLSH*                         s_share_handle;
    static int s_id;
    
    HTTPHelper()
    : s_share_handle(nullptr)
    , s_request_lock(new HTTPLock)
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        
        s_share_handle = curl_share_init();
        curl_share_setopt(s_share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
    }
public:
    ~HTTPHelper()
    {
        s_request_lock->Lock();
        
        std::set<HTTPRequest*>::iterator p;
        for (p = s_async_requests.begin(); p != s_async_requests.end(); ++p)
        {
            HTTPRequest* request = *p;
            request->Stop();
			if (request->IsAutoDestroy())
			{
				HTTPRequest::Destroy(request);
			}
        }
        s_async_requests.clear();
        s_request_lock->UnLock();

        curl_share_cleanup(s_share_handle);
        curl_global_cleanup();
        s_share_handle = nullptr;
    }
    static HTTPHelper* Instance()
    {
        static HTTPHelper the_single_instance;
        return &the_single_instance;
    }
    
    static void h_Sleep(unsigned int dt)
    {
#ifdef _MSC_VER
        ::Sleep(dt);
#else
        usleep(dt * 1000);
#endif
    }
    
    CURLSH* GetSharedCurl()
    {
        return s_share_handle;
    }
    int  GenID()
    {
        s_id++;
        return s_id;
    }
    bool FindRequest(HTTPRequest* request)
    {
        s_request_lock->Lock();
        auto p = s_async_requests.find(request);
        if(p != s_async_requests.end())
        {
            s_request_lock->UnLock();
            return true;
        }
        s_request_lock->UnLock();
        return false;
    }
    void RemoveRequest(HTTPRequest* request)
    {
        s_request_lock->Lock();
        std::set<HTTPRequest*>::const_iterator p = s_async_requests.find(request);
        if(p != s_async_requests.end())
        {
            s_async_requests.erase(p);
        }
        s_request_lock->UnLock();
        if(request && request->IsAutoDestroy())
        {
            HTTPRequest::Destroy(request);
        }
    }
    void AddRequest(HTTPRequest* request)
    {
        s_request_lock->Lock();
        s_async_requests.insert(request);
        s_request_lock->UnLock();
    }
    
    void Tick(float dt)
    {
        bool empty = false;
        while(!empty)
        {
            s_request_lock->Lock();
            empty = s_async_requests.empty();
            s_request_lock->UnLock();
            h_Sleep(200);
        }
    }
};

int HTTPHelper::s_id = 0;

class HTTPImpl
{
    friend class HTTPRequest;
private:
    HTTPRequest*       m_owner;
    H_HTTPHANDLE       m_share_handle;
    H_HTTPHANDLE       m_http_headers;
#ifdef _MSC_VER
    H_HTTPHANDLE       m_perform_thread;
#else
    pthread_t          m_perform_thread;
#endif
    
#ifdef _MSC_VER
    std::list<H_HTTPHANDLE> m_download_threads;
#else
    std::list<pthread_t>    m_download_threads;
#endif
    
    int                m_id;
    int                m_retry_times;
    long               m_time_out;
    bool               m_is_running;
    bool               m_is_download;
    long               m_http_code;
    double             m_downloaded_size;
    double             m_total_size;
    int                m_thread_count;
    int                m_download_range_begin;
    int                m_download_range_end;
    bool               m_multi_download;
    bool               m_download_fail;
    bool               m_is_cancel;
    bool               m_is_async;
    
    std::string        m_url;
    std::string        m_download_path;
    std::string        m_receive_content;
    std::string        m_receive_header;
    std::string        m_error_string;
    char*              m_post_data;
    size_t             m_post_data_size;
    void*              m_userdata;
    
    ResultCallback     m_result_callback;
    ProgressCallback   m_progress_callback;
    IRequestHandle*    m_request_handle;
    HTTPLock*          m_download_lock;
    
    //
    bool               m_enable_ssl;
	std::string        m_certificate_file;
    bool               m_enable_redirect;
    bool               m_use_post;
    std::string        m_headers;
    std::string        m_proxy;
    long               m_proxy_port;
    
    typedef struct tDownloadThreadChunk
    {
        static int  counter;
        int         _id;
        FILE*       _fp;
        long        _startidx;
        long        _endidx;
        
        HTTPImpl*     _download;
        
        tDownloadThreadChunk():_fp(nullptr), _download(nullptr)
        {
            _id = ++counter;
        }
    }DownloadThreadChunk;

public:
    HTTPImpl(int id, HTTPRequest* owner, H_HTTPHANDLE share_handle = nullptr)
    : m_id(id)
    , m_owner(owner)
    , m_share_handle(share_handle)
    , m_http_headers(nullptr)
#ifdef _MSC_VER
    , m_perform_thread(nullptr)
#else
    , m_perform_thread(pthread_t())
#endif
    , m_is_running(false)
    , m_retry_times(HTTPRequest::s_kRetryCount)
    , m_time_out(HTTPRequest::s_kTimeout)
    , m_http_code(0)
    , m_post_data(nullptr)
    , m_post_data_size(0)
    , m_result_callback(nullptr)
    , m_progress_callback(nullptr)
    , m_request_handle(nullptr)
    , m_userdata(nullptr)
    , m_is_download(false)
    , m_downloaded_size(0.0)
    , m_total_size(0.0)
    , m_thread_count(HTTPRequest::s_kThreadCount)
    , m_download_range_begin(0)
    , m_download_range_end(0)
    , m_multi_download(false)
    , m_download_fail(false)
    , m_is_cancel(false)
    , m_download_lock(nullptr)
    , m_is_async(false)
    
    , m_enable_ssl(false)
    , m_enable_redirect(true)
    , m_use_post(false)
    , m_proxy_port(0)
    {
    }
    ~HTTPImpl()
    {
        if(m_request_handle)
        {
            m_request_handle->OnDelete(m_owner);
        }
        Release();
        printf("[%d] ~HTTPImpl\n", m_id);
    }
    void Release()
    {    
        Close();

        if (m_http_headers)
        {
            curl_slist_free_all(reinterpret_cast<curl_slist*>(m_http_headers));
            m_http_headers = nullptr;
        }
        
        delete m_post_data;
        m_post_data = nullptr;

        delete m_download_lock;
        m_download_lock = nullptr;
        
        m_owner = nullptr;
        m_is_running = false;
        m_result_callback = nullptr;
        m_progress_callback = nullptr;
        m_request_handle = nullptr;
        printf("[%d] HTTPImpl::Release\n", m_id);
    }
    int  GetRequestId()
    {
        return m_id;
    }
    void SetRetryTimes(int retry_times)
    {
        m_retry_times = retry_times;
    }
    void SetTimeout(long time_out)
    {
        m_time_out = time_out;
    }
    void SetSSLVerify(bool enable)
    {
        m_enable_ssl = enable;
    }
	void SetSSLCertificate(const std::string& filename)
	{
		m_certificate_file = filename;
	}
    void SetUrl(const std::string& url)
    {
        m_url = url;
    }
    void SetMovedUrl(bool get_moved_url)
    {
        m_enable_redirect = get_moved_url;
    }
    bool SetDownloadMode(bool is_download)
    {
        if(m_is_running) return false;
        m_is_download = is_download;
        return true;
    }
    void SetDownloadPath(const std::string& path)
    {
        m_download_path = path;
    }
    void SetDownloadThreadCount(int count)
    {
        m_thread_count = count;
    }
    void SetDownloadRange(int from, int to)
    {
        m_download_range_begin = from;
        m_download_range_end = to;
    }
    
    void SetUserData(void* userdata)
    {
        m_userdata = userdata;
    }
    void* GetUserData()
    {
        return m_userdata;
    }

    void SetProgressCallback(ProgressCallback pc)
    {
        m_progress_callback = pc;
    }
    void SetResultCallback(ResultCallback rc)
    {
        m_result_callback = rc;
    }
    void SetRequestHandle(IRequestHandle* handle)
    {
        m_request_handle = handle;
    }
    void SetPostMethod(bool post)
    {
        m_use_post = post;
    }
    void SetPostData(const void* data, size_t size)
    {
        m_post_data_size = size;
        if (m_post_data)
        {
            delete m_post_data;
            m_post_data = nullptr;
        }
        if(size)
        {
            m_post_data = new char[size+1];
            m_post_data[size] = 0x0;
            memcpy(m_post_data, data, size);
        }
    }
    
    void SetRequestHeader(const std::string& header)
    {
        m_headers = header;
    }
    
    void SetRequestProxy(const std::string& proxy, long proxy_port)
    {
        m_proxy = proxy;
        m_proxy_port = proxy_port;
    }

    bool     GetHeader(std::string* header)
    {
        if (m_receive_header.empty()) return false;
        else if (header) *header = m_receive_header;
        
        return true;
    }
    bool     GetContent(std::string* receive)
    {
        if (m_receive_content.empty()) return false;
        else if (receive) *receive = m_receive_content;
        
        return true;
    }
    bool     GetErrorString(std::string* error_string)
    {
        if (m_error_string.empty()) return false;
        else if (error_string) *error_string = m_error_string;
        
        return true;
    }
    
    long     GetHttpCode() { return m_http_code; }
    
public:
    int Run(bool async)
    {
        if (m_is_running)
        {
            return 0;
        }
        m_is_async = async;
        HTTPHelper::Instance()->AddRequest(m_owner);
        
        if (!async)
        {
            int code = Perform();
            printf("Finish And Will Remove\n");
            HTTPHelper::Instance()->RemoveRequest(m_owner);
            return code;
        }
        else
        {
#ifdef _MSC_VER
            DWORD thread_id;
            m_perform_thread = (H_HTTPHANDLE)CreateThread(NULL, 0, RequestThread, this, 0, &thread_id);
#else
            pthread_create(&m_perform_thread, NULL, RequestThread, this);
#endif
            return 0;
        }
    }
    void Stop()
    {
        if(!m_is_running)
        {
            return;
        }
        Abort();
        printf("thread stoped.\n");
    }
    int Resume()
    {
        return Run(m_is_async);
    }
    void Close()
    {
		if (!m_is_running)
		{
			return;
		}

        if(HTTPHelper::Instance()->FindRequest(m_owner))
        {
#ifdef _MSC_VER
            if (WaitForSingleObject(m_perform_thread, INFINITE) != WAIT_OBJECT_0)
#else
            if(pthread_join(m_perform_thread) != 0)
#endif
            {
                printf("Failed to join thread.\n");
            }

			HTTPHelper::Instance()->RemoveRequest(m_owner);	
        }

        m_is_cancel = true;
        m_is_running = false;
        printf("close thread.\n");
    }
protected:
    void    ReqeustResultDefault(bool success, const std::string& content)
    {
        if(m_request_handle)
        {
            m_request_handle->OnRequestFinished(success, content, m_owner);
        }
        if(m_result_callback)
        {
            m_result_callback(success, content, m_owner);
        }
    }
    int    ProgressDefault(double total_size, double downloaded_size)
    {
        if(m_request_handle)
        {
            m_request_handle->OnProgress(total_size, downloaded_size, m_owner);
        }
        if(m_progress_callback)
        {
            m_progress_callback(total_size, downloaded_size, m_owner);
        }
        
        return 0;
    }
    
    static size_t RetriveHeaderFunction(void *buffer, size_t size, size_t nitems, void *userdata)
    {
        std::string* receive_header = reinterpret_cast<std::string*>(userdata);
        if (receive_header && buffer)
        {
            receive_header->append(reinterpret_cast<const char*>(buffer), size * nitems);
        }
        
        return nitems * size;
    }
    
    static size_t RetriveContentFunction(void *ptr, size_t size, size_t nmemb, void *userdata)
    {
        std::string* receive_content = reinterpret_cast<std::string*>(userdata);
        if (receive_content && ptr)
        {
            receive_content->append(reinterpret_cast<const char*>(ptr), size * nmemb);
        }
        
        return nmemb * size;
    }
    
    static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
    {
        DownloadThreadChunk* thread_chunk = reinterpret_cast<DownloadThreadChunk*>(userdata);
        
        if (thread_chunk->_download->m_is_cancel || !thread_chunk->_download->m_is_running)
        {
            return 0;
        }
        
        DoHTTPLock http_lock(thread_chunk->_download->m_download_lock);
        
        size_t written = 0;
        size_t real_size = size * nmemb;
        if (thread_chunk->_endidx > 0)
        {
            if (thread_chunk->_startidx <= thread_chunk->_endidx)
            {
                if (thread_chunk->_startidx + real_size > thread_chunk->_endidx)
                {
                    real_size = thread_chunk->_endidx - thread_chunk->_startidx + 1;
                }
            }
        }
        
        int seek_error = fseek(thread_chunk->_fp, thread_chunk->_startidx, SEEK_SET);
        if (seek_error != 0)
        {
            perror("fseek");
        }
        else
        {
            written = fwrite(ptr, 1, real_size, thread_chunk->_fp);
        }
        thread_chunk->_download->m_downloaded_size += written;
        thread_chunk->_startidx += written;
        
        return written;
    }
    
    static int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
    {
        DownloadThreadChunk* thread_chunk = reinterpret_cast<DownloadThreadChunk*>(clientp);
		if (thread_chunk->_download->m_is_cancel || !thread_chunk->_download->m_is_running)
		{
			return 0;
		}
        DoHTTPLock http_lock(thread_chunk->_download->m_download_lock);
        double total_size = thread_chunk->_download->m_total_size;
        double downloaded_size = thread_chunk->_download->m_downloaded_size;
        thread_chunk->_download->ProgressDefault(total_size, downloaded_size);
        
        return 0;
    }
    
#ifdef _MSC_VER
    static DWORD WINAPI RequestThread(LPVOID param)
#else
    static void* RequestThread(void* param)
#endif
    {
        
        HTTPHelper::h_Sleep(10);

        HTTPImpl* request = reinterpret_cast<HTTPImpl*>(param);
        
        if (request)
        {
            request->Perform();
            HTTPHelper::Instance()->RemoveRequest(request->m_owner);
        }
        
#ifdef _MSC_VER
        return 1;
#else
        return NULL;
#endif
    }
#ifdef _MSC_VER
    static DWORD WINAPI DownloadWork(LPVOID param)
#else
    static void* DownloadWork(void* param)
#endif
    {
        DownloadThreadChunk* thread_chunk = reinterpret_cast<DownloadThreadChunk*>(param);
        
#ifdef _MSC_VER
        return thread_chunk->_download->DoDownload(thread_chunk);
#else
        thread_chunk->_download->DoDownload(thread_chunk);
        return NULL;
#endif
    }
    static int DoDownload(DownloadThreadChunk* thread_chunk)
    {
        CURL* curl_handle = curl_easy_init();
        if(!curl_handle)
        {
            thread_chunk->_download->m_download_fail = false;
            delete thread_chunk;
            return -1;
        }
        if(thread_chunk->_download->m_share_handle)
        {
            curl_easy_setopt(curl_handle, CURLOPT_SHARE, thread_chunk->_download->m_share_handle);
            curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);
        }
        
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, thread_chunk->_download->m_enable_ssl);
        curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, thread_chunk->_download->m_enable_ssl);
		if (!thread_chunk->_download->m_certificate_file.empty())
			curl_easy_setopt(curl_handle, CURLOPT_CAINFO, thread_chunk->_download->m_certificate_file.c_str());
        
        curl_easy_setopt(curl_handle, CURLOPT_URL, thread_chunk->_download->m_url.c_str());
        
        const char* user_agent = ("Mozilla/5.0 (Windows NT 5.1; rv:5.0) Gecko/20100101 Firefox/5.0");
        curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, user_agent);
        
        curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 5L);
        curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        
        curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_POST, 0L);
        
        curl_easy_setopt(curl_handle, CURLOPT_FORBID_REUSE, 1);
        curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT_MS, thread_chunk->_download->m_time_out); // ?
        curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, 0);   //0 means block always
        
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, thread_chunk);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, RetriveHeaderFunction);
        curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, NULL);
        
        curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 0L);
#ifdef CURLOPT_XFERINFODATA
        curl_easy_setopt(curl_handle, CURLOPT_XFERINFOFUNCTION, progress_callback);
		curl_easy_setopt(curl_handle, CURLOPT_XFERINFODATA, thread_chunk);
#else
		curl_easy_setopt(curl_handle, CURLOPT_PROGRESSFUNCTION, progress_callback);
		curl_easy_setopt(curl_handle, CURLOPT_PROGRESSDATA, thread_chunk);
#endif 
        
        
        curl_easy_setopt(curl_handle, CURLOPT_LOW_SPEED_LIMIT, 1L);
        curl_easy_setopt(curl_handle, CURLOPT_LOW_SPEED_TIME, 5L);
        
        //if (thread_chunk->_endidx != 0)
        {
            std::string down_range;
            std::ostringstream ostr;
            if (thread_chunk->_endidx > 0)
            {
                ostr << thread_chunk->_startidx << "-" << thread_chunk->_endidx;
            }
			else
            {
                ostr << thread_chunk->_startidx << "-";
            }
            
            down_range = ostr.str();
			if (!down_range.empty())
			{
				curl_easy_setopt(curl_handle, CURLOPT_RANGE, down_range.c_str());
				printf("[%d] DoDownload[thread id:%d] load range: %s\n", thread_chunk->_download->m_id, thread_chunk->_id, down_range.c_str());
			}
        }
		
        CURLcode curl_code = curl_easy_perform(curl_handle);
        if (curl_code == CURLE_OPERATION_TIMEDOUT)
        {
            int retry_count = thread_chunk->_download->m_retry_times;
            while (retry_count > 0 && thread_chunk->_download->m_is_running)
            {
                printf("[%d] DoDownload[thread id:%d] Connect Timeout, Will Try %d/%d\n",
					   thread_chunk->_download->m_id,
                       thread_chunk->_id,
                       1+thread_chunk->_download->m_retry_times-retry_count,
                       thread_chunk->_download->m_retry_times);
                curl_code = curl_easy_perform(curl_handle);
                if (curl_code != CURLE_OPERATION_TIMEDOUT) break;
                retry_count--;
            }
        }
        if (curl_code == CURLE_OPERATION_TIMEDOUT)
        {
            printf("[%d] DoDownload[thread id:%d] Connect Timeout\n", thread_chunk->_download->m_id, thread_chunk->_id);
        }
        
        long http_code;
        curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
        if (curl_code == CURLE_OK && (http_code >= 200 && http_code <= 300))
        {
            thread_chunk->_download->m_http_code = http_code;
            thread_chunk->_download->m_download_fail = false;
        }
        else
        {
            const char* err_string = curl_easy_strerror(curl_code);
            thread_chunk->_download->m_error_string = err_string;
            thread_chunk->_download->m_download_fail = true;
            thread_chunk->_download->m_http_code = http_code;
        }
        
        curl_easy_cleanup(curl_handle);
		printf("[%d] DoDownload[thread id:%d] exit.\n", thread_chunk->_download->m_id, thread_chunk->_id);
        delete thread_chunk;
        return curl_code;
    }
protected:
    void Abort()
    {
		try
		{
#ifdef _MSC_VER
			if (m_perform_thread)
			{
				CloseHandle(m_perform_thread);
				m_perform_thread = nullptr;
			}
#else
			pthread_kill(m_perform_thread, 0);
#endif
		}
		catch (std::exception e) 
        {
			printf("close thread exception\n");
		}

		HTTPHelper::Instance()->RemoveRequest(m_owner);
		m_is_cancel = true;
        m_is_running = false;
        printf("abort thread.\n");
    }
    int Perform()
    {
		printf("[%d] Start Perform request:%d, download:%d \n", m_id, m_is_download?0:1, m_is_download?1:0);
        m_is_running = true;
        m_is_cancel = false;
        m_receive_header.clear();
        m_receive_content.clear();
        int code = 0;
        if(m_is_download)
        {
            code = PerformDownload();
        }
        else
        {
            code = PerformRequest();
        }
        printf("[%d] Perform Finish %d\n", m_id, code);
        return code;
    }
    int PerformRequest()
    {
        H_HTTPHANDLE curl_handle = curl_easy_init();
        if(m_share_handle)
        {
            curl_easy_setopt(curl_handle, CURLOPT_SHARE, m_share_handle);
            curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);
        }
        
        if (curl_handle)
        {
            CURLcode curl_code;
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_URL, m_url.c_str());
            //set force http redirect
            if(m_enable_redirect)
            {
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_MAXREDIRS, 5);
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
            }
            else
            {
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 0L);
            }
            //Post
            if(m_use_post)
            {
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_POST, 1);
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, m_post_data ? m_post_data : "");
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, m_post_data_size);
            }
            //headers
            if(!m_headers.empty())
            {
                m_http_headers = curl_slist_append(reinterpret_cast<curl_slist*>(m_http_headers), m_headers.c_str());
            }
            if (m_http_headers)
            {
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, reinterpret_cast<curl_slist*>(m_http_headers));
            }
            //Proxy
            if(!m_proxy.empty())
            {
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_PROXYPORT, m_proxy_port);
                curl_code = curl_easy_setopt(curl_handle, CURLOPT_PROXY, m_proxy.c_str());
            }
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, RetriveHeaderFunction);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &m_receive_header);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, RetriveContentFunction);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &m_receive_content);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_NOSIGNAL, 1);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_CONNECTTIMEOUT_MS, 5000);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT_MS, m_time_out);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, m_enable_ssl);
            curl_code = curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, m_enable_ssl);
			if(!m_certificate_file.empty())
				curl_code = curl_easy_setopt(curl_handle, CURLOPT_CAINFO, m_certificate_file.c_str());
            
            curl_code = curl_easy_perform(curl_handle);
            if (curl_code == CURLE_OPERATION_TIMEDOUT)
            {
                int retry_count = m_retry_times;
                while (retry_count > 0)
                {
                    printf("[%d] PerformRequest Connect Timeout, Will Try %d/%d\n", m_id, 1+m_retry_times-retry_count, m_retry_times);
                    curl_code = curl_easy_perform(curl_handle);
                    if (curl_code != CURLE_OPERATION_TIMEDOUT) break;
                    retry_count--;
                }
            }
            if (curl_code == CURLE_OPERATION_TIMEDOUT)
            {
                printf("[%d] PerformRequest Connect Timeout\n", m_id);
            }
            
            curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &m_http_code);
            if (curl_code == CURLE_OK && m_http_code == 200)
            {
                ReqeustResultDefault(true, m_receive_content);
            }
            else
            {
                const char* err_string = curl_easy_strerror(curl_code);
                m_error_string = err_string;
                curl_code = CURLE_HTTP_POST_ERROR;
                ReqeustResultDefault(false, m_error_string);
            }
            
            if (m_http_headers)
            {
                curl_slist_free_all(reinterpret_cast<curl_slist*>(m_http_headers));
                m_http_headers = nullptr;
            }
            m_is_running = false;
            curl_easy_cleanup(curl_handle);
            return curl_code;
        }
        
        m_error_string = "Failed to curl_easy_init";
        ReqeustResultDefault(false, m_error_string);
        return -1;
    }
    
    int PerformDownload()
    {
        if(!m_download_lock)
            m_download_lock = new HTTPLock;
        bool success = GetDownloadFileSize(m_total_size);
        printf("[%d] GetDownloadFileSize size:%f, success:%s\n", m_id, m_total_size, success ? "true" : "false");
        if (!success)
        {
            m_is_running = false;
            ReqeustResultDefault(false, m_error_string);
            return -1;
        }
        
        std::string out_file_name = m_download_path;
        std::string src_file_name = out_file_name;
        out_file_name += ".dl";
        
        FILE *fp = nullptr;
#ifdef _MSC_VER
        remove(out_file_name.c_str());
        fp = fopen(out_file_name.c_str(), "wb");
#else
        unlink(out_file_name.c_str());
        fp = fopen(out_file_name.c_str(), "wb");
#endif
        if (!fp)
        {
			m_error_string = "failed to open " + out_file_name;
            m_is_running = false;
            ReqeustResultDefault(false, m_error_string);
            return -1;
        }
        
        int down_code = 0;
        int thread_count = SplitDownloadCount(m_total_size);
        
        m_thread_count = thread_count > m_thread_count ? m_thread_count : thread_count;
		printf("[%d] split thread count %d\n", m_id, m_thread_count);
        //文件大小有分开下载的必要并且服务器支持多线程下载时，启用多线程下载
        if (m_multi_download && m_thread_count > 1)
        {
            long gap = static_cast<long>(m_total_size) / m_thread_count;
            
            for (int i = 0; i < m_thread_count; i++)
            {
                DownloadThreadChunk* thread_chunk = new DownloadThreadChunk;
                thread_chunk->_fp = fp;
                thread_chunk->_download = this;
                
                if (i < m_thread_count - 1)
                {
                    thread_chunk->_startidx = i * gap + m_download_range_begin;
                    thread_chunk->_endidx = thread_chunk->_startidx + gap - 1;
                }
                else
                {
                    thread_chunk->_startidx = i * gap;
                    thread_chunk->_endidx = m_download_range_end;
                }
                
#ifdef _MSC_VER
                DWORD thread_id;
                H_HTTPHANDLE hThread = (H_HTTPHANDLE)CreateThread(NULL, 0, DownloadWork, thread_chunk, 0, &(thread_id));
#else
                pthread_t hThread;
                pthread_create(&hThread, NULL, DownloadWork, thread_chunk);
#endif
                m_download_threads.push_back(hThread);
            }
            
#ifdef _MSC_VER
			/*DWORD ret = WaitForMultipleObjects(m_download_threads.size(), &(m_download_threads.front()), TRUE, INFINITE);
			while (WAIT_OBJECT_0 + m_download_threads.size() != ret && WAIT_FAILED != ret)
			{
				HTTPHelper::h_Sleep(10);
			}
			if (WAIT_FAILED == ret)
			{
				m_is_running = false;
				m_download_fail = true;
				m_error_string = "WaitForMultipleObjects occured error " + std::to_string(::GetLastError());
			}*/
            for (H_HTTPHANDLE handle : m_download_threads)
            {
				DWORD dwEvent = WaitForSingleObject(handle, INFINITE);
				if (dwEvent == WAIT_OBJECT_0)
				{
					CloseHandle(handle);
				}
				else if (dwEvent == WAIT_FAILED)
				{
					char tmpbuff[10] = { 0 };
#ifdef _MSC_VER
					sprintf(tmpbuff, "%ld", ::GetLastError());
#else
					sprintf(tmpbuff, "%ld", errno);
#endif
					m_download_fail = true;
					m_error_string = "WaitForSingleObject occured error ";
					m_error_string += tmpbuff;
					CloseHandle(handle);
				}
				HTTPHelper::h_Sleep(10);
            }
#else
            for(pthread_t thread : m_download_threads)
            {
                pthread_join(thread, NULL);
            }
#endif
            m_download_threads.clear();
            
            if(m_download_fail)
                down_code = -1;
        }
        else
        {
            DownloadThreadChunk* thread_chunk = new DownloadThreadChunk;
            thread_chunk->_fp = fp;
            thread_chunk->_download = this;
            thread_chunk->_startidx = m_download_range_begin;
            thread_chunk->_endidx = m_download_range_end;
            down_code = DoDownload(thread_chunk);
        }
        
        fclose(fp);
        
        if (!m_download_fail)
        {
#ifdef _MSC_VER
            MoveFileExA(out_file_name.c_str(), src_file_name.c_str(), MOVEFILE_REPLACE_EXISTING);
#else
            unlink(src_file_name.c_str());
            rename(out_file_name.c_str(), src_file_name.c_str());
#endif
        }
        else
        {
#ifdef _MSC_VER
            remove(out_file_name.c_str());
#else
            unlink(out_file_name.c_str());
#endif
        }

		m_is_running = false;
        ReqeustResultDefault(m_download_fail ? false : true, m_error_string);
        
        return down_code;
    }
    
    bool GetDownloadFileSize(double& down_file_length)
    {
        if (m_url.empty())
        {
            return false;
        }
        else
        {
            CURL *handle = curl_easy_init();
            if(!handle)
                return false;
            if(m_share_handle)
            {
                curl_easy_setopt(handle, CURLOPT_SHARE, m_share_handle);
                curl_easy_setopt(handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);
            }
            
            if (handle)
            {
                curl_easy_setopt(handle, CURLOPT_URL, m_url.c_str());
                curl_easy_setopt(handle, CURLOPT_HEADER, 1);
                curl_easy_setopt(handle, CURLOPT_NOBODY, 1);
                curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, RetriveHeaderFunction);
                curl_easy_setopt(handle, CURLOPT_HEADERDATA, &m_receive_header);
                curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, RetriveContentFunction);
                curl_easy_setopt(handle, CURLOPT_WRITEDATA, NULL);
                curl_easy_setopt(handle, CURLOPT_CONNECTTIMEOUT_MS, 5000);
                curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, m_time_out);   //0 means block always
                curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, m_enable_ssl);
                curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, m_enable_ssl);
				if(!m_certificate_file.empty())
					curl_easy_setopt(handle, CURLOPT_CAINFO, m_certificate_file.c_str());
           
                if(m_download_range_begin >=0)
                {
                    std::string down_range;
                    std::ostringstream ostr;
                    
                    ostr << m_download_range_begin << "-";
                    
                    if(m_download_range_end >0)
                    {
                        ostr << m_download_range_end;
                    }
                    down_range = ostr.str();
                    curl_easy_setopt(handle, CURLOPT_RANGE, down_range.c_str());
                }
                
                //set force http redirect
                if(m_enable_redirect)
                {
                    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 5);
                    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
                }
                else
                {
                    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 0L);
                }
                //Post
                if(m_use_post)
                {
                    curl_easy_setopt(handle, CURLOPT_POST, 1);
                    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, m_post_data ? m_post_data : "");
                    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, m_post_data_size);
                }
                //headers
                if(!m_headers.empty())
                {
                    m_http_headers = curl_slist_append(reinterpret_cast<curl_slist*>(m_http_headers), m_headers.c_str());
                }
                if (m_http_headers)
                {
                    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, reinterpret_cast<curl_slist*>(m_http_headers));
                }
                //Proxy
                if(!m_proxy.empty())
                {
                    curl_easy_setopt(handle, CURLOPT_PROXYPORT, m_proxy_port);
                    curl_easy_setopt(handle, CURLOPT_PROXY, m_proxy.c_str());
                }
                
                
                CURLcode curl_code = curl_easy_perform(handle);
                
                if (curl_code == CURLE_OPERATION_TIMEDOUT)
                {
                    int retry_count = m_retry_times;
                    while (retry_count > 0)
                    {
                        printf("GetDownloadFileSize Connect Timeout, Will Try %d/%d\n", 1+m_retry_times-retry_count, m_retry_times);
                        curl_code = curl_easy_perform(handle);
                        if (curl_code != CURLE_OPERATION_TIMEDOUT) break;
                        retry_count--;
                    }
                }
                
                if (curl_code == CURLE_OPERATION_TIMEDOUT)
                {
                    printf("GetDownloadFileSize Connect Timeout\n");
                }
                
                bool success = false;
                curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &m_http_code);
                
                if (curl_code == CURLE_OK)
                {
                    success = true;
                    curl_easy_getinfo(handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &down_file_length);
                    
                    try
                    {
                        //匹配"Content-Range: bytes 2-1449/26620" 则证明支持多线程下载
                        std::regex pattern("CONTENT-RANGE\\s*:\\s*\\w+\\s*(\\d+)-(\\d*)/(\\d+)", std::regex::icase);
                        m_multi_download = std::regex_search(m_receive_header, pattern);
                    }
                    catch(std::exception e)
                    {
                        m_multi_download = false;
                        printf("[exception]%s\n", e.what());
                    }
                }
                else
                {
                    const char* err_string = curl_easy_strerror(curl_code);
                    m_error_string = err_string;
                }
                
                if (m_http_headers)
                {
                    curl_slist_free_all(reinterpret_cast<curl_slist*>(m_http_headers));
                    m_http_headers = nullptr;
                }
                
                curl_easy_cleanup(handle);
                return success;
            }
            
            return false;
        }
    }
    
    int SplitDownloadCount(double down_size)
    {
        const double size_2mb = 2.0 * 1024 * 1024;
        const double size_10mb = 10.0 * 1024 * 1024;
        const double size_50mb = 50.0 * 1024 * 1024;
        
        if (down_size <= size_2mb)
        {
            return 1;
        }
        else if (down_size > size_2mb && down_size <= size_10mb)
        {
            return static_cast<int>(down_size / (size_2mb));
        }
        else if (down_size > size_10mb && down_size <= size_50mb)
        {
            return HTTPRequest::s_kThreadCount + 1;
        }
        else
        {
            int down_count = static_cast<int>(down_size / size_10mb);
            return down_count > 10 ? 10 : down_count;
        }
        
        return 1;
    }
};

int HTTPImpl::DownloadThreadChunk::counter = 0;
////////////////////////////////////////////////////////////////////////
////
void         HTTPRequest::SetRetryTimes(int retry_times)
{
    if(!_impl) return;
    return _impl->SetRetryTimes(retry_times);
}
void         HTTPRequest::SetTimeout(long time_out)
{
    if(!_impl) return;
    return _impl->SetTimeout(time_out);
}
void         HTTPRequest::SetUrl(const std::string& url)
{
    if(!_impl) return;
    return _impl->SetUrl(url);
}
void         HTTPRequest::SetMovedUrl(bool get_moved_url)
{
    if(!_impl) return;
    return _impl->SetMovedUrl(get_moved_url);
}
void         HTTPRequest::SetUserData(void* userdata)
{
    if(!_impl) return;
    return _impl->SetUserData(userdata);
}
void*       HTTPRequest::GetUserData()
{
    if(!_impl) return nullptr;
    return _impl->GetUserData();
}
int         HTTPRequest::GetRequestId()
{
    if(!_impl) return 0;
    return _impl->GetRequestId();
}
void         HTTPRequest::SetProgressCallback(ProgressCallback pc)
{
    if(!_impl) return;
    return _impl->SetProgressCallback(pc);
}
void         HTTPRequest::SetResultCallback(ResultCallback rc)
{
    if(!_impl) return;
    return _impl->SetResultCallback(rc);
}
void         HTTPRequest::SetRequestHandle(IRequestHandle* handle)
{
    if(!_impl) return;
    return _impl->SetRequestHandle(handle);
}

void         HTTPRequest::SetPostMethod(bool post)
{
    if(!_impl) return;
    return _impl->SetPostMethod(post);
}
void         HTTPRequest::SetPostData(const std::string& message)
{
    return SetPostData(message.c_str(), message.size());
}
void         HTTPRequest::SetPostData(const void* data, size_t size)
{
    if(!_impl) return;
    return _impl->SetPostData(data, size);
}
void         HTTPRequest::SetRequestHeader(const std::map<std::string, std::string>& headers)
{
    for (auto it = headers.begin(); it != headers.end(); ++it)
    {
        std::string header = it->first;
        header += ": ";
        header += it->second;
        SetRequestHeader(header);
    }
}
void         HTTPRequest::SetRequestHeader(const std::string& header)
{
    if(!_impl) return;
    return _impl->SetRequestHeader(header);
}
void         HTTPRequest::SetRequestProxy(const std::string& proxy, long proxy_port)
{
    if(!_impl) return;
    return _impl->SetRequestProxy(proxy, proxy_port);
}

void         HTTPRequest::SetSSLVerify(bool enable)
{
    if(!_impl) return;
    return _impl->SetSSLVerify(enable);
}

void		 HTTPRequest::SetSSLCertificate(const std::string& filename)
{
	if (!_impl) return;
	return _impl->SetSSLCertificate(filename);
}

bool         HTTPRequest::SetDownloadMode(bool is_download)
{
    if(!_impl) return false;
    return _impl->SetDownloadMode(is_download);
}

void         HTTPRequest::SetDownloadPath(const std::string& path)
{
    if(!_impl) return;
    return _impl->SetDownloadPath(path);
}
void         HTTPRequest::SetDownloadThreadCount(size_t count)
{
    if(!_impl) return;
    return _impl->SetDownloadThreadCount((int)count);
}

void         HTTPRequest::SetDownloadRange(int from, int to)
{
    if(!_impl) return;
    return _impl->SetDownloadRange(from, to);
}

bool         HTTPRequest::GetHeader(std::string* header)
{
    if(!_impl) return false;
    return _impl->GetHeader(header);
}
bool         HTTPRequest::GetContent(std::string* content)
{
    if(!_impl) return false;
    return _impl->GetContent(content);
}
bool         HTTPRequest::GetErrorString(std::string* error_string)
{
    if(!_impl) return false;
    return _impl->GetErrorString(error_string);
}
long         HTTPRequest::GetHttpCode()
{
    if(!_impl) return 0;
    return _impl->GetHttpCode();
}

HTTPRequest::HTTPRequest(bool init)
:_impl(nullptr)
,_auto_destroy(true)
{
    if(init) Init();
}
HTTPRequest::~HTTPRequest()
{
	int id = _impl ? _impl->GetRequestId() : -1;
    delete _impl;
    _impl = nullptr;
	printf("[%d] ~HTTPRequest\n", id);
}

void         HTTPRequest::Init()
{
    if(_impl) return;
    int id = HTTPHelper::Instance()->GenID();
    H_HTTPHANDLE s_shared_handle = HTTPHelper::Instance()->GetSharedCurl();
    _impl = new HTTPImpl(id, this, s_shared_handle);
}

int          HTTPRequest::Run(bool download, bool async)
{
    assert(_impl);
    assert(!_impl->m_is_running);
    assert(!_impl->m_is_cancel);
    _impl->SetDownloadMode(download);
    return _impl->Run(async);
}

void         HTTPRequest::Stop()
{
    assert(_impl);
    assert(_impl->m_is_running);
    _impl->Stop();
}

int          HTTPRequest::Resume()
{
    assert(_impl);
    assert(!_impl->m_is_running);
    assert(_impl->m_is_cancel);
    return _impl->Resume();
}

bool         HTTPRequest::IsRuning()
{
    return _impl && _impl->m_is_running;
}
bool         HTTPRequest::IsCancle()
{
    return _impl && _impl->m_is_cancel;
}

void         HTTPRequest::Tick(float dt)
{
    HTTPHelper::Instance()->Tick(dt);
}

HTTPRequest* HTTPRequest::Create(bool init)
{
    return new HTTPRequest(init);
}
void  HTTPRequest::Destroy(HTTPRequest*& request)
{
    int id = request->GetRequestId();
    delete request;
    request = nullptr;
}

namespace
{
	class MyRequestHandle : public IRequestHandle
	{
	public:
		~MyRequestHandle()
		{

		}
		void OnDelete(void* userdata)
		{
			HTTPRequest* pOwner = (HTTPRequest*)userdata;
			printf("[%d] Need Destroy MyRequestHandle\n", pOwner->GetRequestId());
		}
		void OnRequestFinished(bool success, const std::string& content, void* userdata)
		{
			HTTPRequest* pOwner = (HTTPRequest*)userdata;
			if (!success)
			{
				std::cerr <<"[" << pOwner->GetRequestId() << "] download error:" << content.c_str() << std::endl;
			}
			else
			{
				std::cout << "[" << pOwner->GetRequestId() << "] Download Finished!" << std::endl;
			}
		}
		int  OnProgress(double total_size, double handle_size, void* userdata)
		{
			HTTPRequest* pOwner = (HTTPRequest*)userdata;
			long tmp = static_cast<long>(handle_size / total_size * 100);
			printf("\r[%d] download progress:%ld%%\n", pOwner->GetRequestId(), tmp);

			return 0;
		}
	};

	static void On_HTTP_Request_Callback(bool success, const std::string& data, void* userdata)
	{
		if (success)
		{
			std::ofstream outfile;
			outfile.open("baidu.html", std::ios_base::binary | std::ios_base::trunc);
			if (outfile.good()) outfile.write(data.c_str(), data.size());
		}
		else
			printf("error:%s\n", data.c_str());
	}

	void TestHTTPRequest()
	{
		auto v = HTTPRequest::Create(true);
		HTTPRequest& http = *v;
		http.SetUrl("http://www.libyyu.com/lidengfeng");
		http.SetSSLVerify(true);
		http.SetTimeout(5000);
		http.SetResultCallback(On_HTTP_Request_Callback);
		http.SetRequestHeader("User-Agent:Mozilla/4.04[en](Win95;I;Nav)");
		http.Run(false, true);
	}
	void TestHTTPRequestDownload()
	{
		auto v = HTTPRequest::Create(true);
		HTTPRequest& http = *v;
		static MyRequestHandle dc;
		//http.SetUrl("https://curl.haxx.se/download/curl-7.61.1.tar.gz");
		http.SetUrl("http://www.libyyu.com/static/sound/happy-birthday.mp3");
		http.SetSSLVerify(false);
		http.SetTimeout(5000);
		//http.SetDownloadRange(0, 3986003);
		const char* down_file = "./cur3.tar.gz.mp3";
		http.SetRequestHandle(&dc);
		http.SetDownloadPath(down_file);
		http.Run(true, true);
	}
	void TestHTTPRequestDownload2()
	{
		auto v = HTTPRequest::Create(true);
		HTTPRequest& http = *v;
		static MyRequestHandle dc;
		http.SetUrl("http://download.videolan.org/pub/videolan/contrib/i686-w64-mingw32/vlc-contrib-i686-w64-mingw32-latest.tar.bz2");
		http.SetSSLVerify(false);
		http.SetTimeout(5000);
		//http.SetDownloadRange(0, 3986003);
		const char* down_file = "./vlc-contrib-i686-w64-mingw32-latest.tar.bz2";
		http.SetRequestHandle(&dc);
		http.SetDownloadPath(down_file);
		http.Run(true, true);
	}
}

#ifdef   __cplusplus
extern "C" {
#endif

	HTTP_API void test_http()
	{
		TestHTTPRequest();
		TestHTTPRequestDownload();
		TestHTTPRequestDownload2();
		HTTPRequest::Tick(0);
		printf("exit \n");
	}

#ifdef   __cplusplus
}
#endif