#ifndef __MySkinLayout_h
#define __MySkinLayout_h

#pragma once
#include "MyControls/MyControls.h"

class CMySkinLayoutUI : public DuiLib::CListContainerElementUI
{
public:
	static const int MyRowNum = 5;
	CMySkinLayoutUI();
	virtual ~CMySkinLayoutUI();

	static LPCTSTR GetClassName();
	LPCTSTR GetClass() const;
	int GetImageCount() { return _ImageCount; }
	void AddImage(const DuiLib::CDuiString& szImage);
protected:
	void Init();
	DuiLib::CAlbumButtonUI* _pImage[MyRowNum];
// 	CAlbumButtonUI* _pImage1;
// 	CAlbumButtonUI* _pImage2;
// 	CAlbumButtonUI* _pImage3;

	int _ImageCount;
};

#endif//__MySkinLayout_h