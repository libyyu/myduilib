#ifndef GIFIMAGE_H
#define GIFIMAGE_H
#pragma once

#define _USE_GDIPLUS 1

#ifdef _USE_GDIPLUS
#include <GdiPlus.h>
//#include <gdiplusimaging.h>
//#pragma comment( lib, "GdiPlus.lib" )
// UMU: DO NOT use "using namespace" in .h file
//using namespace Gdiplus;
//class Gdiplus::Image;
//class Gdiplus::PropertyItem;
//using Gdiplus::Image;
//using Gdiplus::PropertyItem;
#endif

namespace DuiLib
{

	class CGifImage
	{
	public:
		CGifImage(void);
		~CGifImage(void);

	public:
		BOOL LoadFromFile(LPCTSTR pszFileName);
		BOOL LoadFromIStream(IStream* pStream);
		BOOL LoadFromBuffer(const BYTE* lpBuf, DWORD dwSize);
		BOOL LoadFromResource(HINSTANCE hInstance, LPCTSTR pszResourceName, LPCTSTR pszResType);
		BOOL LoadFromResource(HINSTANCE hInstance, UINT nIDResource, LPCTSTR pszResType);
		BOOL SaveAsFile(LPCTSTR pszFileName);
		void Destroy();
		BOOL IsAnimatedGif();
		UINT GetFrameCount();
		long GetFrameDelay(int nFramePos = -1);
		void ActiveNextFrame();
		void SelectActiveFrame(int nFramePos);
		BOOL Draw(HDC hDestDC, int xDest, int yDest, int nFramePos = -1);
		BOOL Draw(HDC hDestDC, const RECT& rectDest, int nFramePos = -1);
		int GetWidth();
		int GetHeight();
		UINT GetCurFramePos();
		BOOL GetRawFormat(GUID * lpGuid);

	private:
		CLSID GetEncoderClsidByExtension(const WCHAR * lpExtension);

	private:
		Gdiplus::Image* m_pImage;		// ͼƬ����ָ��
		UINT m_nFrameCnt;				// GIF������֡��
		UINT m_nFramePos;				// ��ǰ֡
		long *m_pFrameDelay;			// ÿ֡�л���ʱʱ��
	};
}
#endif//GIFIMAGE_H