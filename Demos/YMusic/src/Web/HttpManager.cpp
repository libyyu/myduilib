#include "stdafx.h"
#include "HttpManager.h"
#include "YApp.h"
#include "down_http_file.h"
#include "Marco.hpp"
HttpManagerProto::HttpManagerProto()
{
//	CurlGlobalInit();
}

HttpManagerProto::~HttpManagerProto()
{
//	CurlGlobalCleanup();
}

// call before you use this class.
bool HttpManagerProto::CurlGlobalInit()
{
	CURLcode res = ::curl_global_init( CURL_GLOBAL_ALL );
	if( CURLE_OK != res ) 
	{
		//log here
		sLogError(_T("HttpManagerProto::CurlGlobalInit"));
		return false;
	}

	curl_version_info_data* p_version = curl_version_info(CURLVERSION_NOW);
	if (p_version)
	{
#ifdef _CONSOLE
		fprintf(stderr,"\n[libcurl version:%s]\n[host:%s]|\n ",p_version->version,
			p_version->host);
#endif
	}
	return true;
}

void HttpManagerProto::CurlGlobalCleanup()
{
	::curl_global_cleanup();
}

CURL* HttpManagerProto::CurlInit()
{
	CURL* curl = ::curl_easy_init();
	if(NULL == curl)
	{
		//log here
		sLogError(_T("HttpManagerProto::CurlInit"));
	}
	return curl;
}

int HttpManagerProto::DoHttpPost(CurlData* pdata)
{
	assert(pdata);
	CURL* curl = CurlInit();
	if( NULL == curl )
		return em_utility::down_http_file::dr_exit;
	//��������Ҫ��Ҫ����ͷ��, �Լ����Ű�.
	::curl_easy_setopt( curl, CURLOPT_HEADER, false );
	::curl_easy_setopt( curl, CURLOPT_URL, pdata->url );
	::curl_easy_setopt( curl, CURLOPT_POST, 1 );
	::curl_easy_setopt( curl, CURLOPT_POSTFIELDS, pdata->data );
	::curl_easy_setopt( curl, CURLOPT_TIMEOUT, 10 );
	::curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, HttpManagerProto::WriteFunction); 
	::curl_easy_setopt( curl, CURLOPT_WRITEDATA, pdata); 
	::curl_easy_setopt( curl, CURLOPT_NOPROGRESS, !(pdata->bshowprogress));//�Ƿ������ȼ��
	::curl_easy_setopt( curl, CURLOPT_PROGRESSFUNCTION, HttpManagerProto::ProgressFunction);
	::curl_easy_setopt( curl, CURLOPT_PROGRESSDATA,pdata);
	if (pdata->bwritefile)
	{
		pdata->fp = fopen(pdata->filename,"wb");
		assert(pdata->fp);
		if(pdata->fp == NULL)
			return em_utility::down_http_file::dr_open_file_failed;
	}
	CURLcode Code = ::curl_easy_perform( curl);
	if(CURLE_OK != Code)
	{
		sLogError(_T("DoHttpPost Failed: %s"), pdata->url);
		return em_utility::down_http_file::dr_open_url_failed;
	}
	long retcode = 0;
	int nReturn = em_utility::down_http_file::dr_read_file_failed;

	Code = ::curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
	if ( (Code == CURLE_OK) && retcode == 200 )
	{
		nReturn = em_utility::down_http_file::dr_success;
	}
	//д�뵽�ļ�
	if(pdata->bwritefile&&pdata->fp)
	{
		fseek(pdata->fp,SEEK_END,0);
		size_t return_size = fwrite(pdata->buffer, pdata->buffer_len, 1, pdata->fp); 	
	}
	::curl_easy_cleanup(curl);
	//��������ļ����ر��ļ�
	if(pdata->bwritefile&&pdata->fp)
		fclose(pdata->fp);

	return nReturn;
}

int HttpManagerProto::DoHttpGet(CurlData* pdata)
{
	assert(pdata);
	CURL* curl = CurlInit();
	if( NULL == curl )
		return em_utility::down_http_file::dr_exit;
	::curl_easy_setopt( curl, CURLOPT_URL, pdata->url );
	::curl_easy_setopt( curl, CURLOPT_HTTPGET, 1 );
	::curl_easy_setopt( curl, CURLOPT_HEADER, false );
	::curl_easy_setopt( curl, CURLOPT_TIMEOUT, 10 );
	::curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, HttpManagerProto::WriteFunction); 
	::curl_easy_setopt( curl, CURLOPT_WRITEDATA, pdata); 
	::curl_easy_setopt( curl, CURLOPT_NOPROGRESS, !(pdata->bshowprogress));//�Ƿ������ȼ��
	::curl_easy_setopt( curl, CURLOPT_PROGRESSFUNCTION, HttpManagerProto::ProgressFunction);
	::curl_easy_setopt( curl, CURLOPT_PROGRESSDATA,pdata);
	//�Ƿ�д�뵽�ļ�
	if (pdata->bwritefile)
	{
		pdata->fp = fopen(pdata->filename,"wb");
		assert(pdata->fp);
		if(pdata->fp==NULL)
			return em_utility::down_http_file::dr_open_file_failed;
	}
	CURLcode Code = ::curl_easy_perform( curl);
	if(CURLE_OK!=Code)
	{
		sLogError(_T("DoHttpGet Failed: %s"),pdata->url);
		return em_utility::down_http_file::dr_open_url_failed;
	}
	long retcode = 0;
	int nReturn = em_utility::down_http_file::dr_read_file_failed;

	Code = ::curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE , &retcode); 
	if ( (Code == CURLE_OK) && retcode == 200 )
	{
		nReturn = em_utility::down_http_file::dr_success;
	}
	//д�뵽�ļ�
	if(pdata->bwritefile&&pdata->fp)
	{
		fseek(pdata->fp,SEEK_END,0);
		size_t return_size = fwrite(pdata->buffer, pdata->buffer_len, 1, pdata->fp); 	
	}
	::curl_easy_cleanup(curl);
	//��������ļ����ر��ļ�
	if(pdata->bwritefile&&pdata->fp)
		fclose(pdata->fp);

	return nReturn;
}

int HttpManagerProto::DoHttpHeader(CurlData* pdata)
{
	assert(pdata);
	CURL* curl = CurlInit();
	::curl_easy_setopt( curl, CURLOPT_URL, pdata->url );
	::curl_easy_setopt( curl, CURLOPT_NOBODY, true );
	::curl_easy_setopt( curl, CURLOPT_HEADER, true );      //�������ݰ���HTTPͷ��
//	::curl_easy_setopt( curl, CURLOPT_RETURNTRANSFER,)
	::curl_easy_setopt( curl, CURLOPT_AUTOREFERER, true );
	::curl_easy_setopt( curl, CURLOPT_TIMEOUT, 10 );       //���ó�ʱ
//	::curl_easy_setopt( curl, CURLOPT_NOSIGNAL, 1);        //���������ź�
//	::curl_easy_setopt( curl, CURLOPT_RANGE, "0-500");     //���ڶϵ�����, �������صķ�Ƭ
// 	CURLOPT_NOPROGRESS  
// 		Ϊ��ʹCURLOPT_PROGRESSFUNCTION������. CURLOPT_NOPROGRESS���뱻����Ϊfalse.
// 		CURLOPT_PROGRESSFUNCTION
// 		CURLOPT_PROGRESSFUNCTION ָ���ĺ������������ÿ�뱻libcurl����һ��.
// 		CURLOPT_PROGRESSDATA
// 		CURLOPT_PROGRESSDATAָ���Ĳ�������ΪCURLOPT_PROGRESSFUNCTIONָ�������Ĳ���.

	return 0;
}

size_t HttpManagerProto::WriteFunction( void* ptr, size_t size, size_t nmemb, void *usrptr )
{
	
	CurlData* pdata = static_cast<CurlData*>(usrptr);
	
	char *newbuff;
	size_t rembuff;

	size *= nmemb;

	rembuff = pdata->buffer_len - pdata->buffer_pos; /* remaining space in buffer */

	if(size > rembuff) 
	{
		/* not enough space in buffer */
		newbuff = static_cast<char*>(::realloc(pdata->buffer,pdata->buffer_len + (size - rembuff) + 1) );
		if(newbuff == NULL) 
		{
			//log here
			sLogError(_T("HttpManagerProto::WriteFunction relloc error"));
			size = rembuff;
		}
		else 
		{
			/* realloc suceeded increase buffer size*/
			pdata->buffer_len += size - rembuff;
			pdata->buffer = newbuff;
		}
	}

	memcpy(&pdata->buffer[pdata->buffer_pos], ptr, size);
	pdata->buffer_pos += size;
	pdata->buffer[pdata->buffer_pos] = '\0';
	
	return size;
}

//---------------------------------------------------------------------------------------------------------------
// dltotal : 
// dlnow : 
// ultotal : 
int HttpManagerProto::ProgressFunction(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	CurlData* pdata = static_cast<CurlData*>(clientp);
	if(!pdata)
		return 0;
	pdata->dltotal = dltotal;
	pdata->dlnow = dlnow;
	pdata->ultotal = ultotal;
	pdata->ulnow = ulnow;

	if(pdata->h_notify_wnd&&pdata->n_notify_msg>0)
		::PostMessage(pdata->h_notify_wnd,pdata->n_notify_msg,(WPARAM)pdata, WP_LOADING);

	return 0;
}