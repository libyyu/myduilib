#pragma once
#include "UIlib.h"
#include <memory>
class CSongInfo
{
private:
	UINT m_nLocalID;//本地ID
	bool m_bLocal;//if bLocal is true then nSongID is 0;
	UINT m_nSongID;//歌曲ID
	UINT m_nResourceID;//资源ID
	UINT m_nAlbumID;//专辑ID
	UINT m_nArtistID;//作者Id
	UINT m_nAudioLength;//时长;
	UINT m_nPlayCount;//播放次数

	long m_lAddTimeGMT;//添加时间
	long m_lLastModifyTimeGMT;//上次修改时间
	long m_lLastPlayTimeGMT;//上次播放时间

	int m_nFileType;//文件类型
	UINT m_nStartPos;//文件起始位置
	UINT m_nFileSize;//文件大小

	DuiLib::CDuiString m_sSongKey;
	DuiLib::CDuiString	m_sSongName;//歌曲名称
	DuiLib::CDuiString	m_sAlbumName;//专辑名称
	DuiLib::CDuiString	m_sArtistName;//艺术家名称
	DuiLib::CDuiString m_sRealArtistName;//实际艺术家名称
	DuiLib::CDuiString m_sCoverLocalPath;
	DuiLib::CDuiString m_sLrcLocalPath;
	DuiLib::CDuiString m_sLocalPath;
	DuiLib::CDuiString m_sCoverUrl;
	DuiLib::CDuiString m_sLrcUrl;
	DuiLib::CDuiString m_sListenFileUrl;
	DuiLib::CDuiString m_sFileExt;
	DuiLib::CDuiString m_sMD5;
	bool m_bAble;
	int m_nFlag;
	int m_nTag;
public:
	CSongInfo();
	CSongInfo(const CSongInfo& s);
	~CSongInfo();
public:
	bool operator== (const CSongInfo& other);
	bool IsLocal()const;
	void SetLocal(bool bLocal);
	bool IsEnable()const;
	void SetEnable(bool bEnable);
	UINT GetLocalId()const;
	void SetLocalId(UINT nId);
	UINT GetSongId()const;
	void SetSongId(UINT nSongId);
	UINT GetResourceId()const;
	void SetResourceID(UINT nResourceId);
	UINT GetAlbumId()const;
	void SetAlbumId(UINT nAlbumId);
	UINT GetArtistId()const;
	void SetArtistId(UINT nArtistId);
	UINT GetAudioLength()const;
	void SetAudioLength(UINT nAudioLength);
	UINT GetPlayCount()const;
	void SetPlayCount(UINT nPlayCount);
	void AddPlayCount();
	int GetFileType()const;
	void SetFileType(int nFileType);
	UINT GetFileSize()const;
	void SetFileSize(UINT nFileSize);
	UINT GetFileStartPos()const;
	void SetFileStartPos(UINT nStartPos);
	DuiLib::CDuiString GetSongKey()const;
	void SetSongKey(const DuiLib::CDuiString& sSongKey);
	DuiLib::CDuiString GetSongName()const;
	void SetSongName(const DuiLib::CDuiString& sSongName);
	DuiLib::CDuiString GetAlbumName()const;
	void SetAblumName(const DuiLib::CDuiString& sAblumName);
	DuiLib::CDuiString GetArtistName()const;
	void SetArtistName(const DuiLib::CDuiString& sArtistName);
	DuiLib::CDuiString GetRealArtistName()const;
	void SetRealArtistName(const DuiLib::CDuiString& sRealArtistName);
	DuiLib::CDuiString GetLocalPath()const;
	void SetLocalPath(const DuiLib::CDuiString& sPath);
	DuiLib::CDuiString GetCoverLocalPath()const;
	void SetCoverLocalPath(const DuiLib::CDuiString& sPath);
	DuiLib::CDuiString GetLrcLocalPath()const;
	void SetLrcLocalPath(const DuiLib::CDuiString& sPath);
	DuiLib::CDuiString GetListenFileUrl()const;
	void SetListenFileUrl(const DuiLib::CDuiString& sUrl);
	void SetFileExt(const DuiLib::CDuiString& sExt);
	DuiLib::CDuiString GetFileExt() const;

	DuiLib::CDuiString GetMD5()const;
	void SetMD5(const DuiLib::CDuiString& sMD5);

	DuiLib::CDuiString GetCoverUrl()const;
	void SetCoverUrl(const DuiLib::CDuiString& sUrl);
	DuiLib::CDuiString GetLrcUrl()const;
	void SetLrcUrl(const DuiLib::CDuiString& sUrl);

	long GetAddTime()const;
	void SetAddTime(long lTime);
	long GetLastModifyTime()const;
	void SetLastModifyTime(long lTime);
	long GetLastPlayTime()const;
	void SetLastPlayTime(long nTime);

	int GetFlag()const;
	void SetFlag(int nFlag);

	int GetTag() const;
	void SetTag(int ntag);
};
typedef std::shared_ptr<CSongInfo> spSongInfoT;

class songOrder  
{  
public:  
	bool operator()(spSongInfoT nod, spSongInfoT node)  
	{  
		return nod->GetLocalId() < node->GetLocalId();
	}  
}; 
class checkDuplic  
{  
public:  
	bool operator()(spSongInfoT nod, spSongInfoT node)  
	{  
		if (nod->IsLocal() && node->IsLocal())
		{
			return (nod->GetLocalPath() == node->GetLocalPath());
		}
		else if(!nod->IsLocal() && !node->IsLocal())
		{
			return (nod->GetListenFileUrl() == node->GetListenFileUrl());
		}
		else
		{
			return false;
		}
	}  
};  
/************************************************************************/
/*                                                                      */
/************************************************************************/
class CPlayListInfo
{
public:
	enum PlayListType
	{
		plt_sys = 0,//系统列表,如默认列表;
		plt_auto,//智能列表，推荐，自动生成;
		plt_Roaming,//漫游;
		plt_user  //用户自建列表;
	};
protected:
	UINT m_nLocalID;
	UINT m_nID;
	DuiLib::CDuiString m_sName;
	DuiLib::CDuiString m_sComment;
	PlayListType m_eType;
	bool m_bShow;
	int m_nFlag;
	int m_nTag;
public:
	CPlayListInfo();
	~CPlayListInfo();
	CPlayListInfo(const DuiLib::CDuiString& sPlayListName,CPlayListInfo::PlayListType eType);

	UINT GetLocalId()const;
	void SetLocalId(UINT nId);
	UINT GetId()const;
	void SetId(UINT nId);
	CPlayListInfo::PlayListType GetListType()const;
	void SetListType(CPlayListInfo::PlayListType eType);
	DuiLib::CDuiString GetName()const;
	void SetName(const DuiLib::CDuiString& sName);
	DuiLib::CDuiString GetComment()const;
	void SetComment(const DuiLib::CDuiString& sComment);

	bool IsShow()const;
	void SetShow(bool bShow);

	int GetFlag()const;
	void SetFlag(int nFlag);

	int GetTag() const;
	void SetTag(int ntag);
};
typedef std::shared_ptr<CPlayListInfo> spPlayListInfoT;
/************************************************************************/
/*                                                                      */
/************************************************************************/
class CFavInfo
{
public:
	enum FavType
	{	
		ft_album =1,//专辑;
		ft_collect,  //精选集;
		ft_artist,//艺人;
		ft_song//单曲;
	};

private:
	UINT m_nId;
	DuiLib::CDuiString m_sName;
	FavType m_eType;
public:
	CFavInfo();
	CFavInfo(FavType type,UINT nId,LPCTSTR pszName = NULL);
	~CFavInfo();

	UINT GetFavId()const;
	void SetFavId(UINT nId);
	DuiLib::CDuiString GetFavName()const;
	void SetFavName(const DuiLib::CDuiString& sName);
	FavType GetType()const;
	void SetType(FavType eType);

};

typedef std::shared_ptr<CFavInfo> spFavInfoT;
/************************************************************************/
/*                                                                      */
/************************************************************************/
struct SearchResult
{
	bool bNext;
	std::vector<spSongInfoT> SearchList;

	SearchResult()
	{
		bNext=false;
	}
};
/************************************************************************/
/*                                                                      */
/************************************************************************/
struct MessageInfoCountT
{
	unsigned int nPmcount;
	unsigned int nMsgcount;
	unsigned int nFanscount;
	unsigned int nTime;
	unsigned int nSongCount; //送歌
	unsigned int nFavCount; //赞
	unsigned int nShareCount;//好友分享 
	MessageInfoCountT()
	{
		nPmcount = 0;
		nMsgcount = 0;
		nFanscount = 0;
		nTime = 0;
		nSongCount = 0;
		nFavCount = 0;
		nShareCount = 0;
	}
};
typedef std::shared_ptr<MessageInfoCountT> spMessageInfoCountT;
/************************************************************************/
/*                                                                      */
/************************************************************************/
struct UserInfoT
{
	DuiLib::CDuiString sUserID;
	DuiLib::CDuiString sNickName;
	DuiLib::CDuiString sArea;
	DuiLib::CDuiString sAvaterUrl;
	DuiLib::CDuiString sYear;
	unsigned int nFollowCount;
	unsigned int nFollowerCount;
	int nFlag;
	DuiLib::CDuiString sSex;  //性别
	DuiLib::CDuiString sLevel;//等级
	DuiLib::CDuiString sRegTime;
	unsigned int nCollect;
	DuiLib::CDuiString sSignature; //签名
	UserInfoT()
	{
		nFollowCount = 0;
		nFollowerCount = 0;
		nFlag = 0;
		nCollect = 0;
	}
	// 	UserInfoT(const UserInfoT& s)
	// 	{
	// 		*this = s;
	// 	}
};
struct LogonUserInfoT:public UserInfoT
{
	DuiLib::CDuiString sLogonEmail;
	DuiLib::CDuiString sPasswordMD5;
	bool bKeepLogged;
};
typedef std::shared_ptr<LogonUserInfoT> spLogonUserInfoT;
typedef std::shared_ptr<UserInfoT> spUserInfoT;
/************************************************************************/
/*                                                                      */
/************************************************************************/
struct UserInfoListResult
{
	bool bNext;
	std::vector<spUserInfoT> userlist;

	UserInfoListResult()
	{
		bNext=false;
	}
};

struct WebServiceResult
{
	bool         isError;
	std::string  ErrorMsg;

	operator bool() const
	{
		return !isError;
	}
	DuiLib::CDuiString Msg()
	{
		DuiLib::CDuiString s_out;
#ifdef _UNICODE
		std::wstring puni = DuiLib::Convert::UTF8ToUnicode(ErrorMsg.c_str(), ErrorMsg.length());
		s_out.Assign((LPCTSTR)puni.c_str(), puni.size());
#else
		std::string pmb = DuiLib::Convert::UTF8ToMB(ErrorMsg.c_str(), ErrorMsg.length());
		s_out.Assign((LPCTSTR)pmb.c_str(), pmb.size());
#endif
		return s_out;
	}
};
/************************************************************************/
/*                                                                      */
/************************************************************************/
class CSynOperation
{
public:
	enum ObjectType
	{
		SOT_PlayList = 1 ,//播放列表;
		SOT_Song
	};
	enum OperateType
	{
		OT_Add = 1,//添加
		OT_Update,//更新
		OT_Del  //删除
	};
	CSynOperation();
	CSynOperation(unsigned int nId,ObjectType eObType,OperateType eOpType);
	~CSynOperation();

private:
	unsigned int nAutoID;
	unsigned int nObjectID;
	ObjectType eObjType;
	OperateType eOperType;
	int nFlag;
public:
	unsigned int GetAutoID()const;
	void SetAutoID(unsigned int nID);
	unsigned int GetObjectID()const;
	void SetObjectID(unsigned int nID);
	ObjectType GetObjectType()const;
	void SetObjectType(ObjectType eType);
	OperateType GetOperateType()const;
	void SetOperateType(OperateType eType);
	int GetFlag()const;
	void SetFlag(int nflag);
};

typedef std::shared_ptr<CSynOperation> spSynOperationT;
/************************************************************************/
/*                                                                      */
/************************************************************************/
extern unsigned int const MAX_USER_LIST;
extern unsigned int const MAX_FAV_NOTICE;
/************************************************************************/
/*                                                                      */
/************************************************************************/
namespace ConfigEX
{
	extern const TCHAR KEEPMOST_TOP[];	
	extern const TCHAR VOLUME[];
	extern const TCHAR LOCKDESKLRC[];
	extern const TCHAR NOWARN_REMOVELIST[];
	extern const TCHAR NOWARN_REMOVESONG[];
};
/************************************************************************/
/*                                                                      */
/************************************************************************/
enum ProtocolIndex{
	PI_Empty = 1,
	PI_GetOp,
	PI_RegExt,
	PI_UnRegExt,
	PI_Run,
	PI_UnRun,
	PI_RegUrlProto,
	PI_UnRegUrlProto,
	PI_END
};

extern TCHAR const XIAMI_HOST[];
extern TCHAR const SERVER_URL[];

/************************************************************************/
/*                                                                      */
/************************************************************************/

#define  RegURL_KEY				 _T("regURL")
#define  ForGetPasswordURL_KEY   _T("forGetPasswordURL")
#define  SinaLoginURL_KEY		 _T("sinaLoginURL")
#define  QQLoginURL_KEY			 _T("qqLoginURL")
#define  WebLoginSucceedURL_KEY  _T("webLoginSucceedURL")


