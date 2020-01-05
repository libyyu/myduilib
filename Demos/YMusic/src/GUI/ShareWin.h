#ifndef __ShareWin_h
#define __ShareWin_h

#include "BaseWindow.h"
#include <WebService/UrlService.h>
#include "Marco.hpp"
#include "MyControls/MyControls.h"
#include "Mgr/DB/ShareData.h"

class CShareWin :
	public YWindow,
	public DuiLib::INotifyUI,
	public SkinChangedReceiver,
	public DuiLib::Singleton<CShareWin>
{
public:

	CShareWin();
	~CShareWin();
	static LPCTSTR GetWindowClass();
	LPCTSTR GetWindowClassName() const;
	void Share(spSongInfoT song);
	void Share(emShareObjT type,std::size_t id,LPCTSTR title,LPCTSTR content);
protected:
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void Notify(DuiLib::TNotifyUI& msg);

	void OnClick(const DuiLib::TNotifyUI& msg);

protected:
	virtual BOOL Receive(SkinChangedParam param);
	virtual LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnWebService(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void    OnWebService(void *pRequest ,void *pProto);
	virtual LRESULT ProcessWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
 //   url::emShareObjT    _type;
	std::size_t  _id;
	UINT_PTR     _nTimerId;

	struct {
		DuiLib::CLabelUI*        _title;
		DuiLib::CLabelUI*        _textlen;
		DuiLib::CFadeButtonUI*   _sharebtn;
		DuiLib::CRichEditUI*     _edit;
	}ui;
private:
	DECLARE_SINGLETON_CLASS(CShareWin);
};


#endif//__ShareWin_h