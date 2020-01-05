#ifndef __BaseWindow_h
#define __BaseWindow_h

#pragma once
#include "UIlib.h"
#include "skin_change_event.hpp"

/************************************************************************/
/*                                                                      */
/************************************************************************/
class CBaseWindow 
	: public DuiLib::CWin
	, public DuiLib::IDialogBuilderCallback
{
	
public:
	
	CBaseWindow();
	~CBaseWindow();
	
public:
	virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);
protected:

protected:
	LPCTSTR GetWindowClassName() const;
	virtual UINT GetClassStyle() const;
protected:
	virtual LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnWebService(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void OnWebService(void *pRequest ,void *pProto);
};

typedef   CBaseWindow		   YWindow;

#endif//__BaseWindow_h