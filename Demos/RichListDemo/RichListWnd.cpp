#include "StdAfx.h"
#include "RichListWnd.h"


//////////////////////////////////////////////////////////////////////////
///


DUI_BEGIN_MESSAGE_MAP(CPage1, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_END_MESSAGE_MAP()

CPage1::CPage1()
{
	m_pPaintManager = NULL;
}

void CPage1::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CPage1::OnClick(TNotifyUI& msg)
{
	if(msg.pSender->GetName() == _T("down_ico"))
	{                
		CControlUI *find_ctrl =m_pPaintManager->FindSubControlByName(msg.pSender->GetParent()->GetParent(), _T("down_name"));

		if(find_ctrl)
		{
			MessageBox(NULL, 
				find_ctrl->GetText()+_T(" ��ʾδѡ�����еİ�ť�������������ð�ť�������ҵ�������listcontainer.."), 
				_T("DUILIB DEMO"), MB_OK);   
			((CLabelUI *)find_ctrl)->SetText(_T("�ɳ���̬���ú������..."));
		}
		}
	else if(msg.pSender->GetName() == _T("down_del"))
	{
		CListUI *down_list = 
			static_cast<CListUI*>(m_pPaintManager->FindControl(_T("down_list_tab")));
		if(!down_list)
			return;

		down_list->RemoveAt(down_list->GetCurSel());                   
	}
	else if(msg.pSender->GetName() == _T("down_new"))
	{
		CListUI *down_list = static_cast<CListUI*>(m_pPaintManager->FindControl(_T("down_list_tab")));
		if(!down_list)
			return;

		CListContainerElementUI *new_node = new CListContainerElementUI;
		new_node->SetAttributeList(_T("height=\"45\""));

		CHorizontalLayoutUI *new_h_lay = new CHorizontalLayoutUI;
		new_h_lay->SetAttributeList(_T("float=\"false\" ")\
			_T("childpadding=\"10\" inset=\"3,5,3,5\""));

		CButtonUI *new_btn_1 = new CButtonUI;
		new_btn_1->SetAttributeList(
			_T("name=\"down_ico\" float=\"false\" ")\
			_T("bordersize=\"0\" width=\"32\" maxheight=\"26\" ")\
			_T("bkimage=\"downlist_app.png\" ")\
			_T("normalimage=\"file='downlist_run.png' dest='20,14,32,26'\""));

		CVerticalLayoutUI *new_v_lay = new CVerticalLayoutUI;
		new_h_lay->Add(new_btn_1);
		new_h_lay->Add(new_v_lay);

		CLabelUI *new_label = new CLabelUI;
		new_label->SetAttributeList(_T("textcolor=\"#FFAAAAAA\" showhtml=\"true\""));
		new_label->SetText(_T("new added item.exe"));
		new_label->SetName(_T("down_name"));
		CProgressUI *new_progress = new CProgressUI;
		new_progress->SetMinValue(0);
		new_progress->SetMaxValue(100);
		new_progress->SetValue(1);
		new_progress->SetMaxWidth(200);
		new_progress->SetMaxHeight(7);
		new_progress->SetForeImage(_T("progress_fore.png"));
		new_progress->SetName(_T("down_progress"));
		new_v_lay->Add(new_label);
		new_v_lay->Add(new_progress);

		CLabelUI *new_label2 = new CLabelUI;
		CLabelUI *new_label3 = new CLabelUI;
		CVerticalLayoutUI *new_v_lay2 = new CVerticalLayoutUI;
		new_h_lay->Add(new_v_lay2);
		new_v_lay2->Add(new_label2);
		new_v_lay2->Add(new_label3);
		new_label2->SetAttributeList(
			_T("align=\"right\" text=\"\" textcolor=\"#FFAAAAAA\" showhtml=\"true\""));
		new_label3->SetAttributeList(
			_T("align=\"right\" text=\"0.00K/34.33M \" textcolor=\"#FFAAAAAA\" showhtml=\"true\""));

		new_node->Add(new_h_lay);
		down_list->Add(new_node);
	}
}

void CPage1::OnSelectChanged( TNotifyUI &msg )
{

}

void CPage1::OnItemClick( TNotifyUI &msg )
{

}

//////////////////////////////////////////////////////////////////////////
///

DUI_BEGIN_MESSAGE_MAP(CPage2, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_END_MESSAGE_MAP()


CPage2::CPage2()
{
	m_pPaintManager = NULL;
}

void CPage2::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CPage2::OnClick(TNotifyUI& msg)
{

}

void CPage2::OnSelectChanged( TNotifyUI &msg )
{

}

void CPage2::OnItemClick( TNotifyUI &msg )
{

}

//////////////////////////////////////////////////////////////////////////
///

DUI_BEGIN_MESSAGE_MAP(CRichListWnd, CNotifyPump)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED,OnSelectChanged)
	DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK,OnItemClick)
DUI_END_MESSAGE_MAP()

CRichListWnd::CRichListWnd(void)
{
	m_Page1.SetPaintMagager(GetPaintMgr());
	m_Page2.SetPaintMagager(GetPaintMgr());

	AddVirtualWnd(_T("page1"),&m_Page1);
	AddVirtualWnd(_T("page2"),&m_Page2);
}

CRichListWnd::~CRichListWnd(void)
{
	RemoveVirtualWnd(_T("page1"));
	RemoveVirtualWnd(_T("page2"));
}
/////////////////////////////////////////////////////////////////////////
////
void CRichListWnd::OnFinalMessage( HWND hWnd)
{

	GetPaintMgr()->RemovePreMessageFilter(this);
	GetPaintMgr()->RemoveNotifier(this);
	GetPaintMgr()->ReapObjects(GetPaintMgr()->GetRoot());

	__super::OnFinalMessage(hWnd);
	delete this;
}
LPCTSTR CRichListWnd::GetWindowClassName() const
{
	return _T("RichListWnd");
}
void CRichListWnd::OnClick( TNotifyUI &msg )
{
	if( msg.pSender == m_pCloseBtn ) 
	{ 
		PostQuitMessage(0); // ��Ϊactivex��ԭ��ʹ��close���ܻ���ִ���
		return; 
	}else if( msg.pSender == m_pMinBtn ) 
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); 
		return; 
	}else if( msg.pSender == m_pMaxBtn ) 
	{ 
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); 
		return; 
	}else if( msg.pSender == m_pRestoreBtn ) 
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
		return;
	}
	else if( msg.pSender->GetName() == _T("quitbtn") ) 
	{
		PostQuitMessage(0); // ��Ϊactivex��ԭ��ʹ��close���ܻ���ִ���
	}
}

void CRichListWnd::OnSelectChanged( TNotifyUI &msg )
{
	if(msg.pSender->GetName() == _T("down_list"))
	{
		static_cast<CTabLayoutUI*>(GetPaintMgr()->FindControl(_T("tab_main")))->SelectItem(0);
	}
	else if(msg.pSender->GetName() == _T("down_his"))
	{
		static_cast<CTabLayoutUI*>(GetPaintMgr()->FindControl(_T("tab_main")))->SelectItem(1);
	}
}

void CRichListWnd::OnItemClick( TNotifyUI &msg )
{
	TCHAR alert_msg[64] = {0};
	int index = ((CListContainerElementUI *)msg.pSender)->GetIndex();
	wsprintf(alert_msg, _T("ѡ������%d, ���ұ����ڵ�������Ŀ��..."), index);
	MessageBox(NULL, alert_msg, _T("DUILIB DEMO"), MB_OK);            

	CControlUI *find_ctrl = GetPaintMgr()->FindSubControlByName(msg.pSender, _T("down_name"));

	if(find_ctrl)
	{
		MessageBox(NULL, 
			find_ctrl->GetText()+_T(" ѡ���е�������Ŀ����.."), 
			_T("DUILIB DEMO"), MB_OK);   
		((CLabelUI *)find_ctrl)->SetText(_T("�ɳ���̬���ú������..."));
	}
	else
	{
		MessageBox(NULL, _T("��������δΪ�ؼ�����name�����Ҳ���Ҫ�����Ŀؼ�"), 
			_T("DUILIB DEMO"), MB_OK);   
	}

	find_ctrl = GetPaintMgr()->FindSubControlByName(msg.pSender, _T("down_progress"));

	if(find_ctrl)
	{
		TCHAR alert_msg[256] = {0};
		wsprintf(alert_msg, _T("������ֵ:%d"), ((CProgressUI *)find_ctrl)->GetValue());
		MessageBox(NULL, alert_msg, _T("DUILIB DEMO"), MB_OK);   

		((CProgressUI *)find_ctrl)->SetValue(30);
		MessageBox(NULL, _T("�޸��˽�����ֵ"), _T("DUILIB DEMO"), MB_OK);   
	}
}

void CRichListWnd::Notify( TNotifyUI &msg )
{
	return CNotifyPump::NotifyPump(msg);
}

LRESULT CRichListWnd::OnMouseWheel( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	// ���ie�ؼ��ղ���������Ϣ������
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	::ScreenToClient(GetPaintMgr()->GetPaintWindow(), &pt);
	CControlUI* pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(_T("ie")));
	if( pControl && pControl->IsVisible() ) {
		RECT rc = pControl->GetPos();
		if( ::PtInRect(&rc, pt) ) {
			return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		}
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CRichListWnd::OnSysCommand( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	// ��ʱ�����յ�WM_NCDESTROY���յ�wParamΪSC_CLOSE��WM_SYSCOMMAND
	if( wParam == SC_CLOSE ) {
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	BOOL bZoomed = ::IsZoomed(*this);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if( ::IsZoomed(*this) != bZoomed ) {
		if( !bZoomed ) {
			CControlUI* pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(false);
			pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(true);
		}
		else {
			CControlUI* pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(_T("maxbtn")));
			if( pControl ) pControl->SetVisible(true);
			pControl = static_cast<CControlUI*>(GetPaintMgr()->FindControl(_T("restorebtn")));
			if( pControl ) pControl->SetVisible(false);
		}
	}
	return lRes;
}

void CRichListWnd::InitWindow()
{
	m_pCloseBtn = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(_T("closebtn")));
	m_pMaxBtn = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(_T("maxbtn")));
	m_pRestoreBtn = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(_T("restorebtn")));
	m_pMinBtn = static_cast<CButtonUI*>(GetPaintMgr()->FindControl(_T("minbtn")));
}

LRESULT CRichListWnd::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
	CControlUI* pHover = GetPaintMgr()->FindControl(pt);
	if( pHover == NULL ) return 0;
	/*��ʾ��ͣ�������б��ͼ����ʱ����̬�任����ͼ��״̬��ʾ*/
	if(pHover->GetName() == _T("down_ico"))
	{
		MessageBox(NULL, _T("�����ĳ�ؼ����簴ť����ͣ�󣬶�Ŀ��ؼ�����������ı���״̬ͼ���С"), _T("DUILIB DEMO"), MB_OK);
		((CButtonUI *)pHover)->SetAttributeList(
			_T("normalimage=\"file='downlist_pause.png' dest='15,9,32,26'\""));                
	}
	return 0;
}

LRESULT CRichListWnd::OnChar( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	/*��ʾ������Ϣ�Ĵ���*/
	TCHAR press_char = (TCHAR)wParam;
	if(press_char == VK_BACK)
	{
		MessageBox(NULL, _T("�����˻��˼�"), _T("DUILIB DEMO"), MB_OK);
	}
	else
	{
		bHandled = FALSE;
	}
	return 0;
}

LRESULT CRichListWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	ModifyStyle(WS_CAPTION, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	if (!RegisterSkin(_T("duilib.xml"), 0, this))
		return 0;

	GetPaintMgr()->AddPreMessageFilter(this);
	GetPaintMgr()->AddNotifier(this);
	
	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left, \
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	InitWindow();

	return 0;
}

LRESULT CRichListWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
			return ResponseDefaultKeyEvent(wParam);
		default:
			break;
		}
	}
	return FALSE;
}
LRESULT CRichListWnd::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		Close();
		return TRUE;
	}

	return FALSE;
}
CControlUI* CRichListWnd::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}
