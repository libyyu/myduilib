#ifndef __SysSkinLayout_h
#define __SysSkinLayout_h

#pragma once
#include "MyControls/MyControls.h"
class CSysSkinLayoutUI : public DuiLib::CListContainerElementUI
{
	int _ImageCount;
public:
	static const int SysRowNum = 5;
	CSysSkinLayoutUI();
	virtual ~CSysSkinLayoutUI();

	static LPCTSTR GetClassName();
	LPCTSTR GetClass() const;
	int GetImageCount() { return _ImageCount; }
	void AddImage(const DuiLib::CDuiString& szImage);
protected:
	void Init();
	DuiLib::CAlbumButtonUI* _pImage[SysRowNum];
// 	CAlbumButtonUI* _pImage1;
// 	CAlbumButtonUI* _pImage2;
// 	CAlbumButtonUI* _pImage3;
};






#endif//__SysSkinLayout_h