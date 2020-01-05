#ifndef WEBSERVICE_H
#define WEBSERVICE_H
#pragma once
#include "UrlService.h"
#include "network/NetWork.h"
#include "ResponseParser.h"

class WEBSERVICE_API CWebServiceMgr : public ILinker
{
public:
	typedef url::CRequest* LPRequest;
	typedef url::CRequest::ResponderT ResponderT;
	typedef std::vector<unsigned char> ByteArrayT;
	CWebServiceMgr();
	~CWebServiceMgr();
public:	
	void SetNotifyMsg(unsigned int msg){_nMsg = msg;}
	std::string BuildHttpHeader(LPRequest pRequest,bool bPost = false);
	void ClearRequest();
	void AddHandler(unsigned int RequestType, CResponseParser::HandlerFuncT pFunc);
public:
	void SendRequest(LPRequest pRequest);
	void SendRequest(void* hwnd,LPRequest pRequest,size_t dwTag);
	void SendRequest(void* hwnd,LPRequest pRequest);
	void SendRequest(LPRequest pRequest,ResponderT responder, size_t dwTag);
	void SendRequest(LPRequest pRequest,ResponderT responder);
	void SendRequest(LPRequest pRequest, ByteArrayT& buffer);

	void AsynSendRequest(LPRequest pRequest);
	void AsynSendRequest(void* hwnd,LPRequest pRequest,size_t dwTag);
	void AsynSendRequest(void* hwnd,LPRequest pRequest);
	void AsynSendRequest(LPRequest pRequest,ResponderT responder,size_t dwTag);
	void AsynSendRequest(LPRequest pRequest,ResponderT responder);
	
	void PostRequest(LPRequest pRequest);
	void PostRequest(void* hwnd,LPRequest pRequest,size_t dwTag);
	void PostRequest(void* hwnd,LPRequest pRequest);
	void PostRequest(LPRequest pRequest,ResponderT responder, size_t dwTag);
	void PostRequest(LPRequest pRequest,ResponderT responder);

	void AsynPostRequest(LPRequest pRequest);
	void AsynPostRequest(void* hwnd,LPRequest pRequest, size_t dwTag);
	void AsynPostRequest(void* hwnd,LPRequest pRequest);
	void AsynPostRequest(LPRequest pRequest,ResponderT responder, size_t dwTag);
	void AsynPostRequest(LPRequest pRequest,ResponderT responder);
private:
	void onAsynRequest(LPRequest pRequest,bool bPost = false);
	void OnRequest(LPRequest pRequest,bool bPost = false);
	void OnResponse(LPRequest pRequest,unsigned char* ppbuffer, size_t bytes); 
	void OnResponse(LPRequest pRequest,void* pResponse);
		
	bool GetRequestFromCache(const std::string& headerStr, ByteArrayT& buffer);
protected:
	virtual void onConnected(hLinker h);
	virtual void onDisconnected(hLinker h,const int ec);
	virtual void onSendCompleted(hLinker h,const void* pbuffer,const int ec,size_t bytes);
	virtual size_t onRecvCompleted(hLinker h,const void* pbuffer,size_t bytes);
	virtual void onDestroy(hLinker h);
private:
	void _SendRequest(LPRequest pRequest, void* buffer);//pbuffer is FBuffer
private:
	unsigned int			 _nMsg;
	CWebServiceCatch		 _webCache;
	CResponseParser          _parser;
};

#endif//WEBSERVICE_H
