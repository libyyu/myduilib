
/***********************************************************
 *	FileName��lrcFileReader.h
 *  Author  ��DFn Li
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
	DuiLib::CDuiString sArtist;// ������
	DuiLib::CDuiString sTitle;// ������
	DuiLib::CDuiString sAlbum;//ר����
	DuiLib::CDuiString sLrcWriter;//���������

	int Offset;//ȫ��ʱ�����������ȫ����ǰ
	int iCount;//���LrcNode����
	class LrcNode
	{
	public:
		int StartShowTime;//��λ���룬�뿪ʼ����ʱ�ļ��ֵ
		DuiLib::CDuiString sLyric;//�������
		int length;
		int OldPos;//�ϴ���ʾ��λ��
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

	bool Parse(FILE * file );//�����ļ�
	bool ParseLine(const DuiLib::CDuiString &line);//����һ��,�ɹ�������һ��tag, ����true�����򷵻�false
	void Clear();	
private:
//	TextEncodedMode CheckTextEncodedMode(FILE * file);
	int m_nErrorCounter;
};

#endif//_LRCFILEREADER_H