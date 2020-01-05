#include "stdafx.h"
#include "jsHandler.h"
#include "YApp.h"
#include "YMusic.h"
#include "Web/UrlPraser.h"
#include "base64.h"

CJsHandler::CJsHandler(DuiLib::CWebBrowserUI* pWebUI):_refNum(1),_pWebUI(pWebUI)
{

}
CJsHandler::~CJsHandler()
{

}
void CJsHandler::RegisterJsCallBack(LPCTSTR lpszName,JsCallBackT f)
{
	ASSERT(lpszName);
	ASSERT(f);
	if(!lpszName || !f)
		return;

	_jsEventMap.push_back(JsEventMapT::value_type(lpszName,f));
}
void CJsHandler::UnRegisterJsCallBack(LPCTSTR lpszName)
{
	ASSERT(lpszName);
	if(!lpszName)
		return;

	for (JsEventMapT::iterator it = _jsEventMap.begin(),iend = _jsEventMap.end();it != iend;)
	{
		if(0 == it->first.Compare(lpszName))
			_jsEventMap.erase(it++);
		else
			++it;
	}
}

BOOL CJsHandler::CallJsFun( LPOLESTR pName, VARIANT *pVarResult, VARIANT *params /*= NULL*/, int cArgs/* = 0*/)
{
	ASSERT(_pWebUI);
	if (_pWebUI)
		return _pWebUI->CallJsFun(pName, pVarResult, params, cArgs);
	return FALSE;
}

// IUnknown Methods
STDMETHODIMP CJsHandler::QueryInterface(REFIID iid,void**ppvObject)
{
	*ppvObject = NULL;
	if (iid == IID_IUnknown)	*ppvObject = this;
	else if (iid == IID_IDispatch)	*ppvObject = (IDispatch*)this;
	if(*ppvObject)
	{
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CJsHandler::AddRef()
{
	return ::InterlockedIncrement(&_refNum);
}

STDMETHODIMP_(ULONG) CJsHandler::Release()
{
	::InterlockedDecrement(&_refNum);
	if(_refNum == 0)
	{
		delete this;
	}
	return _refNum;
}

// IDispatch Methods
HRESULT _stdcall CJsHandler::GetTypeInfoCount(
	unsigned int * pctinfo) 
{
	return E_NOTIMPL;
}

HRESULT _stdcall CJsHandler::GetTypeInfo(
	unsigned int iTInfo,
	LCID lcid,
	ITypeInfo FAR* FAR* ppTInfo) 
{
	return E_NOTIMPL;
}

HRESULT _stdcall CJsHandler::GetIDsOfNames(
	REFIID riid, 
	OLECHAR FAR* FAR* rgszNames, 
	unsigned int cNames, 
	LCID lcid, 
	DISPID FAR* rgDispId 
	)
{
	DuiLogInfo(_T("CJsHandler::GetIDsOfNames: %s"), (TCHAR*)(rgszNames[0]));
	ASSERT(rgszNames[0]);
	DuiLib::CDuiString crgszName;
#ifdef _UNICODE
	crgszName = rgszNames[0];
#else
	crgszName = rgszNames[0];
#endif
	size_t i = 0;
	for (JsEventMapT::iterator it = _jsEventMap.begin(),iend = _jsEventMap.end();it != iend;++it)
	{
		if(0 == it->first.Compare(crgszName))
		{
			*rgDispId = i;
			break;
		}

		++i;
	}
	return S_OK;
}

HRESULT _stdcall CJsHandler::Invoke(
	DISPID dispIdMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS* pDispParams,
	VARIANT* pVarResult,
	EXCEPINFO* pExcepInfo,
	unsigned int* puArgErr
	)
{
	if((JsEventMapT::size_type)dispIdMember < _jsEventMap.size())
	{
		_jsEventMap[dispIdMember].second(const_cast<CJsHandler*>(this),pVarResult,pDispParams);
		return S_OK;
	}

	return E_NOTIMPL;
}

//webBrow
HRESULT STDMETHODCALLTYPE CJsHandler::GetExternal(/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
{
	*ppDispatch = this;
	return S_OK;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
const TCHAR CSimpleJsHandler::WEB_TO_APP_JSFUNNAME[] = _T("InvokeApp");
const TCHAR CSimpleJsHandler::APP_TO_WEB_JSFUNNAME[] = _T("InvokeWeb");
CSimpleJsHandler::CSimpleJsHandler(DuiLib::CWebBrowserUI* pWebUI):_refNum(1),_pWebUI(pWebUI)
{

}
CSimpleJsHandler::~CSimpleJsHandler()
{

}
void CSimpleJsHandler::RegisterJsCallBack(LPCTSTR lpszName,JsCallBackT f)
{
	ASSERT(lpszName);
	ASSERT(f);
	if(!lpszName || !f)
		return;

	_jsEventMap.insert(JsEventMapT::value_type(lpszName,f));
}

void CSimpleJsHandler::UnRegisterJsCallBack(LPCTSTR lpszName)
{
	ASSERT(lpszName);
	if(!lpszName)
		return;

	_jsEventMap.erase(lpszName);
}

BOOL CSimpleJsHandler::CallJsFun(LPCTSTR lpszFunName,LPCTSTR sParams,VARIANT *pVarResult)
{
	ASSERT(_pWebUI);
 //	if(_pWebUI)
 //		return _pWebUI->CallJsFun((LPOLESTR)lpszFunName,pVarResult);
	return FALSE;
}
BOOL CSimpleJsHandler::InvokeWeb(LPCTSTR lpszFunName,LPCTSTR sParam)
{
	ASSERT(_pWebUI);
	if(_pWebUI)
	{
		VARIANT vars[2] = {0};
		vars[1].vt	    = VT_BSTR;
		vars[1].bstrVal =  SysAllocString((const OLECHAR*)lpszFunName);
		vars[0].vt	    = VT_BSTR;
		vars[0].bstrVal =  SysAllocString((const OLECHAR*)sParam);
		BOOL bRet = _pWebUI->CallJsFun((LPOLESTR)APP_TO_WEB_JSFUNNAME, NULL, &vars[0], 2);
		ASSERT(bRet);
		VariantClear(&vars[0]);
		VariantClear(&vars[1]);
		return bRet;
	}
	return FALSE;
}

// IUnknown Methods
STDMETHODIMP CSimpleJsHandler::QueryInterface(REFIID iid,void**ppvObject)
{
	*ppvObject = NULL;
	if (iid == IID_IUnknown)	*ppvObject = this;
	else if (iid == IID_IDispatch)	*ppvObject = (IDispatch*)this;
	if(*ppvObject)
	{
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

STDMETHODIMP_(ULONG) CSimpleJsHandler::AddRef()
{
	return ::InterlockedIncrement(&_refNum);
}

STDMETHODIMP_(ULONG) CSimpleJsHandler::Release()
{
	::InterlockedDecrement(&_refNum);
	if(_refNum == 0)
	{
		//delete this;
	}
	return _refNum;
}

// IDispatch Methods
HRESULT _stdcall CSimpleJsHandler::GetTypeInfoCount(
	unsigned int * pctinfo) 
{
	return E_NOTIMPL;
}

HRESULT _stdcall CSimpleJsHandler::GetTypeInfo(
	unsigned int iTInfo,
	LCID lcid,
	ITypeInfo FAR* FAR* ppTInfo) 
{
	return E_NOTIMPL;
}

HRESULT _stdcall CSimpleJsHandler::GetIDsOfNames(
	REFIID riid, 
	OLECHAR FAR* FAR* rgszNames, 
	unsigned int cNames, 
	LCID lcid, 
	DISPID FAR* rgDispId 
	)
{
	ASSERT(rgszNames[0]);
	DuiLib::CDuiString sTrace = _T("CSimpleJsHandler::GetIDsOfNames :(");
	for (size_t i=0;i<cNames-1;++i)
	{
		sTrace += rgszNames[i];
		sTrace += _T(",");
	}
	sTrace += rgszNames[cNames-1];
	sTrace += _T(")");
	sLog(sTrace.GetData());
	DuiLib::CDuiString crgszName;
#ifdef _UNICODE
	crgszName = rgszNames[0];
#else
	crgszName = rgszNames[0];
#endif
	*rgDispId = ((0 ==  _tcsicmp(crgszName,WEB_TO_APP_JSFUNNAME)) ? 1 : 0);
//	*rgDispId = ((0 ==  _tcsicmp(rgszNames[0],_T("callkwmusic"))) ? 1 : 0);
//	ASSERT(*rgDispId);
	return S_OK;
}
BSTR Variant2BSTR( _variant_t vString )
{
	vString.ChangeType(VT_BSTR);
	return vString.bstrVal;
}
HRESULT _stdcall CSimpleJsHandler::Invoke(
	DISPID dispIdMember,
	REFIID riid,
	LCID lcid,
	WORD wFlags,
	DISPPARAMS* pDispParams,
	VARIANT* pVarResult,
	EXCEPINFO* pExcepInfo,
	unsigned int* puArgErr
	)
{
	HRESULT hrReturn = E_NOTIMPL;
	if(1 == dispIdMember && pDispParams && pDispParams->cArgs>0/*&& (1== pDispParams->cArgs || 2 == pDispParams->cArgs)*/)
	{
		DuiLib::CDuiString firstParam;
		DuiLib::CDuiString secondParam;
// 		if(1 == pDispParams->cArgs)
// 			firstParam = pDispParams->rgvarg[0];
// 		else
// 		{
// 			firstParam = pDispParams->rgvarg[1];
// 			secondParam = pDispParams->rgvarg[0];
// 		}
		DuiLib::CDuiString sTrace = _T("CSimpleJsHandler::Invoke :(");
		for (int i=0;i<pDispParams->cArgs-1;++i)
		{
			sTrace += Variant2BSTR(pDispParams->rgvarg[i]);
			sTrace += _T(",");
		}
		sTrace += Variant2BSTR(pDispParams->rgvarg[pDispParams->cArgs-1]);
		sTrace += _T(")");
		sLog(sTrace.GetData());

		firstParam = Variant2BSTR(pDispParams->rgvarg[pDispParams->cArgs-1]);
		if(pDispParams->cArgs >= 2) secondParam = Variant2BSTR(pDispParams->rgvarg[0]);
		JsEventMapT::iterator it = _jsEventMap.find(firstParam);
//		ASSERT(it != _jsEventMap.end());
		if(_jsEventMap.end() != it && it->second)
		{
			DuiLib::CDuiString sResult;
			it->second(const_cast<CSimpleJsHandler*>(this),secondParam, sResult);
			if(!sResult.IsEmpty())
			{
				ASSERT(pVarResult);
				pVarResult->vt = VT_BSTR;
				pVarResult->bstrVal = SysAllocString((const OLECHAR * )sResult.GetData());
			}
			hrReturn = S_OK;
		}
	}

//	ASSERT(FALSE);
	return hrReturn;
}

//webBrow
HRESULT STDMETHODCALLTYPE CSimpleJsHandler::GetHostInfo( DOCHOSTUIINFO __RPC_FAR *pInfo)
{
	if (pInfo != NULL)
	{
		pInfo->dwFlags |= /*DOCHOSTUIFLAG_SCROLL_NO | DOCHOSTUIFLAG_FLAT_SCROLLBAR  | */ DOCHOSTUIFLAG_NO3DBORDER;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSimpleJsHandler::GetExternal(/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
{
	*ppDispatch = this;
	return S_OK;
}

void CSimpleJsHandler::BeforeNavigate2( IDispatch *pDisp,VARIANT *&url,VARIANT *&Flags,VARIANT *&TargetFrameName,VARIANT *&PostData,VARIANT *&Headers,VARIANT_BOOL *&Cancel )
{
	DuiLib::CDuiString surl; surl += Variant2BSTR(*url);
	sLog(_T("BeforeNavigate2:%s"), surl.GetData());
	if(surl.Left(7) == _T("http://"))
	{
		spLogonUserInfoT theUser = theRuntimeState->GetCurrentContext()->GetUser();
		if(theUser && !Headers->bstrVal)
		{
			IWebBrowser2* pWebBro = _pWebUI->GetWebBrowser2();
			this->NavigateUrl(pWebBro,surl);
			*Cancel = VARIANT_TRUE;
			return;
		}
	}
	
// 	IHTMLElement* pEle = NULL;_pWebUI->GetHTMLDocument2()->get_body(&pEle);
// 	CComQIPtr<IHTMLElement> spElem(pEle);
// 	VARIANT var;
// 	spElem->get_onclick(&var);
// //	CComQIPtr<IHTMLEventObj> spEvent(var.pdispVal);
// 	CComQIPtr<IDispatch> spDisp(pDisp);
// 	DISPID dispId = 0;
// 	OLECHAR *szName = L"onclick";
// 	spDisp->GetIDsOfNames(IID_NULL, &szName, 1, LOCALE_SYSTEM_DEFAULT, &dispId);
// 	DISPPARAMS params;
// 	memset(&params, 0, sizeof(params));
// 	VARIANT varRet;
// 	spDisp->Invoke(dispId, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &params, &varRet, NULL, NULL);
	*Cancel = VARIANT_FALSE;
	//*Cancel = ( (_delegate.OnWillNavigate(this,surl)) ?  VARIANT_FALSE : VARIANT_TRUE);
}

void CSimpleJsHandler::NavigateUrl(IWebBrowser2* pWebBro,LPCTSTR url)
{
	ASSERT(pWebBro);
	ASSERT(url);
	if(!pWebBro || !url)
		return;

	std::string requestHeader;
	spLogonUserInfoT theUser = theRuntimeState->GetCurrentContext()->GetUser();
	if(theUser)
	{
		DuiLib::CDuiString str;
		str.Format(_T("%s:%s"),theUser->sLogonEmail.GetData(),theUser->sPasswordMD5.GetData());
		std::string ansiStr = TransToStdString(str);
		if(!ansiStr.empty())
		{
			std::string base64Str = ZBase64::Encode((const unsigned char*)ansiStr.c_str(),ansiStr.size());
			if(!base64Str.empty())
			{
				requestHeader += "Authorization:Basic ";
				requestHeader += base64Str;
				requestHeader += "\r\n";
			}
		}
	}

	DuiLib::CDuiString headerStr = TransToDuiString(requestHeader.c_str());

	VARIANT vHeaders;
	VARIANT vNull;

	VariantInit(&vHeaders);
	VariantInit(&vNull);

	vHeaders.vt      = VT_BSTR;
	vHeaders.bstrVal = SysAllocString((const OLECHAR*)headerStr.GetData());

	_variant_t varStr(url);
	pWebBro->Navigate2(&varStr,&vNull,&vNull,&vNull,&vHeaders);
	VariantClear(&vHeaders);
	VariantClear(&vNull);
}
