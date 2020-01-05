
/***********************************************************
 *	FileName：SearchLrcFromWeb.h
 *  Author  ：DFn Li
 *  Timer   : 2013/3/5 14:24
 *  E-Mail  : libyyu@qq.com
 *  Comment : search lrc from the internet
 *  Version : 1.0.0.1
 ***********************************************************/
#ifndef _SEARCHLRCFROMWEB_H
#define _SEARCHLRCFROMWEB_H

#pragma once

#include "UIlib.h"
#include "Mgr/DB/ShareData.h"
#include "Utility/Convert.h"
typedef struct _SONGINFO
{
	DuiLib::CDuiString strTitle;
	DuiLib::CDuiString strArtist;
	DuiLib::CDuiString strAlbume;
	DuiLib::CDuiString strLrcUrl;
	DuiLib::CDuiString strKey;
}SONGINFO;

typedef std::vector<SONGINFO> SongInfoVT;



class CSearchLrcFromWeb
{
public:
	CSearchLrcFromWeb(void);
	~CSearchLrcFromWeb(void);

	SongInfoVT SearchLrc(spSongInfoT pSong);
	SongInfoVT SearchLrc(const DuiLib::CDuiString& sSongName, const DuiLib::CDuiString& sArtistName);
	bool       DownLoadFile(const DuiLib::CDuiString& strUrl, const DuiLib::CDuiString& strLocalFilePath){return DownloadLrcTrack(strUrl,strLocalFilePath);}
	bool       DownloadLrcFile(const DuiLib::CDuiString& strUrl, const DuiLib::CDuiString& strLocalFilePath);
private:
	SongInfoVT SearchLrcFromLibyyu(spSongInfoT pSong);
	SongInfoVT SearchLrcFromLibyyu(const DuiLib::CDuiString& sSongName, const DuiLib::CDuiString& sArtistName);
	bool       DownloadLrcTrack(const DuiLib::CDuiString& strUrl, const DuiLib::CDuiString& strLocalFilePath);
	bool       DownloadContent(const DuiLib::CDuiString& strUrl, std::vector<BYTE>& buffer);
public:
//	static bool DownLoad(const char* url);
private:

	SongInfoVT m_SongInfoV;

	DuiLib::CDuiString    m_strSearchPathLibyyu;
	DuiLib::CDuiString    m_strDownloadPathLibyyu;

};
/**********************************************************************************/

class EncodeHelper 
{
public:
	static DuiLib::CDuiString _binary_2_hex(const unsigned char* p_data, unsigned int n_len)
	{
		assert(p_data != NULL && n_len > 0);
		TCHAR* p_output = new TCHAR[n_len * 2 + 1];
		ZeroMemory(p_output, sizeof(TCHAR) * (n_len * 2 + 1));
		TCHAR* p_pos = p_output;
		for (unsigned int i = 0; i < n_len; ++i)
		{
			_stprintf_s(p_pos, 3, _T("%02x"), p_data[i]);
			p_pos += 2;
		}
		DuiLib::CDuiString s_result = p_output;
		delete[] p_output;
		return s_result;
	}
	static std::string unicode_2_utf8(const wchar_t* p_input)
	{
		std::string s_result;
		if (p_input != NULL && wcslen(p_input) > 0)
		{
			s_result = em_utility::mci::UnicodeToUTF8(p_input);
		}
		return s_result;
	}
	static std::string unicode_2_utf8(const DuiLib::CDuiString& input)
	{
#ifdef _UNICODE
		return unicode_2_utf8((const wchar_t*)(input.GetData()));
#else
		return std::string(input.GetData());
#endif
	}
	static int my_atoi(const char* s,int radix=16/*默认十六进制形式*/)
	{
		int i,n=0;
		int nTemp=0;
		for(i=0;s[i];i++)
		{

			nTemp = 0;
			if(s[i]>='0'&&s[i]<='9') 
			{
				nTemp = (s[i]-'0');
			}
			else if(s[i]=='a'||s[i]=='A')
			{
				nTemp = 10;
			}
			else if (s[i]=='b'||s[i]=='B')
			{
				nTemp = 11;
			}
			else if (s[i]=='c'||s[i]=='C')
			{
				nTemp = 12;
			}
			else if (s[i]=='d'||s[i]=='D')
			{
				nTemp = 13;
			}
			else if (s[i]=='e'||s[i]=='E')
			{
				nTemp = 14;
			}
			else if (s[i]=='f'||s[i]=='F')
			{
				nTemp = 15;
			}

			n=radix*n+nTemp;

		}

		return(n);


	}
	// 根据歌词ID生成千千CODE校验码算法函数
	static DuiLib::CDuiString CreateQianQianCode(const DuiLib::CDuiString& singer, const DuiLib::CDuiString& title, int lrcId)
	{
		DuiLib::CDuiString qqHexStr = singer + title;
		std::string strqqHex = unicode_2_utf8(qqHexStr);
		qqHexStr = _binary_2_hex((const unsigned char*)strqqHex.c_str(),strqqHex.length()); // 转成UTF8的十六进制形式
		qqHexStr.MakeUpper();
		int length = qqHexStr.GetLength() / 2;
		int* song = new int[length];
		for (int i = 0; i < length; i++) 
		{
			std::string nNum = unicode_2_utf8(qqHexStr.Mid(i * 2, 2));
			song[i] = my_atoi(nNum.c_str());
		}
		int t1 = 0, t2 = 0, t3 = 0;
		t1 = (lrcId & 0x0000FF00) >> 8;
		if ((lrcId & 0x00FF0000) == 0) {
			t3 = 0x000000FF & ~t1;
		} else {
			t3 = 0x000000FF & ((lrcId & 0x00FF0000) >> 16);
		}

		t3 = t3 | ((0x000000FF & lrcId) << 8);
		t3 = t3 << 8;
		t3 = t3 | (0x000000FF & t1);
		t3 = t3 << 8;
		if ((lrcId & 0xFF000000) == 0) {
			t3 = t3 | (0x000000FF & (~lrcId));
		} else {
			t3 = t3 | (0x000000FF & (lrcId >> 24));
		}

		int j = length - 1;
		while (j >= 0) {
			int c = song[j];
			if (c >= 0x80) c = c - 0x100;

			t1 = (int)((c + t2) & 0x00000000FFFFFFFF);
			t2 = (int)((t2 << (j % 2 + 4)) & 0x00000000FFFFFFFF);
			t2 = (int)((t1 + t2) & 0x00000000FFFFFFFF);
			j -= 1;
		}
		j = 0;
		t1 = 0;
		while (j <= length - 1) {
			int c = song[j];
			if (c >= 128) c = c - 256;
			int t4 = (int)((c + t1) & 0x00000000FFFFFFFF);
			t1 = (int)((t1 << (j % 2 + 3)) & 0x00000000FFFFFFFF);
			t1 = (int)((t1 + t4) & 0x00000000FFFFFFFF);
			j += 1;
		}

		int t5 = (int)Conv(t2 ^ t3);
		t5 = (int)Conv(t5 + (t1 | lrcId));
		t5 = (int)Conv(t5 * (t1 | t3));
		t5 = (int)Conv(t5 * (t2 ^ lrcId));

		long t6 = (long)t5;
		if (t6 > 2147483648)
			t5 = (int)(t6 - 4294967296);
		DuiLib::CDuiString strRsult;
		TCHAR szBuff[100]={0};
		_itot_s(t5,szBuff,100,10);
		return DuiLib::CDuiString(szBuff);
	}
	static long Conv(int i) 
	{
		long r = i % 4294967296;
		if (i >= 0 && r > 2147483648)
			r = long(r - 4294967296);

		if (i < 0 && r < 2147483648)
			r = long(r + 4294967296);
		return r;
	}

};


#endif//_SEARCHLRCFROMWEB_H