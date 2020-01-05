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
	//�õ����б�����¼�����û�
	bool GetAllUserInfo(std::vector<spLogonUserInfoT> &UserInfoList);
	//�õ�ָ��ID�û�
	spLogonUserInfoT GetUserInfo( const char* sUserID);
	//����û�
	bool AddUserInfo(spLogonUserInfoT spUserInfo);
	//�����û�
	bool UpdateUserInfo(spLogonUserInfoT spUserInfo);
	//ɾ���û�
	bool RemoveUserInfo(const char* sUserID);
	bool RemoveUserInfo(unsigned int nUserID);
	//���һ����¼�û�
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
	static const char PlayListTableName[];//�б�                  --�洢�û����������б�
	static const char SongListTableName[];//������                --ÿ���б���Ӧһ��������
	static const char FavoritesTableName[];//�ղ�				  --�û��ղر�������ͬ��
	static const char PlayRecordTableName[];//���ż�¼			  --�û����ز��ż�¼
	static const char DetailPlayRecordTableName[];//��ϸ���ż�¼
	static const char LocalMusicPacketTableName[];//�������ְ�
	static const char OperationTableName[];//����				  --�û�������ʷ
public:	
	bool GetDictValueTCHAR(const char* sKey, DuiLib::CDuiString& sValue);
	bool UpdateDictValueTCHAR(const char* sKey, const DuiLib::CDuiString& sValue);
	bool InsertDictTCHAR(const char* sKey, const DuiLib::CDuiString& sValue);
	// --��ȡָ��Id���б���Ϣ
	bool GetPlayList(spPlayListInfoT& PlayListInfo,unsigned int nPlayListID);
	// --��ȡ�����б���Ϣ
	bool GetAllPlayLists(std::vector<spPlayListInfoT> &PlayList);
	// --��ȡָ�����͵��б�
	bool GetAllPlayListsByType(std::vector<spPlayListInfoT> &PlayList,unsigned int nType);
	// --����û��б�
	bool AddPlayList(spPlayListInfoT spListInfo);
	// --�����б���Ϣ
	bool UpdatePlayList(spPlayListInfoT spListInfo);
	// --�Ƴ�ָ���б�
	bool RemovePlayList(unsigned int nPlayListID);
	// --������ָ���б�
	bool RenamePlayList(unsigned int nPlayListID, const DuiLib::CDuiString& newName);
	// --���ĳ�����Ƶ��б��Ƿ����
	bool IsPlayListExists(const DuiLib::CDuiString& sPlayListName);
	// --�����б����ƻ�ȡ�б���Ϣ
	bool GetPlayListByName(const DuiLib::CDuiString& sPlayListName,spPlayListInfoT& PlayListInfo);

	// --��ȡָ���б�Id�ĸ����б�
	bool GetPlayListSongs(std::vector<spSongInfoT> &SongList,unsigned int nPlayListID = 0  );
	// --��Ӹ�����ָ���б�
	bool AddSongsToPlayList(spSongInfoT spSongInfo, unsigned int nPlayListID = 0);
	bool AddSongToPlayList(const DuiLib::CDuiString& songPath,unsigned int nPlayListID,spSongInfoT& spSongInfo);
	bool AddSongsToPlayList(std::vector<spSongInfoT> &SongList, unsigned int nPlayListID = 0);
	// --����ָ���б�Id�ĸ�����Ϣ
	bool UpdatePlayListSongInfo(spSongInfoT spSongInfo, unsigned int nPlayListID = 0);
	bool UpdatePlayListSongInfo(std::vector<spSongInfoT> SongArray,unsigned int nPlayListID = 0);
	bool UpdatePlayListSongPath(spSongInfoT spSongInfo, unsigned int nPlayListID = 0);
	// --ɾ��ָ���б�Id�ĸ���
	bool RemovePlayListSongs(spSongInfoT spSongInfo, unsigned int nPlayListID = 0);
	bool RemovePlayListSongs(std::vector<spSongInfoT> &SongList, unsigned int nPlayListID = 0);
	// --���ָ���б�Id�ĸ���
	bool RemoveAllPlayListSongs( unsigned int nPlayListID = 0);
	// --ɾ��ָ���б�Id�ĸ����б�
	bool DeletePlayList( unsigned int nPlayListID = 0);

	// --�ҵ��ղ�
	bool CreateFavTable();
	bool AddFavorites(spFavInfoT spFavInfo);
	bool AddFavorites(std::vector<spFavInfoT> &FavList);
	bool RemoveFavorites(unsigned int nId,unsigned int nType);
	bool RemoveFavorites(std::vector<spFavInfoT> &FavList);
	bool RemoveAllFavorites();
	bool GetAllFavorites(std::vector<spFavInfoT> &FavList);
	
	// --�ҵĲ��ż�¼
	bool AddPlayDetailRecord(unsigned int nSongId,unsigned int nSecond,bool bSkip);
	bool GetRecentFavRecord(std::vector<spFavInfoT> &FavList);
	bool GetPlayRecord(std::vector<std::pair<int,int> >& DataArray,bool bSkipRecord);
	
	// --�������ְ��ӿ�;
	bool GetLocalMusicPacket(unsigned int nPlayListLocalId,std::vector<std::pair<UINT,UINT> >& PacketArray,UINT nStartPos,UINT nLimit,bool bSong);
	bool AddMusicPacket(unsigned int nPlayListLocalId,const std::pair<UINT,UINT>& Packet);
	bool AddMusicPacket(unsigned int nPlayListLocalId,const std::vector<std::pair<UINT,UINT> >& PacketArray);
	bool RemoveMusicPacket(unsigned int nPlayListLocalId,const std::pair<UINT,UINT>& Packet);
	UINT GetMusicPacketCount(unsigned int nPlayListLocalId,bool bSong);
		
	// --Operation�����;
	bool CreateOperationTable();
	bool GetAllOperation(std::vector<spSynOperationT>& OperationArray);
	bool AddOperation(spSynOperationT spSynOperation);
	bool DelOperation(spSynOperationT spSynOperation);
protected:
	// --����Ĭ�ϲ����б�[�����б�](ϵͳ����)
	bool AddDefaultPlayList();
	// --�����ҵ��ղ��б�[�ҵ��ղ�](ϵͳ����)
	bool AddDefaultFav();
	bool CreateTablePlayLists();
	bool CreateTableSongLists(const char* sPlayTableName);
protected:
	virtual bool OnCreateTables();
	virtual bool OnUpdateTables(int oldVer,int newVer);
};
typedef std::shared_ptr<CUserDBase> spUserDBaseT;

#endif//__database_h__