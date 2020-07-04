// stdafx.cpp : 只包括标准包含文件的源文件
// UIDesigner.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"
#include "UIUtil.h"

CMainFrame* g_pMainFrame = NULL;
CToolBoxWnd* g_pToolBoxWnd = NULL;
COutuptWnd* g_pOutputWnd = NULL;
CPropertiesWnd* g_pPropertiesWnd = NULL;
CFileView* g_pFileView = NULL;
CClassView* g_pClassView = NULL;
CResourceViewBar* g_pResourceView = NULL;
CHookAPI g_pHookAPI;


void DuiLog_Handle(int level, const char* message)
{
	OutputDebugStringA(message);

	if (g_pOutputWnd && ::IsWindow(*g_pOutputWnd))
	{
		CString str = UTF8_TO_CSTRING(message);
		if (level == 2) {
			str = _T("[Error]") + str;
			g_pOutputWnd->AppendLog(str, RGB(255, 0, 0));
		}
		else if (level == 2)
		{
			str = _T("[Warn]") + str;
			g_pOutputWnd->AppendLog(str, RGB(255, 125, 0));
		}
		else {
			g_pOutputWnd->AppendLog(str);
		}
		
	}

}
void format_log(int level, const TCHAR* filename, unsigned int line, const TCHAR* format, ...)
{
	TCHAR szBuffer[2048] = { 0 };
	va_list args;
	va_start(args, format);
	wvnsprintf(szBuffer, lengthof(szBuffer) - 2, format, args);
	va_end(args);

	CDuiString szMessage;
	szMessage.Format(_T("[%s:%d] %s"), filename ? filename : _T("<no source>"), szBuffer);
#ifdef UNICODE
	std::string s = DuiLib::to_string(szMessage.GetData());
#else
	std::string s = szMessage.GetData();
#endif
	DuiLog_Handle(level, s.c_str());
}


UINT glb_ControlICON(CString& strClass)
{
	if (0 == strClass.Compare(_T("Control")))
		return IDI_TOOLBOX_CONTROL;
	else if (0 == strClass.Compare(_T("Container")))
		return IDI_TOOLBOX_CONTAINER;
	else if (0 == strClass.Compare(_T("Label")))
		return IDI_TOOLBOX_LABEL;
	else if (0 == strClass.Compare(_T("Text")))
		return IDI_TOOLBOX_TEXT;
	else if (0 == strClass.Compare(_T("Button")))
		return IDI_TOOLBOX_BUTTON;
	else if (0 == strClass.Compare(_T("ActiveX")))
		return IDI_TOOLBOX_ACTIVE;
	else if (0 == strClass.Compare(_T("Combo")))
		return IDI_TOOLBOX_COMBO;
	else if (0 == strClass.Compare(_T("Edit")))
		return IDI_TOOLBOX_EDIT;
	else if (0 == strClass.Compare(_T("Option")))
		return IDI_TOOLBOX_OPTION;
	else if (0 == strClass.Compare(_T("Progress")))
		return IDI_TOOLBOX_PROGRESS;
	else if (0 == strClass.Compare(_T("ScrollBar")))
		return IDI_TOOLBOX_SCROLLBAR;
	else if (0 == strClass.Compare(_T("Slider")))
		return IDI_TOOLBOX_SLIDER;
	else if (0 == strClass.Compare(_T("HorizontalLayout")))
		return IDI_TOOLBOX_HORIZONTAL_LAYOUT;
	else if (0 == strClass.Compare(_T("VerticalLayout")))
		return IDI_TOOLBOX_VERTICAL_LAYOUT;
	else if (0 == strClass.Compare(_T("TabLayout")))
		return IDI_TOOLBOX_TAB_LAYOUT;
	else if (0 == strClass.Compare(_T("TileLayout")))
		return IDI_TOOLBOX_TILE_LAYOUT;
	else if (0 == strClass.Compare(_T("ChildLayout")))
		return IDI_TOOLBOX_TILE_LAYOUT;
	else if (0 == strClass.Compare(_T("WebBrowser")))
		return IDI_TOOLBOX_ACTIVE;
	else if (0 == strClass.Compare(_T("Flash")))
		return IDI_TOOLBOX_ACTIVE;
	else if (0 == strClass.Compare(_T("GifAnim")))
		return IDI_TOOLBOX_ACTIVE;
	else if (0 == strClass.Compare(_T("TreeView")))
		return IDI_TOOLBOX_ACTIVE;
	else if (0 == strClass.Compare(_T("List")))
		return IDI_TOOLBOX_COMBO;
	else if (0 == strClass.Compare(_T("RichEdit")))
		return IDI_TOOLBOX_EDIT;
	else if (0 == strClass.Compare(_T("DateTime")))
		return IDI_TOOLBOX_ACTIVE;
	else
		return IDI_TOOLBOX_CONTROL;
}

void glb_SetControlPos(CControlUI* pControl, RECT rc)
{
	pControl->SetPos(rc);
	RECT rcItem = pControl->GetPos();

	if (pControl->IsFloat()) {
		CControlUI* pParent = pControl->GetParent();
		if (pParent != NULL) {
			RECT rcParentPos = pParent->GetPos();
				
			DuiLib::TPercentInfo m_piFloatPercent = pControl->GetFloatPercent();
			LONG width = rcParentPos.right - rcParentPos.left;
			LONG height = rcParentPos.bottom - rcParentPos.top;
			RECT rcPercent = { (LONG)(width * m_piFloatPercent.left), (LONG)(height * m_piFloatPercent.top),
				(LONG)(width * m_piFloatPercent.right), (LONG)(height * m_piFloatPercent.bottom) };
			
			DuiLib::CDuiString strValue;
			{
				RECT pos;
				pos.left = rcItem.left - rcPercent.left;
				pos.top = rcItem.top - rcPercent.top;
				pos.right = rcItem.right - rcPercent.right;
				pos.bottom = rcItem.bottom - rcPercent.bottom;
				
				strValue.Format(_T("%d,%d,%d,%d"), pos.left, pos.top, pos.right, pos.bottom);
			}
			pControl->SetXMLAttribute(_T("pos"), strValue);
		}
	}
	else {
		DuiLib::CDuiString strValue;
		{
			RECT pos;
			pos.left = rcItem.left;
			pos.top = rcItem.top;
			pos.right = rcItem.right;
			pos.bottom = rcItem.bottom;

			strValue.Format(_T("%d,%d,%d,%d"), pos.left, pos.top, pos.right, pos.bottom);
		}
		pControl->SetXMLAttribute(_T("pos"), strValue);
	}
}