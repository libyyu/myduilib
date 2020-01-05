#pragma once
#include "WebServiceCatch.h"
#include <functional>
#include <vector>

namespace url
{
	typedef std::string UTF8STR;
	typedef UTF8STR     URLSTR;

    enum emRequestT
	{
		WSRT_NULL,
		//登录
		WSRT_LOGIN,
		WSRT_WEBLOGIN,	
		WRST_TEST,
	};


	std::string WEBSERVICE_API FormatString(const char* format, ...);

    class WEBSERVICE_API CRequest
    {
    public:
      typedef std::function<void (CRequest*,void*)> ResponderT;
      CRequest(emRequestT requestType,const URLSTR& host, const URLSTR& paths,const URLSTR& params);
      CRequest(emRequestT requestType,const URLSTR& host, const URLSTR& paths,const URLSTR& params,ResponderT responder);
      ~CRequest();
        
      emRequestT GetRequestType() const;
	  URLSTR GetHost() const;
      URLSTR GetPaths() const;
	  URLSTR GetParams() const;   

      size_t GetTag() const;
      void SetTag(size_t dwTag);

	  void* GetUserData() const;
	  void SetUserData(void* ud);

      CatchStrategyType GetCatchStrategyType() const;
	  void SetCatchStrategyType(CatchStrategyType nType);

      bool IsCatched() const;
	  void SetCatched(bool bCatch);

      bool IsTimeOut() const;
      void SetTimeOut(bool b);

      bool HasReponder() const;
      void SetReponder(ResponderT responder);
	  void Response(void* pdata);
    protected:
        ResponderT              _responder;
		int                     _tCreate;
		URLSTR                  _sHost;
		URLSTR                  _sPaths;
		URLSTR                  _sParams; 
		emRequestT              _requestType;
        bool                    _bIsFromCatch;
		CatchStrategyType       _nStrategyType;
        bool                    _isTimeOut;
        size_t                  _dwTag;
		void*                   _ud;
    };
	
	class Requester
	{
	private:
		const static std::string              _sAPI_URL;
		const static std::string              _sAPI_KEY;
		const static std::string              _sAPI_SECRET;
	public:
		Requester();
		~Requester();

		static std::string GenSigUrl( const std::string& sMethod, const std::vector<std::pair<std::string,std::string> >& v);
		static std::string GenSigUrlEX( const std::string& sMethod, const std::vector<std::pair<std::string,std::string> >& v);
		//登录
		static CRequest* MakeLogin(const UTF8STR& sUserName,const UTF8STR& sPasswordMd5);
		static CRequest* MakeLogin(const UTF8STR& sToken);
		static CRequest* MakeTest();
	};
}
