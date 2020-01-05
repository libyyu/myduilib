#ifndef __database_h__
#define __database_h__
#pragma once
#include <vector>
#include "UIlib.h"
#include <database/Database.h>
#include "ShareData.h"

class CGlobalDBase : public CDatabase
{
public:
	static const char UserInfoTableName[];
	static const char LastLogonUserKey[];
public:
	//得到所有本机登录过的用户
	bool GetAllUserInfo(std::vector<spLogonUserInfoT> &UserInfoList);
	//得到指定ID用户
	spLogonUserInfoT GetUserInfo( const char* sUserID);
	//添加用户
	bool AddUserInfo(spLogonUserInfoT spUserInfo);
	//更新用户
	bool UpdateUserInfo(spLogonUserInfoT spUserInfo);
	//删除用户
	bool RemoveUserInfo(const char* sUserID);
	bool RemoveUserInfo(unsigned int nUserID);
	//最后一个登录用户
	spLogonUserInfoT GetLastUserInfo();
	bool UpdateLastUserInfo(spLogonUserInfoT spUserInfo);

	bool GetAllPlayLists(std::vector<spPlayListInfoT> &PlayList);
	bool AddPlayList(spPlayListInfoT spListInfo);
	bool UpdatePlayList(spPlayListInfoT spListInfo);
	bool RemovePlayList(unsigned int nPlayListID);
public:
	bool GetDictValueTCHAR(const char* sKey, DuiLib::CDuiString& sValue);
	bool UpdateDictValueTCHAR(const char* sKey, const DuiLib::CDuiString& sValue);
	bool InsertDictTCHAR(const char* sKey, const DuiLib::CDuiString& sValue);
protected:
	bool CreateUserTable();
protected:
	virtual bool OnCreateTables();
	virtual bool OnUpdateTables(int oldVer,int newVer);
};
typedef std::shared_ptr<CGlobalDBase> spGlobalDBaseT;

class CUserDBase:public CDatabase
{
public:
	static const char PlayListTableName[];//列表                  --存储用户本地所有列表
	static const char SongListTableName[];//歌曲表                --每个列表都对应一个歌曲表
	static const char FavoritesTableName[];//收藏				  --用户收藏表，需网络同步
	static const char PlayRecordTableName[];//播放记录			  --用户本地播放记录
	static const char DetailPlayRecordTableName[];//详细播放记录
	static const char LocalMusicPacketTableName[];//本地音乐包
	static const char OperationTableName[];//操作				  --用户操作历史
public:	
	bool GetDictValueTCHAR(const char* sKey, DuiLib::CDuiString& sValue);
	bool UpdateDictValueTCHAR(const char* sKey, const DuiLib::CDuiString& sValue);
	bool InsertDictTCHAR(const char* sKey, const DuiLib::CDuiString& sValue);
	// --获取指定Id的列表信息
	bool GetPlayList(spPlayListInfoT& PlayListInfo,unsigned int nPlayListID);
	// --获取所有列表信息
	bool GetAllPlayLists(std::vector<spPlayListInfoT> &PlayList);
	// --获取指定类型的列表
	bool GetAllPlayListsByType(std::vector<spPlayListInfoT> &PlayList,unsigned int nType);
	// --添加用户列表
	bool AddPlayList(spPlayListInfoT spListInfo);
	// --更新列表信息
	bool UpdatePlayList(spPlayListInfoT spListInfo);
	// --移除指定列表
	bool RemovePlayList(unsigned int nPlayListID);
	// --重命名指定列表
	bool RenamePlayList(unsigned int nPlayListID, const DuiLib::CDuiString& newName);
	// --检查某个名称的列表是否存在
	bool IsPlayListExists(const DuiLib::CDuiString& sPlayListName);
	// --根据列表名称获取列表信息
	bool GetPlayListByName(const DuiLib::CDuiString& sPlayListName,spPlayListInfoT& PlayListInfo);

	// --获取指定列表Id的歌曲列表
	bool GetPlayListSongs(std::vector<spSongInfoT> &SongList,unsigned int nPlayListID = 0  );
	// --添加歌曲到指定列表
	bool AddSongsToPlayList(spSongInfoT spSongInfo, unsigned int nPlayListID = 0);
	bool AddSongToPlayList(const DuiLib::CDuiString& songPath,unsigned int nPlayListID,spSongInfoT& spSongInfo);
	bool AddSongsToPlayList(std::vector<spSongInfoT> &SongList, unsigned int nPlayListID = 0);
	// --更新指定列表Id的歌曲信息
	bool UpdatePlayListSongInfo(spSongInfoT spSongInfo, unsigned int nPlayListID = 0);
	bool UpdatePlayListSongInfo(std::vector<spSongInfoT> SongArray,unsigned int nPlayListID = 0);
	bool UpdatePlayListSongPath(spSongInfoT spSongInfo, unsigned int nPlayListID = 0);
	// --删除指定列表Id的歌曲
	bool RemovePlayListSongs(spSongInfoT spSongInfo, unsigned int nPlayListID = 0);
	bool RemovePlayListSongs(std::vector<spSongInfoT> &SongList, unsigned int nPlayListID = 0);
	// --清空指定列表Id的歌曲
	bool RemoveAllPlayListSongs( unsigned int nPlayListID = 0);
	// --删除指定列表Id的歌曲列表
	bool DeletePlayList( unsigned int nPlayListID = 0);

	// --我的收藏
	bool CreateFavTable();
	bool AddFavorites(spFavInfoT spFavInfo);
	bool AddFavorites(std::vector<spFavInfoT> &FavList);
	bool RemoveFavorites(unsigned int nId,unsigned int nType);
	bool RemoveFavorites(std::vector<spFavInfoT> &FavList);
	bool RemoveAllFavorites();
	bool GetAllFavorites(std::vector<spFavInfoT> &FavList);
	
	// --我的播放记录
	bool AddPlayDetailRecord(unsigned int nSongId,unsigned int nSecond,bool bSkip);
	bool GetRecentFavRecord(std::vector<spFavInfoT> &FavList);
	bool GetPlayRecord(std::vector<std::pair<int,int> >& DataArray,bool bSkipRecord);
	
	// --本地音乐包接口;
	bool GetLocalMusicPacket(unsigned int nPlayListLocalId,std::vector<std::pair<UINT,UINT> >& PacketArray,UINT nStartPos,UINT nLimit,bool bSong);
	bool AddMusicPacket(unsigned int nPlayListLocalId,const std::pair<UINT,UINT>& Packet);
	bool AddMusicPacket(unsigned int nPlayListLocalId,const std::vector<std::pair<UINT,UINT> >& PacketArray);
	bool RemoveMusicPacket(unsigned int nPlayListLocalId,const std::pair<UINT,UINT>& Packet);
	UINT GetMusicPacketCount(unsigned int nPlayListLocalId,bool bSong);
		
	// --Operation表操作;
	bool CreateOperationTable();
	bool GetAllOperation(std::vector<spSynOperationT>& OperationArray);
	bool AddOperation(spSynOperationT spSynOperation);
	bool DelOperation(spSynOperationT spSynOperation);
protected:
	// --创建默认播放列表[试听列表](系统属性)
	bool AddDefaultPlayList();
	// --创建我的收藏列表[我的收藏](系统属性)
	bool AddDefaultFav();
	bool CreateTablePlayLists();
	bool CreateTableSongLists(const char* sPlayTableName);
protected:
	virtual bool OnCreateTables();
	virtual bool OnUpdateTables(int oldVer,int newVer);
};
typedef std::shared_ptr<CUserDBase> spUserDBaseT;

#endif//__database_h__