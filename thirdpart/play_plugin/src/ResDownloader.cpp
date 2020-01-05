#ifdef _WIN32
#	include <winsock2.h>
#	include <windows.h>
#endif
#include "ResDownloader.h"
#include <algorithm>
#include <cassert>
#include "flib.h"
#include "base/url.h"

static void BulidFileHttpHeader(const char* psUrl,size_t nPos,size_t nLen,std::string& strOutHeader)
{
	assert(psUrl);
	std::string s_url(psUrl);
	std::replace(s_url.begin(),s_url.end(),'\\','/');

	int n_start_pos = s_url.find("//")+2;
	int n_end_pos   = s_url.find('/',n_start_pos);
	std::string s_host_name = s_url.substr(n_start_pos,n_end_pos -n_start_pos);
	std::string s_url_path = s_url.substr(n_end_pos);

	char szRange[64] = {0};
	if(0 == nLen)
		sprintf_s(szRange,"Range:bytes=%d-\r\n",nPos);
	else
		sprintf_s(szRange,"Range:bytes=%d-%d\r\n",nPos,nPos+nLen-1);

	strOutHeader.clear();
	strOutHeader += "GET ";
	strOutHeader += url::urlencode(s_url_path);   //"http://f1.xiami.net/112373/524201/21_1771121775_3431424.mp3";
	strOutHeader += " HTTP/1.1\r\n"
					"Accept: */*\r\n"
					"Accept-Language: zh-cn\r\n"
					"Accept-Encoding: gzip, deflate\r\n"
					"Accept-Charset: utf8\r\n"
					"User-Agent:XSpeeded 1.0.0.1 (Windows)\r\n";


	strOutHeader += szRange;

	strOutHeader += "Host: ";
	strOutHeader += s_host_name;
	strOutHeader += "\r\n"
					"Connection: keep-alive\r\n"
					"Pragma: no-cache\r\n"
					"Cache-Control: no-cache\r\n\r\n";
}
#ifdef _WIN32
unsigned long __stdcall CResDownloader::OnDownThread(void* p_void)
#else
void* CResDownloader::OnDownThread(void* p_void)
#endif
{
	CResDownloader* pDownloader = static_cast<CResDownloader*>(p_void);
	assert(pDownloader);
	if(pDownloader)
		pDownloader->OnDownPro();
	return 0;
}

CResDownloader::CResDownloader(const char* psUrl,CResDownloaderDelegate* delegate)
	:_sUrl(psUrl)
	,_delegate(delegate)
	,_isRunning(false)
#ifdef _WIN32
	,_hThread(NULL)
#endif
{
	assert(psUrl);
	assert(delegate);
}

CResDownloader::~CResDownloader()
{
	stop();
}

void CResDownloader::start()
{
	assert(isStopped());
	if(isStopped())
	{
		_isRunning = true;
#ifdef _WIN32
		_hThread = ::CreateThread(NULL,0,&CResDownloader::OnDownThread,this,0,NULL);
		assert(_hThread);
		if (!_hThread)
		{
			_isRunning = false;
		}
#else
		int ret = pthread_create(&_thread, NULL, OnDownThread, (void*)this);
		assert(0 == ret);
		if (0 != ret)
		{
			_isRunning = false;
		}
#endif
	}
}
void CResDownloader::stop()
{
	if(!isStopped())
	{
		_isRunning = false;
#ifdef _WIN32
		WaitForSingleObject(_hThread,INFINITE);
		CloseHandle(_hThread);
		_hThread = NULL;
#else
		pthread_join(_thread, NULL);
#endif
	}
}

bool CResDownloader::isStopped() const
{
#ifdef _WIN32
	return (NULL == _hThread) && (!_isRunning);
#else
	return (!_isRunning);
#endif
		
}

bool CResDownloader::isRunning() const
{
	return _isRunning;
}

void CResDownloader::OnDownPro() const
{
	F_SOCKET_STARTUP

	std::string surl(_sUrl);
	std::replace(surl.begin(),surl.end(),'\\','/');

	int bpos = surl.find("//")+2;
	int epos = surl.find('/',bpos);
	std::string hostname =surl.substr(bpos,epos - bpos);

	_FStd(FSockAddr) addr(SERVER_PORT,hostname.c_str());

	_FStd(FBuffer) buffer;
	size_t  totalRecvBytes = 0;
	int     nFileLen       = -1;
	bool    isFinished     = false;
	int     maxErrCnt      = MAX_ERROR_CNT;
	_FStd(FSocket)::ExitWaitPred pred = [this]()->bool { return !this->isRunning(); };

__ReDown:
	while(this->_isRunning && maxErrCnt-- && !isFinished)
	{
		_FStd(FSocket) con;
		if(!con.Create(SOCK_STREAM) || 
			!con.ConnectEx(&addr,TIMEOUT,pred))
			continue;
		
		if(!con.WaitWriteable(TIMEOUT,pred))
			continue;

		{
			std::string strRequestHeader;
			BulidFileHttpHeader(_sUrl.c_str(),totalRecvBytes,0,strRequestHeader);
			char * buf       = (char*)strRequestHeader.c_str();
			const size_t cnt = strRequestHeader.length();
			if(cnt != con.Send(buf,cnt))
				continue;
		}
	
		buffer.clear();
		bool isParseHeader = false;
		while(_isRunning && !isFinished)
		{
			if(!con.WaitReadable(TIMEOUT,pred))
				break;

			char*  recvBuf     = (char*)buffer[buffer.wpos()];
			size_t recvBufSize = buffer.nextwriteblocksize();
			assert(recvBuf);
			assert(recvBufSize);
			if(!recvBuf || !recvBufSize)
				break;

			int nRecvBytes = con.Recv(recvBuf,recvBufSize);
			if(0 >= nRecvBytes)
				break;
			if(nRecvBytes > 0)
			{
				buffer.wpos(buffer.wpos() + nRecvBytes);

				if(!isParseHeader)
				{
					static const char HTTP_END_FLAG[]      = "\r\n\r\n";
					static const char CONTENT_LENGTH[]     = "Content-Length: ";
					static const char HTTP_OK_FLAG[]       = "200 OK";
					static const char HTTP_RANGE_OK_FLAG[] = "206 Partial Content";
					const int pos = buffer.find((uint8*)HTTP_END_FLAG,strlen(HTTP_END_FLAG));
					if(-1 == pos)
						continue;

					if(-1 == buffer.find((uint8*)HTTP_OK_FLAG,strlen(HTTP_OK_FLAG)) &&
					   -1 == buffer.find((uint8*)HTTP_RANGE_OK_FLAG,strlen(HTTP_RANGE_OK_FLAG)))
						break;

					if(-1 == nFileLen)
					{
						const int i = buffer.find((uint8*)CONTENT_LENGTH,strlen(CONTENT_LENGTH));
						if(-1 != i)
						{
							*((buffer)[pos]) = '\0';
							sscanf_s((char*)(buffer)[i+strlen(CONTENT_LENGTH)],"%d\r\n",&nFileLen);
						}

						assert(0 != nFileLen);
						if(_delegate)
							_delegate->OnRecvHeader(const_cast<CResDownloader*>(this),nFileLen);
					}

					isParseHeader = true;
					buffer.rpos(pos + strlen(HTTP_END_FLAG));
				}

				if(!buffer.empty() && isParseHeader)
				{
					totalRecvBytes += buffer.cnt();
					if(_delegate)
						_delegate->OnRecvData(const_cast<CResDownloader*>(this),(const char*)buffer[buffer.rpos()],buffer.cnt(),nFileLen);
					buffer.clear();
				}

				if(nFileLen != -1)
				{
					assert(totalRecvBytes <= nFileLen);
					isFinished = (totalRecvBytes >= nFileLen);
				}
			}
		}
	}


	if(_delegate)
	{
		if(isFinished)
			_delegate->OnFinished(const_cast<CResDownloader*>(this));
		else if(!_isRunning)
			_delegate->OnCancled(const_cast<CResDownloader*>(this));
		else
		{
			bool isReDown = !_delegate->OnFailed(const_cast<CResDownloader*>(this));
			if(isReDown)
			{
				maxErrCnt = MAX_ERROR_CNT;
				goto __ReDown;
			}
		}
	}
}