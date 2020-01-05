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
		Gdiplus::Image* m_pImage;		// 图片对象指针
		UINT m_nFrameCnt;				// GIF动画总帧数
		UINT m_nFramePos;				// 当前帧
		long *m_pFrameDelay;			// 每帧切换延时时间
	};
}
#endif//GIFIMAGE_H