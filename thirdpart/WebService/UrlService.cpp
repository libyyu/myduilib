#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <map>
#include "UrlService.h"
#include "md5_checksum.h"
#include <algorithm>
#include "base/url.h"

namespace url 
{
	CRequest::CRequest(emRequestT requestType,const URLSTR& host, const URLSTR& paths,const URLSTR& params)
		:_requestType(requestType),
		_sHost(host),
		_sPaths(paths),
		_sParams(params),
		_tCreate(clock()),
		_bIsFromCatch(false),
		_nStrategyType(CST_NOCATCH),
		_isTimeOut(false),
		_dwTag(0),
		_ud(0)
	{

	}
	CRequest::CRequest(emRequestT requestType,const URLSTR& host, const URLSTR& paths,const URLSTR& params,ResponderT responder)
		:_requestType(requestType),
		_sHost(host),
		_sPaths(paths),
		_sParams(params),
		_responder(responder),
		_tCreate(clock()),
		_bIsFromCatch(false),
		_nStrategyType(CST_NOCATCH),
		_isTimeOut(false),
		_dwTag(0),
		_ud(0)
	{

	}
	CRequest::~CRequest()
	{

	}

	emRequestT CRequest::GetRequestType() const
	{
		return _requestType;
	}
	URLSTR CRequest::GetHost() const
	{
		return _sHost;
	}
	URLSTR CRequest::GetPaths() const
	{
		return _sPaths;
	}
	URLSTR CRequest::GetParams() const
	{
		return _sParams;
	}

	void* CRequest::GetUserData() const
	{
		return _ud;
	}
	void CRequest::SetUserData(void* ud)
	{
		_ud = ud;
	}
	size_t CRequest::GetTag() const
	{
		return _dwTag;
	}
	void CRequest::SetTag(size_t dwTag)
	{
		_dwTag = dwTag;
	}

	CatchStrategyType CRequest::GetCatchStrategyType() const
	{
		return _nStrategyType;
	}
	void CRequest::SetCatchStrategyType(CatchStrategyType nType)
	{
		_nStrategyType = nType;
	}

	bool CRequest::IsCatched() const
	{
		return _bIsFromCatch;
	}
	void CRequest::SetCatched(bool bCatch)
	{
		_bIsFromCatch = bCatch;
	}

	bool CRequest::IsTimeOut() const
	{
		return _isTimeOut;
	}
	void CRequest::SetTimeOut(bool b)
	{
		_isTimeOut = b;
	}

	bool CRequest::HasReponder() const
	{
		return !!_responder;
	}
	void CRequest::SetReponder(ResponderT responder)
	{
		_responder = responder;
	}
	void CRequest::Response(void* pdata)
	{
		assert(pdata);
		assert(HasReponder());
		if(_responder)
			_responder(this,pdata);
	}

	static CRequest* MakeRequest(emRequestT reType, const char* szHost, const char* szPaths, const char* szParams)
	{
		assert(szHost);
		URLSTR sPaths, sParams;
		if (szPaths) sPaths += szPaths;
		if (szParams) sParams += szParams;
		return (new CRequest(reType, URLSTR(szHost), sPaths, url::urlencode(sParams)));
	}
	static CRequest* MakeRequest(emRequestT reType, const URLSTR& szHost, const URLSTR& szPaths, const URLSTR& szParams)
	{
		return MakeRequest(reType, szHost.c_str(), szPaths.c_str(), szParams.c_str());
	}
	static CRequest* MakeRequestWithTag(emRequestT reType,const char* szHost,const char* szPaths,const char* szParams,size_t dwTag)
	{
		CRequest* rq = MakeRequest(reType,szHost,szPaths,szParams);
		rq->SetTag(dwTag);
		return rq;
	}
	static CRequest* MakeRequestWithParamsTag(emRequestT reType,const char* szHost,const char* szParams, size_t dwTag)
	{
		return MakeRequestWithTag(reType,szHost,"/api",szParams,dwTag);
	}
	static CRequest* MakeRequestWithParams(emRequestT reType,const char* szHost,const char* szParams)
	{
		return MakeRequestWithParamsTag(reType,szHost,szParams,0); 
	}

	std::string FormatString(const char* format, ...)
	{
		va_list va;
		va_start(va, format);
		const int MAX_BUFFLEN = 2049;
		char buff[MAX_BUFFLEN + 1] = { 0 };
#ifdef _MSC_VER
		_vsnprintf_s(buff, MAX_BUFFLEN, format, va);
#else
		vsnprintf(buff, MAX_BUFFLEN, format, va);
#endif
		va_end(va);
		return std::string(buff);
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/


	const std::string Requester::_sAPI_URL ="/api/";
	const std::string Requester::_sAPI_KEY ="bf2061ba3590d1c725bb25a0f6c100eb";
	const std::string Requester::_sAPI_SECRET ="d2b94a19c762c0b69d04cefe5b66f211";
	Requester::Requester()
	{

	}
	Requester::~Requester()
	{

	}

	std::string Requester::GenSigUrl(const std::string& sMethod, const std::vector<std::pair<std::string,std::string> >& v)
	{
		std::string sSigUrl;
		std::string sValue;
		std::string sMd5;

		assert(!sMethod.empty());
		assert(v.size());

		if(sMethod.empty())
			return sSigUrl;

		std::map<std::string,std::string> m;

		// 拼接url
		//sSigUrl = _sAPI_URL + "?";
		for (auto iter = v.begin(); iter != v.end(); ++iter)
		{
			sSigUrl.append(url::urlencode(iter->first));
			sSigUrl.append("=");
			sSigUrl.append(url::urlencode(iter->second));
			sSigUrl.append("&");

			m.insert(std::make_pair(iter->first,iter->second));
		}

		sSigUrl.append("api_key=");
		sSigUrl.append(_sAPI_KEY);
		sSigUrl.append("&method=");
		sSigUrl.append(sMethod);
		sSigUrl.append("&call_id=");

		time_t ltime;
		time(&ltime);
		char buf[50];
#ifdef _MSC_VER
		std::string sCallId = _itoa((int)ltime,buf,10);
#else
		sprintf(buf, "%d", (int)ltime);
		std::string sCallId = buf;
#endif
		sSigUrl.append(sCallId);
		sSigUrl.append("&api_sig=");

		//计算api_sig

		m.insert(std::make_pair("api_key",_sAPI_KEY));
		m.insert(std::make_pair("method",sMethod));
		m.insert(std::make_pair("call_id",sCallId));

		for(auto iter = m.begin(); iter != m.end(); ++iter)
		{
			sValue.append(iter->first);
			sValue.append(iter->second);
		}
		sValue.append(_sAPI_SECRET);
		sMd5 = em_utility::md5_checksum::get_md5((const unsigned char*)sValue.c_str(),(unsigned int)sValue.length());
		assert(!sMd5.empty());
		sSigUrl.append(sMd5);
		return sSigUrl;
	}

	std::string Requester::GenSigUrlEX( const std::string& sMethod, const std::vector<std::pair<std::string,std::string> >& v)
	{
		std::string sSigUrl;
		std::string sValue;
		std::string sMd5;

		assert(!sMethod.empty());
		assert(v.size());

		if(sMethod.empty())
			return sSigUrl;

		// 拼接url
		//sSigUrl = _sAPI_URL + "?";

		std::string sS,sAl,sAr;

		for (auto iter = v.begin(); iter != v.end(); ++iter)
		{
			std::string first  = iter->first;
			std::string second = iter->second;
			sSigUrl.append(url::urlencode_ex(first));
			sSigUrl.append("=");
			sSigUrl.append(url::urlencode_ex(second));
			sSigUrl.append("&");

			std::string::size_type n = first.find_first_of('[');
			if(n != std::string::npos)
			{
				std::string sType = first.substr(0,n);
				if (sType == "s")
				{
					sS.append(second);
					sS.append(",");
				} 
				else if(sType == "al")
				{
					sAl.append(second);
					sAl.append(",");
				}
				else if(sType == "ar")
				{
					sAr.append(second);
					sAr.append(",");
				}
			}
		}

		sS  = sS.substr(0, sS.length()-1);
		sAl = sAl.substr(0, sAl.length()-1);
		sAr = sAr.substr(0, sAr.length()-1);

		sSigUrl.append("api_key=");
		sSigUrl.append(_sAPI_KEY);
		sSigUrl.append("&method=");
		sSigUrl.append(sMethod);
		sSigUrl.append("&call_id=");

		time_t ltime;
		time(&ltime);
		char buf[50];
#ifdef _MSC_VER
		std::string sCallId = _itoa((int)ltime,buf,10);
#else
		sprintf(buf, "%d", (int)ltime);
		std::string sCallId = buf;
#endif
		sSigUrl.append(sCallId);
		sSigUrl.append("&api_sig=");

		//计算api_sig
		std::map<std::string,std::string> m;

		m.insert(std::make_pair("s",sS));
		m.insert(std::make_pair("al",sAl));
		m.insert(std::make_pair("ar",sAr));
		m.insert(std::make_pair("api_key",_sAPI_KEY));
		m.insert(std::make_pair("method",sMethod));
		m.insert(std::make_pair("call_id",sCallId));
		m.insert(std::make_pair("type","1"));

		for(auto iter = m.begin(); iter != m.end(); ++iter)
		{
			sValue.append(iter->first);
			sValue.append(iter->second);
		}
		sValue.append(_sAPI_SECRET);
		sMd5 = em_utility::md5_checksum::get_md5((const unsigned char*)sValue.c_str(),(unsigned int)sValue.length());
		assert(!sMd5.empty());
	
		sSigUrl.append(sMd5);
		return sSigUrl;
	}

	CRequest* Requester::MakeLogin(const UTF8STR& sUserName,const UTF8STR& sPasswordMd5)
	{
		/*std::string sFunc("Members.getByEmail");
		std::vector<std::pair<std::string,std::string> > v;

		v.push_back(std::pair<std::string,std::string>("email",sUserName));
		v.push_back(std::pair<std::string,std::string>("pwd",sPasswordMd5));
		v.push_back(std::pair<std::string,std::string>("type","1"));


		std::string strUrl = FormatString("%s", GenSigUrl(sFunc, v).c_str());
		return MakeRequestWithParams(WSRT_LOGIN, strUrl.c_str());*/
		return NULL;
	}

	CRequest* Requester::MakeLogin(const UTF8STR& sToken)
	{
		/*std::string sFunc("Members.auth");
		std::vector<std::pair<std::string,std::string> > v;

		v.push_back(std::pair<std::string,std::string>("t",sToken));
		std::string strUrl = FormatString("%s", GenSigUrl(sFunc, v).c_str());
		return MakeRequestWithParams(WSRT_WEBLOGIN, strUrl.c_str());
		*/
		return NULL;
	}

	CRequest* Requester::MakeTest()
	{
		return MakeRequest(WRST_TEST, "www.libyyu.com", "/lidengfeng", NULL);
	}
}