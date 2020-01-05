
#ifndef RichEditImp_H
#define RichEditImp_H
#pragma once
#include "ImgOle/IImageOle.h"

namespace DuiLib
{
#define UILIB_API
	struct ImageString
	{
		dui_uint nId;
		dui_uint nIndex;
		dui_uint nWidth;
		dui_uint nHeight;
		dui_bool bUrl;

		CDuiString sFullPath;
		CDuiString sName;
		CDuiString sTip;
	};
	struct RichEdit_Text
	{
		dui_bool bImage;
		CDuiString sText;
	};
	class IRichEdit_ImageStringHandle
	{
	public:
		virtual void MakeImageString(ImageString* pImageInfo, CDuiString& s_out) = 0;
		virtual void MakeImageStringById(dui_uint nId, CDuiString& s_out) = 0;
		virtual void MakeImageStringByFile(CDuiString& sFile, CDuiString& s_out) = 0;
		virtual void MakeImageStringByUrl(CDuiString& sUrl, CDuiString& s_out) = 0;
		virtual void MakeNormalString(CDuiString& s_in, CDuiString& s_out) = 0;

		virtual void PraseImageStringS(CDuiString& s_in, std::vector<RichEdit_Text>& t_out) = 0;
		virtual bool PraseImageString(CDuiString& s_in, ImageString* p_out) = 0;
	};

	HGLOBAL GlobalAllocByString(const char * lpszText, long nLen = -1);
	HGLOBAL GlobalAllocByString(const wchar_t * lpszText, long nLen = -1);

	BOOL EnumSysFont(std::vector<CDuiString> * arrSysFont);	// ö��ϵͳ����
	UILIB_API void Replace(CDuiString& strText, const TCHAR * lpOldStr, const TCHAR * lpNewStr);
	////////////////////////////////////////////////////////////////////////////////
	BOOL UILIB_API RichEdit_SetOleCallback(CRichEditUI* pRichUI, HWND hWnd);
	void UILIB_API RichEdit_RegisterDragDrop(CRichEditUI* pRichUI, HWND hWnd);
	UILIB_API IRichEditOle* RichEdit_GetOleInterface(HWND hWnd);
	int UILIB_API RichEdit_GetWindowTextLength(HWND hWnd);
	int UILIB_API RichEdit_GetWindowText(HWND hWnd, LPTSTR lpszStringBuf, int nMaxCount);
	int UILIB_API RichEdit_GetWindowText(HWND hWnd, CDuiString& strText);
	int UILIB_API RichEdit_GetTextRange(HWND hWnd, CHARRANGE * lpchrg, CDuiString& strText);
	DWORD UILIB_API RichEdit_GetDefaultCharFormat(HWND hWnd, CHARFORMAT& cf);
	BOOL UILIB_API RichEdit_SetDefaultCharFormat(HWND hWnd, CHARFORMAT& cf);
	DWORD UILIB_API RichEdit_GetSelectionCharFormat(HWND hWnd, CHARFORMAT& cf);
	BOOL UILIB_API RichEdit_SetSelectionCharFormat(HWND hWnd, CHARFORMAT& cf);
	void UILIB_API RichEdit_ReplaceSel(HWND hWnd, LPCTSTR lpszNewText, BOOL bCanUndo = FALSE);
	void UILIB_API RichEdit_GetSel(HWND hWnd, LONG& nStartChar, LONG& nEndChar);
	int UILIB_API RichEdit_SetSel(HWND hWnd, LONG nStartChar, LONG nEndChar);

	void UILIB_API RichEdit_SetDefFont(HWND hWnd, LPCTSTR lpFontName, int nFontSize,	// ����Ĭ������
		COLORREF clrText, BOOL bBold, BOOL bItalic,
		BOOL bUnderLine, BOOL bIsLink);
	void UILIB_API RichEdit_SetDefFont(HWND hWnd, LPCTSTR lpFontName, int nFontSize);	// ����Ĭ���������ƺʹ�С
	void UILIB_API RichEdit_SetDefTextColor(HWND hWnd, COLORREF clrText);				// ����Ĭ��������ɫ
	void UILIB_API RichEdit_SetDefLinkText(HWND hWnd, BOOL bEnable);					// ����Ĭ�ϳ�����
	void UILIB_API RichEdit_SetFont(HWND hWnd, LPCTSTR lpFontName, int nFontSize,		// ��������
		COLORREF clrText, BOOL bBold, BOOL bItalic,
		BOOL bUnderLine, BOOL bIsLink);
	void UILIB_API RichEdit_SetFont(HWND hWnd, LPCTSTR lpFontName, int nFontSize);	// �����������ƺʹ�С
	void UILIB_API RichEdit_SetTextColor(HWND hWnd, COLORREF clrText);				// ����������ɫ
	void UILIB_API RichEdit_SetLinkText(HWND hWnd, BOOL bEnable);						// ���ó�����
	BOOL UILIB_API RichEdit_SetStartIndent(HWND hWnd, int nSize);						// ����������(��λ:�)
	BOOL UILIB_API RichEdit_InsertFace(HWND hWnd, LPCTSTR lpszFileName, int nFaceId,	// �������ͼƬ
		int nFaceIndex, COLORREF clrBg, BOOL bAutoScale, int nReservedWidth);
	void UILIB_API RichEdit_GetText(HWND hWnd, CDuiString& strText, IRichEdit_ImageStringHandle* handle = NULL);						// ��ȡ�ı�
	void UILIB_API RichEdit_ReplaceSel(HWND hWnd, LPCTSTR lpszNewText, LPCTSTR lpFontName,	// �滻ѡ���ı�
		int nFontSize, COLORREF clrText, BOOL bBold, BOOL bItalic,
		BOOL bUnderLine, BOOL bIsLink, int nStartIndent, BOOL bCanUndo = FALSE);
	BOOL UILIB_API RichEdit_GetImageOle(HWND hWnd, POINT pt, IImageOle** pImageOle);


	UILIB_API IRichEditOle* RichEdit_GetOleInterface(ITextServices * pTextServices);
	int UILIB_API RichEdit_GetWindowTextLength(ITextServices * pTextServices);
	int UILIB_API RichEdit_GetWindowText(ITextServices * pTextServices, LPTSTR lpszStringBuf, int nMaxCount);
	int UILIB_API RichEdit_GetWindowText(ITextServices * pTextServices, CDuiString& strText);
	int UILIB_API RichEdit_GetTextRange(ITextServices * pTextServices, CHARRANGE * lpchrg, CDuiString& strText);
	DWORD UILIB_API RichEdit_GetDefaultCharFormat(ITextServices * pTextServices, CHARFORMAT& cf);
	BOOL UILIB_API RichEdit_SetDefaultCharFormat(ITextServices * pTextServices, CHARFORMAT& cf);
	DWORD UILIB_API RichEdit_GetSelectionCharFormat(ITextServices * pTextServices, CHARFORMAT& cf);
	BOOL UILIB_API RichEdit_SetSelectionCharFormat(ITextServices * pTextServices, CHARFORMAT& cf);
	void UILIB_API RichEdit_ReplaceSel(ITextServices * pTextServices, LPCTSTR lpszNewText, BOOL bCanUndo = FALSE);
	void UILIB_API RichEdit_GetSel(ITextServices * pTextServices, LONG& nStartChar, LONG& nEndChar);
	int UILIB_API RichEdit_SetSel(ITextServices * pTextServices, LONG nStartChar, LONG nEndChar);

	void UILIB_API RichEdit_SetDefFont(ITextServices * pTextServices, LPCTSTR lpFontName,		// ����Ĭ������
		int nFontSize, COLORREF clrText, BOOL bBold,
		BOOL bItalic, BOOL bUnderLine, BOOL bIsLink);
	void UILIB_API RichEdit_SetFont(ITextServices * pTextServices, LPCTSTR lpFontName, int nFontSize,	// ��������
		COLORREF clrText, BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink);
	BOOL UILIB_API RichEdit_SetStartIndent(ITextServices * pTextServices, int nSize);			// ����������(��λ:�)
	BOOL UILIB_API RichEdit_InsertFace(ITextServices *pTextServices, ITextHost *pTextHost,	// �������ͼƬ
		LPCTSTR lpszFileName, int nFaceId, int nFaceIndex,
		COLORREF clrBg, BOOL bAutoScale, int nReservedWidth);
	void UILIB_API RichEdit_GetText(ITextServices * pTextServices, CDuiString& strText, IRichEdit_ImageStringHandle* handle = NULL);			// ��ȡ�ı�
	void UILIB_API RichEdit_ReplaceSel(ITextServices * pTextServices, LPCTSTR lpszNewText,	// �滻ѡ���ı�
		LPCTSTR lpFontName, int nFontSize, COLORREF clrText,
		BOOL bBold, BOOL bItalic, BOOL bUnderLine, BOOL bIsLink,
		int nStartIndent, BOOL bCanUndo = FALSE);
	BOOL UILIB_API RichEdit_GetImageOle(ITextServices * pTextServices, POINT pt, IImageOle** pImageOle);

	long UILIB_API RichEdit_GetTextLength(ITextServices* pTextServices, DWORD dwFlags = GTL_DEFAULT);
	int UILIB_API RichEdit_GetTextRange(ITextServices* pTextServices, long nStartChar, long nEndChar, CDuiString& s_text);

	typedef HRESULT(CALLBACK  * FGetClassObject)(REFCLSID, REFIID, void**);
	FGetClassObject UILIB_API RegisterComDll(LPCTSTR strPathName);
}


#endif//RichEditImp_H