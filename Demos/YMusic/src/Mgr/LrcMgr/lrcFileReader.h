
/***********************************************************
 *	FileName：lrcFileReader.h
 *  Author  ：DFn Li
 *  Timer   : 2013/3/5 14:24
 *  E-Mail  : libyyu@qq.com
 *  Comment : read lrc from file
 *  Version : 1.0.0.1
 ***********************************************************/
#ifndef _LRCFILEREADER_H
#define _LRCFILEREADER_H

#pragma once

#include<vector>

#include "UIlib.h"

class CLrcFileReader
{
public:
	enum TextEncodedMode
	{
		TEM_ANSI,
		TEM_UTF8,
		TEM_UNICODE,
		TEM_UNICODE_BIG
	};
public:
	TextEncodedMode CheckTextEncodedMode(FILE * file);
public:
	DuiLib::CDuiString sArtist;// 艺人名
	DuiLib::CDuiString sTitle;// 歌曲名
	DuiLib::CDuiString sAlbum;//专辑名
	DuiLib::CDuiString sLrcWriter;//歌词制作者

	int Offset;//全局时间调整，正数全部提前
	int iCount;//歌词LrcNode个数
	class LrcNode
	{
	public:
		int StartShowTime;//单位毫秒，与开始播放时的间隔值
		DuiLib::CDuiString sLyric;//歌词内容
		int length;
		int OldPos;//上次显示的位置
		LrcNode(int nStartShowTime,const DuiLib::CDuiString& s )
		{
			StartShowTime = nStartShowTime;
			sLyric = s;
			length = 0;
			OldPos = 0;
		}
// 		LrcNode operator = (LrcNode& node)
// 		{
// 			StartShowTime = node.StartShowTime;
// 			sLyric = node.StartShowTime;
// 			length = node.StartShowTime;
// 			OldPos = node.OldPos;
// 			return (*this);
// 		}
		bool operator <(LrcNode &node)
		{
			return StartShowTime<node.StartShowTime;
		}
	};
	std::vector<LrcNode> Lyrics; 
	DuiLib::CDuiLock m_lock;

	bool Load(const wchar_t * FileName);
	bool Load(const char * FileName);

	bool Parse(FILE * file );//解析文件
	bool ParseLine(const DuiLib::CDuiString &line);//解析一行,成功解析出一个tag, 返回true，否则返回false
	void Clear();	
private:
//	TextEncodedMode CheckTextEncodedMode(FILE * file);
	int m_nErrorCounter;
};

#endif//_LRCFILEREADER_H