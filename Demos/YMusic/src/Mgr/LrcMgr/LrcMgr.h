#pragma  once
#include <queue>
#include "lrcFileReader.h"
#include "SearchLrcFromWeb.h"

class CLrcMgr
{

public:
	typedef std::shared_ptr<CLrcFileReader>  spLrcReaderT;
	typedef std::shared_ptr<CSearchLrcFromWeb> spSearchFromWeb;
public:
	CLrcMgr(void);
	virtual ~CLrcMgr(void);

public:
	spLrcReaderT GetLrcReader(spSongInfoT spSongInfo);
	spLrcReaderT GetLrcReader2();

	void SetSelectLrcDownload(int nIndex,spSongInfoT pSong);
	void DownloadLrcFromOther(spSongInfoT spSongInfo);
	void DownloadLrcFromOther(const DuiLib::CDuiString& sSongName, const DuiLib::CDuiString& sArtistName);
	SongInfoVT GetSearchResultList(){ return m_LrcList ;}
private:
	bool Load(LPCTSTR lpszLrcFile);
	static DWORD __stdcall DownloadLrcFromWeb(LPVOID param);

	static DuiLib::CDuiString GetLrcDir(spSongInfoT spSongInfo);
	static DuiLib::CDuiString FsReplace(const DuiLib::CDuiString& sInput, const TCHAR cNew);
	bool IsLrcExist(const DuiLib::CDuiString& sLrcFileName);
public:
	void SetFont(const DuiLib::CDuiString& sFontName,int nFontHeight,bool bBold,bool bItalic);
	void SetCurRowTextColor(DWORD dwColor);
	void SetNorRowTextColor(DWORD dwColor);
	void SetBkColor(DWORD dwColor);
	void SetLineHeight(int nLineHeight);
	void SetLrcForward(float fTimeOffset = 0.5);
	void SetLrcBack(float fTimeOffset = 0.5);
	void ResetLrc();

	LOGFONT GetFont() const;
	DWORD   GetCurRowTextColor() const;
	DWORD   GetNorRowTextColor() const;
	DWORD   GetBkColor() const;
	size_t  GetLineHeight() const;
	float   GetLrcTimeOffset() const;

	
private:
	void Init();


private:
	DuiLib::CDuiLock		m_lock;
	spLrcReaderT            m_spLrcReader;
	static const TCHAR      REPLACE_CHARS[];
	SongInfoVT              m_LrcList;
    spSearchFromWeb         m_spSerchLrc;

	//∏Ë¥ …Ë÷√
	LOGFONT                 m_LogFont;
	size_t                  m_nLineHeight;
	float                   m_fTimeOffset;//µ•Œª£∫√Î
	DWORD                   m_dwBKColor;
	DWORD                   m_dwCurRowColor;
	DWORD                   m_dwNorRowColor;
};