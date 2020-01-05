#include "stdafx.h"
#include "BaseWindow.h"
#include "Marco.hpp"
#include "MyControls/MyControls.h"
/*************************************************************************************/

CBaseWindow::CBaseWindow()
{

}

CBaseWindow::~CBaseWindow()
{
	
}

LPCTSTR CBaseWindow::GetWindowClassName() const
{
	return _T("YTingWin");
}
UINT CBaseWindow::GetClassStyle() const
{
	return CS_DBLCLKS/*|CS_DROPSHADOW*/;//|CS_VREDRAW|CS_HREDRAW|CS_SAVEBITS*/;
}

DuiLib::CControlUI* CBaseWindow::CreateControl(LPCTSTR pstrClass)
{
	if (_tcscmp(pstrClass, DUI_CTR_TEMPLLIST) == 0)
	{
		DuiLib::CTemplListUI* pList = new DuiLib::CTemplListUI();
		pList->SetCreateBuilderCallback(this);
		return pList;
	}
	return MyCreateControl(pstrClass);
}

LRESULT CBaseWindow::ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if(WM_WEB_SERVICE==uMsg)
		return OnWebService(uMsg,wParam,lParam,bHandled);

	return __super::ProcessWindowMessage(uMsg,wParam,lParam,bHandled);
}

LRESULT CBaseWindow::OnWebService(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	void *pRequest = (void *)wParam;
	void *proto = (void *)lParam;

	ASSERT(pRequest);
//	ASSERT(proto);
	this->OnWebService(pRequest,proto);

// 	if(proto)
// 	{
// 		delete proto;
// 	}
	if(pRequest)
	{
		delete pRequest;
	}


	bHandled = true;
	return 0;
}

void CBaseWindow::OnWebService(void *pRequest ,void *pProto)
{

}

