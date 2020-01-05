
/***********************************************************
 *	FileName£ºParaseXmlFile.h
 *  Author  £ºDFn Li
 *  Timer   : 2013/3/5 14:24
 *  E-Mail  : libyyu@qq.com
 *  Comment : parase xml file
 *  Version : 1.0.0.1
 ***********************************************************/
#ifndef _PRASEXMLFILE_H
#define _PRASEXMLFILE_H

#pragma once

#include <vector>
#include "UIlib.h"

#define TIXML_USE_STL

struct stLrcInfo
{
	int     nLrcID;
	DuiLib::CDuiString strArtist;
	DuiLib::CDuiString strTitle; 
	DuiLib::CDuiString strKey;
};

typedef std::vector<stLrcInfo> fileinfo_container;

class CParseXmlFile
{
public:
	CParseXmlFile(void);
	virtual ~CParseXmlFile(void);

	static BOOL Parse(fileinfo_container& filesInfo,const DuiLib::CDuiString& strXmlFileName);
	static BOOL ParseBaiduLrcList(fileinfo_container& filesInfo,const DuiLib::CDuiString& strXmlFileName);
};

class CParseJsonFile
{
public:
	CParseJsonFile(void);
	virtual ~CParseJsonFile(void);

	static BOOL Parse(fileinfo_container& filesInfo, const DuiLib::CDuiString& strJsonFileName);
};
#endif//_PRASEXMLFILE_H