#include <stdafx.h>
#include "YMusic.h"
#include "DB.h"
#include <time.h>
#ifdef _DEBUG
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#endif

#include "YApp.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
const char CGlobalDBase::UserInfoTableName[] = "user_lists";
const char CGlobalDBase::LastLogonUserKey[] = "_lastLogonUser";


bool CGlobalDBase::GetDictValueTCHAR(const char* sKey, DuiLib::CDuiString& sValue)
{
	std::string u8;
	if (GetDictValue(sKey, u8)) 
	{
		sValue = TransToDuiString(u8.c_str());
	}
	return false;
}
bool CGlobalDBase::UpdateDictValueTCHAR(const char* sKey, const DuiLib::CDuiString& sValue)
{
	std::string u8 = TransToStdString(sValue);
	return UpdateDictValue(sKey, u8.c_str());
}
bool CGlobalDBase::InsertDictTCHAR(const char* sKey, const DuiLib::CDuiString& sValue)
{
	std::string u8 = TransToStdString(sValue);
	return InsertDict(sKey, u8.c_str());
}

bool CGlobalDBase::CreateUserTable()
{
	sLog(_T("[Begin]�����û��б�<table:%s>----���ݿ�"), UserInfoTableName);
	static const char sSql[] = "create table %Q (id Integer primary key autoincrement ,user_id text unique,email text unique,password text,nick_name text,keep_logged int);";
	try
	{
		if(!this->IsExistTable(UserInfoTableName))
			this->ExecSql(sSql,UserInfoTableName);
	}
	catch(CppSQLite3Exception e)
	{
		sLogError(_T("[End]�����û��б�<table:%s>----���ݿ�"),UserInfoTableName);
		assert(false);
		return SQLERROR;
	}
	catch(...)
	{
		sLogError(_T("[End]�����û��б�<table:%s>----���ݿ�"),UserInfoTableName);
		assert(false);
		return SQLERROR;
	}
	sLog(_T("[End]�����û��б�<table:%s>----���ݿ�"),UserInfoTableName);
	return true;
}

bool CGlobalDBase::GetAllUserInfo(std::vector<spLogonUserInfoT> &UserInfoList)
{
	sLog(_T("[Begin]��ȡ�û��б�<table:%s>---���ݿ�"),UserInfoTableName);
	static const char sSql[] = "select user_id,email,password,nick_name,keep_logged from %Q;";
	SQLResult rs = this->QuerySql(sSql,UserInfoTableName);

	while(!rs.eof())
	{
		spLogonUserInfoT spInfo(new LogonUserInfoT);
		if(!spInfo)
			return false;
		spInfo->sUserID      = TransToDuiString(rs.getStringField(0));
		spInfo->sLogonEmail  = TransToDuiString(rs.getStringField(1));
		spInfo->sPasswordMD5 = TransToDuiString(rs.getStringField(2));
		spInfo->sNickName    = TransToDuiString(rs.getStringField(3));
		spInfo->bKeepLogged  = rs.getIntField(4) ? true : false;

		UserInfoList.push_back(spInfo);
		rs.nextRow();
	}
	sLog(_T("[End]��ȡ�û��б�<table:%s>----���ݿ�"),UserInfoTableName);

	return true;
}

spLogonUserInfoT CGlobalDBase::GetUserInfo( const char* sUserID)
{
	sLog(_T("[Begin]��ȡ�û���Ϣ<table:%s><user_id:%s>----���ݿ�"),UserInfoTableName,sUserID);
	static  const char sSql[] = "select user_id,email,password,nick_name,keep_logged from %Q where user_id = %Q;";
	SQLResult rs = this->QuerySql(sSql,UserInfoTableName,sUserID);

	spLogonUserInfoT info;
	if(!rs.eof())
	{
		info = spLogonUserInfoT(new LogonUserInfoT);

		info->sUserID        = TransToDuiString(rs.getStringField(0));
		info->sLogonEmail    = TransToDuiString(rs.getStringField(1));
		info->sPasswordMD5   = TransToDuiString(rs.getStringField(2));
		info->sNickName      = TransToDuiString(rs.getStringField(3));
		info->bKeepLogged    = rs.getIntField(4) ? true : false;
	}
	sLog(_T("[End]��ȡ�û���Ϣ<table:%s><user_id:%s>----���ݿ�"),UserInfoTableName,sUserID);

	return info;
}

bool CGlobalDBase::AddUserInfo(spLogonUserInfoT spUserInfo)
{
	assert(spUserInfo);
	if(!spUserInfo) return false;
	sLog(_T("[Begin]����û���Ϣ<table:%s><user_id:%s>----���ݿ�"),UserInfoTableName,spUserInfo->sUserID.GetData());

	static const char sSql[] = "insert into %Q(user_id ,email,password,nick_name,keep_logged) values (%Q,%Q,%Q,%Q,%u)";

	std::string sLogId    = TransToStdString(spUserInfo->sUserID);
	std::string sEmail    = TransToStdString(spUserInfo->sLogonEmail);
	std::string sPassword = TransToStdString(spUserInfo->sPasswordMD5);
	std::string sNickName = TransToStdString(spUserInfo->sNickName);

	int nErrCode = this->ExecSql(
		sSql,
		UserInfoTableName,
		sLogId.c_str(),
		sEmail.c_str(),
		sPassword.c_str(),
		sNickName.c_str(),
		(spUserInfo->bKeepLogged?1:0)
		);
	sLog(_T("[End]����û���Ϣ<table:%s><user_id:%s>----���ݿ�"),UserInfoTableName,spUserInfo->sUserID.GetData());

	return SQLERROR != nErrCode;
}

bool CGlobalDBase::UpdateUserInfo(spLogonUserInfoT spUserInfo)
{
	assert(spUserInfo);
	if(!spUserInfo) return false;
	sLog(_T("[Begin]�����û���Ϣ<table:%s><user_id:%s>----���ݿ�"),UserInfoTableName,spUserInfo->sUserID.GetData());

	static const char sSql[] = "update %Q set email = %Q, password = %Q,nick_name = %Q,keep_logged = %u where user_id = %Q";
		
	std::string sLogId    = TransToStdString(spUserInfo->sUserID);
	std::string sEmail    = TransToStdString(spUserInfo->sLogonEmail);
	std::string sPassword = TransToStdString(spUserInfo->sPasswordMD5);
	std::string sNickName = TransToStdString(spUserInfo->sNickName);

	int nErrCode = this->ExecSql(
		sSql,
		UserInfoTableName,
		sEmail.c_str(),
		sPassword.c_str(),
		sNickName.c_str(),
		(spUserInfo->bKeepLogged?1:0),
		sLogId.c_str()
		);

	sLog(_T("[End]�����û���Ϣ<table:%s><user_id:%s>----���ݿ�"),UserInfoTableName,spUserInfo->sUserID.GetData());

	return SQLERROR != nErrCode;
}

bool CGlobalDBase::RemoveUserInfo(const char* sUserID)
{
	sLog(_T("[Begin]�Ƴ��û���Ϣ<table:%s><user_id:%s>----���ݿ�"),UserInfoTableName,sUserID);

	static const char sSql[] = "delete from %Q where user_id = %Q";

	int nErrCode = this->ExecSql(
		sSql,
		UserInfoTableName,
		sUserID
		);

	sLog(_T("[End]�Ƴ��û���Ϣ<table:%s><user_id:%s>----���ݿ�"),UserInfoTableName,sUserID);

	return SQLERROR != nErrCode;
}
bool CGlobalDBase::RemoveUserInfo(unsigned int nUserID)
{
	static const char sSql[] = "delete from %Q where user_id = %u";

	return SQLERROR != this->ExecSql(
		sSql,
		UserInfoTableName,
		nUserID
		);
}

spLogonUserInfoT CGlobalDBase::GetLastUserInfo()
{
	sLog(_T("[Begin]��ȡ���һ����¼�û���Ϣ<table:%s>----���ݿ�"),UserInfoTableName);
	int nUserID = 0;
	std::string sUserID;
	if(GetDictValue(LastLogonUserKey, sUserID) && !sUserID.empty())
	{
		nUserID = atoi(sUserID.c_str());
		sUserID = sUserID;
	}

	if(""==sUserID)
	{
		sLogError(_T("[End]��ȡ���һ����¼�û���Ϣ<table:%s>----���ݿ�"),UserInfoTableName);
		return spLogonUserInfoT();
	}
	sLog(_T("[End]��ȡ���һ����¼�û���Ϣ<table:%s>----���ݿ�"),UserInfoTableName);
	return GetUserInfo(sUserID.c_str());
}

bool CGlobalDBase::UpdateLastUserInfo(spLogonUserInfoT spUserInfo)
{
	assert(spUserInfo);
	if(!spUserInfo) return false;
	sLog(_T("[Begin]�������һ����¼�û���Ϣ<table:%s><user_id:%s>----���ݿ�"),UserInfoTableName,spUserInfo->sUserID.GetData());
// 		DuiLib::CDuiString sUserID;
// 		sUserID.Format(_T("%u"),spUserInfo->nUserID);
	DeleteDict(LastLogonUserKey);
//		return InsertDict(LastLogonUserKey,sUserID);
	sLog(_T("[End]�������һ����¼�û���Ϣ<table:%s><user_id:%s>----���ݿ�"),UserInfoTableName,spUserInfo->sUserID.GetData());
	return InsertDict(LastLogonUserKey,spUserInfo->sUserID);
}

bool CGlobalDBase::OnCreateTables()
{
	return CreateUserTable();
}
bool CGlobalDBase::OnUpdateTables(int oldVer,int newVer)
{
	return true;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
const char CUserDBase::PlayListTableName[] = "play_lists";
const char CUserDBase::SongListTableName[] = "song_lists";
const char CUserDBase::FavoritesTableName[] = "favorites";
const char CUserDBase::PlayRecordTableName[] = "play_record";
const char CUserDBase::DetailPlayRecordTableName[] = "play_record_detail";
const char CUserDBase::LocalMusicPacketTableName[] = "local_music_packet";
const char CUserDBase::OperationTableName[] = "operation";

bool CUserDBase::GetDictValueTCHAR(const char* sKey, DuiLib::CDuiString& sValue)
{
	std::string u8;
	if (GetDictValue(sKey, u8))
	{
		sValue = TransToDuiString(u8.c_str());
	}
	return false;
}
bool CUserDBase::UpdateDictValueTCHAR(const char* sKey, const DuiLib::CDuiString& sValue)
{
	std::string u8 = TransToStdString(sValue);
	return UpdateDictValue(sKey, u8.c_str());
}
bool CUserDBase::InsertDictTCHAR(const char* sKey, const DuiLib::CDuiString& sValue)
{
	std::string u8 = TransToStdString(sValue);
	return InsertDict(sKey, u8.c_str());
}

/*���������б�*/
bool CUserDBase::CreateTablePlayLists()
{
	sLog(_T("[Begin]���������б�<table:%s>----���ݿ�"),PlayListTableName);
	static const char sSql[] = "create table %Q (local_id Integer primary key autoincrement,"
		"web_id int,list_name text unique,type int,is_show int,gmt_create int,gmt_last_modify int,flag int);";
	try
	{
		if(!this->IsExistTable(PlayListTableName))
			this->ExecSql(sSql,PlayListTableName);
	}
	catch(CppSQLite3Exception e)
	{
		assert(false);
		return SQLERROR;
	}
	catch(...)
	{
		assert(false);
		return SQLERROR;
	}
	sLog(_T("[End]���������б�<table:%s>----���ݿ�"),PlayListTableName);

	AddDefaultPlayList();
	AddDefaultFav();

	return true;
}
/*�����б����������ݿ�*/
bool CUserDBase::CreateTableSongLists(const char* sPlayTableName)
{
	assert(sPlayTableName);
	sLog(_T("[Begin]���������б�<table:%s>----���ݿ�"),sPlayTableName);
	//song_local_id,song_id,song_name,artist_id,artist_name,album_id,"
	//album_name,cover_url,cover_path,listen_url,audio_length,"
	//is_disable,lrc_url,audio_type,audio_size,audio_start_pos,"
	//md5,local_path,is_local,gmt_last_play,gmt_last_modify,gmt_add_time,play_count
	static const char sSql[] = "create table %Q (song_local_id Integer primary key autoincrement,song_id int,song_key text, song_name text,artist_id int,artist_name text,"
		"album_id int,album_name text,cover_url text,cover_path text,listen_url text,audio_length int,is_disable int,lrc_url text,audio_type int,"
		"audio_size int,audio_start_pos int,md5 text,local_path text,is_local int,gmt_last_play int,gmt_last_modify int,gmt_add_time int,play_count int default 0);";
	try
	{
		if(!this->IsExistTable(sPlayTableName))
			this->ExecSql(sSql,sPlayTableName);
	}
	catch(CppSQLite3Exception e)
	{
		sLogError(_T("[End]���������б�<table:%s>----���ݿ�"),sPlayTableName);
		assert(false);
		return SQLERROR;
	}
	catch(...)
	{
		sLogError(_T("[End]���������б�<table:%s>----���ݿ�"),sPlayTableName);
		assert(false);
		return SQLERROR;
	}
	sLog(_T("[End]���������б�<table:%s>----���ݿ�"),sPlayTableName);
	return true;
}
bool CUserDBase::GetPlayList(spPlayListInfoT& PlayListInfo,unsigned int nPlayListID)
{
	sLog(_T("[Begin]��ȡָ��<local_id:%d>�����б�<table:%s>----���ݿ�"),nPlayListID,PlayListTableName);
	static const char sSql[] = "select local_id,web_id,list_name,type,is_show,flag from %Q where local_id = %u;";

	SQLResult rs = this->QuerySql(sSql,PlayListTableName,nPlayListID);
	if(!rs.eof())
	{
		if(!PlayListInfo)
			PlayListInfo = spPlayListInfoT(new CPlayListInfo);
		if(!PlayListInfo)
			return false;
		PlayListInfo->SetLocalId(rs.getIntField(0));
		PlayListInfo->SetId(rs.getIntField(1));
		PlayListInfo->SetName(TransToDuiString(rs.getStringField(2)));
		PlayListInfo->SetListType((CPlayListInfo::PlayListType)(rs.getIntField(3)));
		PlayListInfo->SetShow(rs.getIntField(4)==1 ? true :false);
		PlayListInfo->SetFlag(rs.getIntField(5));

	}
	sLog(_T("[End]��ȡָ��<local_id:%d>�����б�<table:%s>----���ݿ�"),nPlayListID,PlayListTableName);
	return true;
}
/*��ȡ�û������б�*/
bool CUserDBase::GetAllPlayLists(std::vector<spPlayListInfoT> &PlayList)
{
	sLog(_T("[Begin]��ȡ�����б�<table:%s>----���ݿ�"),PlayListTableName);
	static const char sSql[] = "select local_id,web_id,list_name,type,is_show,flag from %Q;";

	SQLResult rs = this->QuerySql(sSql,PlayListTableName);
	while(!rs.eof())
	{
		spPlayListInfoT spInfo(new CPlayListInfo);
		if(!spInfo)
			return false;
		spInfo->SetLocalId(rs.getIntField(0));
		spInfo->SetId(rs.getIntField(1));
		spInfo->SetName(TransToDuiString(rs.getStringField(2)));
		spInfo->SetListType((CPlayListInfo::PlayListType)(rs.getIntField(3)));
		spInfo->SetShow(rs.getIntField(4)==1 ? true :false);
		spInfo->SetFlag(rs.getIntField(5));

		PlayList.push_back(spInfo);
		rs.nextRow();
	}
	sLog(_T("[End]��ȡ�����б�<table:%s>----���ݿ�"),PlayListTableName);
	return true;
}
	
bool CUserDBase::GetAllPlayListsByType(std::vector<spPlayListInfoT> &PlayList,unsigned int nType)
{
	sLog(_T("[Begin]��ȡ<type:%d>�����б�<table:%s>----���ݿ�"),nType,PlayListTableName);
	static const char sSql[] = "select local_id,web_id,list_name,type,is_show,flag from %Q where type=%d;";

	SQLResult rs = this->QuerySql(sSql,PlayListTableName,nType);
	while(!rs.eof())
	{
		spPlayListInfoT spInfo(new CPlayListInfo);
		if(!spInfo)
			return false;
		spInfo->SetLocalId(rs.getIntField(0));
		spInfo->SetId(rs.getIntField(1));
		spInfo->SetName(TransToDuiString(rs.getStringField(2)));
		spInfo->SetListType((CPlayListInfo::PlayListType)(rs.getIntField(3)));
		spInfo->SetShow(rs.getIntField(4)==1 ? true :false);
		spInfo->SetFlag(rs.getIntField(5));

		PlayList.push_back(spInfo);
		rs.nextRow();
	}
	sLog(_T("[End]��ȡ<type:%d>�����б�<table:%s>----���ݿ�"),nType,PlayListTableName);
	return true;
}
	
/*�½������б�*/
bool CUserDBase::AddPlayList(spPlayListInfoT spListInfo)
{
	assert(spListInfo);
	if(!spListInfo) return false;

	sLog(_T("[Begin]�½������б�<table:%s>----���ݿ�"),PlayListTableName);

	static const char sSql[] = "insert into %Q(web_id,list_name,type,is_show,gmt_create,gmt_last_modify,flag) values (%u,%Q,%u,%u,%u,%u,%u);";

	std::string sNameUTF8    = TransToStdString(spListInfo->GetName());
	int gmt = (int)time(NULL);
	bool bResult = (this->ExecSql(
		sSql,
		PlayListTableName,
		spListInfo->GetId(),
		sNameUTF8.c_str(),
		(int)(spListInfo->GetListType()),
		spListInfo->IsShow() ? 1 : 0,
		gmt,
		gmt,
		spListInfo->GetFlag()
		)!=SQLERROR);

	if (bResult)
	{
		SQLResult rs = this->QuerySql("select last_insert_rowid() from %Q",PlayListTableName);
		if(rs.eof())
			return false;
		spListInfo->SetLocalId(rs.getIntField(0));

		char szBuf[MAX_PATH] = {0};
		sprintf_s(szBuf,MAX_PATH,"%s_%d",SongListTableName,spListInfo->GetLocalId());
		CreateTableSongLists(szBuf);
	}
	sLog(_T("[End]�½������б�<table:%s>----���ݿ�"),PlayListTableName);
	return bResult;
}
/*���²����б�*/
bool CUserDBase::UpdatePlayList(spPlayListInfoT spListInfo)
{
	assert(spListInfo);
	if(!spListInfo) return false;

	sLog(_T("[Begin]����ָ��<local_id:%d>�����б�<table:%s>----���ݿ�"),spListInfo->GetLocalId(),PlayListTableName);

	static const char sSql[] = "update %Q set web_id = %u ,list_name = %Q ,is_show = %u,gmt_last_modify = %u where local_id = %u;";

	std::string sNameUTF8    = TransToStdString(spListInfo->GetName());

	int nErrCode = this->ExecSql(
		sSql,
		PlayListTableName,
		spListInfo->GetId(),
		sNameUTF8.c_str(),
		spListInfo->IsShow() ? 1 : 0,
		(int)(time(NULL)),
		spListInfo->GetLocalId()
		);
	sLog(_T("[End]����ָ��<local_id:%d>�����б�<table:%s>----���ݿ�"),spListInfo->GetLocalId(),PlayListTableName);

	return SQLERROR != nErrCode;
}
/*�Ƴ������б�*/
bool CUserDBase::RemovePlayList(unsigned int nPlayListID)
{
	sLog(_T("[Begin]�Ƴ�ָ��<local_id:%d>�����б�<table:%s>----���ݿ�"),nPlayListID,PlayListTableName);
	static const char sSql[] = "delete from %Q where local_id = %u;";

//		RemoveAllPlayListSongs(nPlayListID);
	// --��ɾ������Id�ĸ����б�
	DeletePlayList( nPlayListID );

	int nErrCode = this->ExecSql(sSql,PlayListTableName,nPlayListID);

	sLog(_T("[End]�Ƴ�ָ��<local_id:%d>�����б�<table:%s>----���ݿ�"),nPlayListID,PlayListTableName);

	return SQLERROR != nErrCode;
}
bool CUserDBase::RenamePlayList(unsigned int nPlayListID, const DuiLib::CDuiString& newName)
{
	sLog(_T("[Begin]����ָ��<local_id:%d>�����б�<table:%s>----���ݿ�"),nPlayListID,PlayListTableName);

	static const char sSql[] = "update %Q set list_name = %Q gmt_last_modify = %u where local_id = %u;";

	std::string sNameUTF8    = TransToStdString(newName);

	int nErrCode = this->ExecSql(
		sSql,
		PlayListTableName,
		sNameUTF8.c_str(),
		(int)(time(NULL)),
		nPlayListID
		);
	sLog(_T("[End]����ָ��<local_id:%d>�����б�<table:%s>----���ݿ�"),nPlayListID,PlayListTableName);

	return SQLERROR != nErrCode;
}
bool CUserDBase::IsPlayListExists(const DuiLib::CDuiString& sPlayListName)
{
	sLog(_T("[Begin]���ָ��<list_name:%s>�����б�<table:%s>----���ݿ�"), sPlayListName.GetData(),PlayListTableName);

	static const char sSql[] = "select local_id,web_id,list_name from %Q where  list_name = %Q;";
	SQLResult rs = this->QuerySql(sSql,PlayListTableName,TransToStdString(sPlayListName).c_str());
	bool bFind = false;
	if(!rs.eof())
		bFind =  true;
	else 
		bFind =  false;

	sLog(_T("[End]���ָ��<list_name:%s>�����б�<table:%s>----���ݿ�"),sPlayListName.GetData(),PlayListTableName);
	return bFind;
}
bool CUserDBase::GetPlayListByName(const DuiLib::CDuiString& sPlayListName,spPlayListInfoT& PlayListInfo)
{
	sLog(_T("[Begin]��ȡָ��<list_name:%s>�����б�<table:%s>----���ݿ�"),sPlayListName.GetData(),PlayListTableName);
	static const char sSql[] = "select local_id,web_id,list_name,type,is_show,flag from %Q where list_name = %Q;";

	SQLResult rs = this->QuerySql(sSql,PlayListTableName,TransToStdString(sPlayListName).c_str());
	if(!rs.eof())
	{
		if(!PlayListInfo)
			PlayListInfo = spPlayListInfoT(new CPlayListInfo);
		if(!PlayListInfo)
			return false;
		PlayListInfo->SetLocalId(rs.getIntField(0));
		PlayListInfo->SetId(rs.getIntField(1));
		PlayListInfo->SetName(TransToDuiString(rs.getStringField(2)));
		PlayListInfo->SetListType((CPlayListInfo::PlayListType)(rs.getIntField(3)));
		PlayListInfo->SetShow(rs.getIntField(4)==1 ? true :false);
		PlayListInfo->SetFlag(rs.getIntField(5));
	}
	sLog(_T("[End]���ָ��<list_name:%s>�����б�<table:%s>----���ݿ�"),sPlayListName.GetData(),PlayListTableName);
	return true;
}
bool CUserDBase::AddDefaultPlayList()
{
	sLog(_T("[Begin]�½������б�----���ݿ�"));
	if(IsPlayListExists(_T("�����б�")))
		return true;
	spPlayListInfoT spListInfo = spPlayListInfoT(new CPlayListInfo);
	assert(spListInfo);
	if(!spListInfo) return false;
	spListInfo->SetListType(CPlayListInfo::plt_sys);
	spListInfo->SetShow(true);
	spListInfo->SetComment(_T("�����б�"));
	spListInfo->SetName(_T("�����б�"));

	bool bRet = AddPlayList(spListInfo);
	if(!bRet)
		return	assert(false),sLogError(_T("[End]�½������б�----���ݿ�")), false;

	sLog(_T("[End]�½������б�----���ݿ�"));
	return bRet;
}
/*�õ������б�ID��Ӧ�ĸ����б�*/
bool CUserDBase::GetPlayListSongs(std::vector<spSongInfoT> &SongList,unsigned int nPlayListID /* = 0 */ )
{
	char szBuf[MAX_PATH] = {0};
	sprintf_s(szBuf,MAX_PATH,"%s_%d",SongListTableName,nPlayListID);
	sLog(_T("[Begin]��ȡ�б�����<table:%s>----���ݿ�"),szBuf);

	static const char sSql[] = "select song_local_id,song_id,song_key,song_name,artist_id,artist_name,album_id,album_name,"
		"cover_url,cover_path,listen_url,audio_length,is_disable,lrc_url,audio_type,audio_size,audio_start_pos,"
		"md5,local_path,is_local,gmt_last_play,gmt_last_modify,gmt_add_time,play_count "
		" from %Q;";
		
	SQLResult rs = this->QuerySql(sSql,szBuf);
	while(!rs.eof())
	{
		spSongInfoT spInfo(new CSongInfo);
		if(!spInfo)
			return false;
		int nIndex = 0;
		spInfo->SetLocalId(rs.getIntField(nIndex++));
		spInfo->SetSongId(rs.getIntField(nIndex++));
		spInfo->SetSongKey(TransToDuiString(rs.getStringField(nIndex++)));
		spInfo->SetSongName(TransToDuiString(rs.getStringField(nIndex++)));
		spInfo->SetArtistId(rs.getIntField(nIndex++));
		spInfo->SetArtistName(TransToDuiString(rs.getStringField(nIndex++)));
		spInfo->SetAlbumId(rs.getIntField(nIndex++));
		spInfo->SetAblumName(TransToDuiString(rs.getStringField(nIndex++)));
		spInfo->SetCoverUrl(TransToDuiString(rs.getStringField(nIndex++)));
		spInfo->SetCoverLocalPath(TransToDuiString(rs.getStringField(nIndex++)));
		spInfo->SetListenFileUrl(TransToDuiString(rs.getStringField(nIndex++)));
		spInfo->SetAudioLength(rs.getIntField(nIndex++));
		spInfo->SetEnable((rs.getIntField(nIndex++)==1));
		spInfo->SetLrcUrl(TransToDuiString(rs.getStringField(nIndex++)));
		spInfo->SetFileType(rs.getIntField(nIndex++));
		spInfo->SetFileSize(rs.getIntField(nIndex++));
		spInfo->SetFileStartPos(rs.getIntField(nIndex++));
		spInfo->SetMD5(TransToDuiString(rs.getStringField(nIndex++)));
		spInfo->SetLocalPath(TransToDuiString(rs.getStringField(nIndex++)));
		spInfo->SetLocal((rs.getIntField(nIndex++)==1));
		spInfo->SetLastPlayTime((long)rs.getIntField(nIndex++));
		spInfo->SetLastModifyTime((long)rs.getIntField(nIndex++));
		spInfo->SetAddTime((long)rs.getIntField(nIndex++));
		spInfo->SetPlayCount(rs.getIntField(nIndex++));

		SongList.push_back(spInfo);
		rs.nextRow();
	}
	sLog(_T("[End]��ȡ�б�����<table:%s>----���ݿ�"),szBuf);
	return true;
}
/*��Ӹ�����ָ��ID�Ĳ����б�*/
bool CUserDBase::AddSongToPlayList(const DuiLib::CDuiString& songPath,unsigned int nPlayListID,spSongInfoT& spSongInfo)
{
	spSongInfo = thePlayListMgr->PraseFromPath(songPath);
	if (!spSongInfo)
		return false;

	assert(spSongInfo);
		
	char szBuf[MAX_PATH] = {0};
	sprintf_s(szBuf,MAX_PATH,"%s_%d",SongListTableName,nPlayListID);
	sLog(_T("[Begin]��������<table:%s>----���ݿ�"),szBuf);

	static const char sSql[] = "insert into %Q(song_id,song_key,song_name,artist_id,artist_name,album_id,"
		"album_name,cover_url,cover_path,listen_url,audio_length,is_disable,lrc_url,audio_type,audio_size,"
		"audio_start_pos,md5,local_path,is_local,gmt_add_time,play_count)"
		"values(%u,%Q,%u,%Q,%u,%Q,%Q,%Q,%Q,%u,%u,%Q,%u,%u,%u,%Q,%Q,%u,%u,%u);";

	int bResult = (this->ExecSql(sSql,
		szBuf,
		spSongInfo->GetSongId(),
		TransToStdString(spSongInfo->GetSongKey()).c_str(),
		TransToStdString(spSongInfo->GetSongName()).c_str(),
		spSongInfo->GetArtistId(),
		TransToStdString(spSongInfo->GetArtistName()).c_str(),
		spSongInfo->GetAlbumId(),
		TransToStdString(spSongInfo->GetAlbumName()).c_str(),
		TransToStdString(spSongInfo->GetCoverUrl()).c_str(),
		TransToStdString(spSongInfo->GetCoverLocalPath()).c_str(),
		TransToStdString(spSongInfo->GetListenFileUrl()).c_str(),
		spSongInfo->GetAudioLength(),
		(spSongInfo->IsEnable() ? 1 :0),
		TransToStdString(spSongInfo->GetLrcUrl()).c_str(),
		spSongInfo->GetFileType(),
		spSongInfo->GetFileSize(),
		spSongInfo->GetFileStartPos(),
		TransToStdString(spSongInfo->GetMD5()).c_str(),
		TransToStdString(spSongInfo->GetLocalPath()).c_str(),
		(spSongInfo->IsLocal() ? 1 : 0),
		(int)time(NULL),
		spSongInfo->GetPlayCount()));
	if (bResult)
	{
		SQLResult rs = this->QuerySql("select last_insert_rowid() from %Q",szBuf);
		if(rs.eof())
			return false;
		spSongInfo->SetLocalId(rs.getIntField(0));
	}
	sLog(_T("[End]��������<table:%s>----���ݿ�"),szBuf);

	return true;
}
bool CUserDBase::AddSongsToPlayList(spSongInfoT spSongInfo, unsigned int nPlayListID/* = 0*/)
{
	assert(spSongInfo);
		
	char szBuf[MAX_PATH] = {0};
	sprintf_s(szBuf,MAX_PATH,"%s_%d",SongListTableName,nPlayListID);
	sLog(_T("[Begin]��������<table:%s>----���ݿ�"),szBuf);

	static const char sSql[] = "insert into %Q(song_id,song_key,song_name,artist_id,artist_name,album_id,"
		"album_name,cover_url,cover_path,listen_url,audio_length,is_disable,lrc_url,audio_type,audio_size,"
		"audio_start_pos,md5,local_path,is_local,gmt_add_time,play_count)"
		"values(%u,%Q,%Q,%u,%Q,%u,%Q,%Q,%Q,%Q,%u,%u,%Q,%u,%u,%u,%Q,%Q,%u,%u,%u);";

	int bResult = (this->ExecSql(sSql,
		szBuf,
		spSongInfo->GetSongId(),
		TransToStdString(spSongInfo->GetSongKey()).c_str(),
		TransToStdString(spSongInfo->GetSongName()).c_str(),
		spSongInfo->GetArtistId(),
		TransToStdString(spSongInfo->GetArtistName()).c_str(),
		spSongInfo->GetAlbumId(),
		TransToStdString(spSongInfo->GetAlbumName()).c_str(),
		TransToStdString(spSongInfo->GetCoverUrl()).c_str(),
		TransToStdString(spSongInfo->GetCoverLocalPath()).c_str(),
		TransToStdString(spSongInfo->GetListenFileUrl()).c_str(),
		spSongInfo->GetAudioLength(),
		(spSongInfo->IsEnable() ? 1 :0),
		TransToStdString(spSongInfo->GetLrcUrl()).c_str(),
		spSongInfo->GetFileType(),
		spSongInfo->GetFileSize(),
		spSongInfo->GetFileStartPos(),
		TransToStdString(spSongInfo->GetMD5()).c_str(),
		TransToStdString(spSongInfo->GetLocalPath()).c_str(),
		(spSongInfo->IsLocal() ? 1 : 0),
		(int)time(NULL),
		spSongInfo->GetPlayCount()));
	if (bResult)
	{
		SQLResult rs = this->QuerySql("select last_insert_rowid() from %Q",szBuf);
		if(rs.eof())
			return false;
		spSongInfo->SetLocalId(rs.getIntField(0));
	}
	sLog(_T("[End]��������<table:%s>----���ݿ�"),szBuf);

	return true;
}
bool CUserDBase::AddSongsToPlayList(std::vector<spSongInfoT> &SongList, unsigned int nPlayListID /*= 0*/)
{
	if(!this->BeginTransaction())
		return assert(false),false;
	for (auto it=SongList.begin();it!=SongList.end();++it)
	{
		this->AddSongsToPlayList(*it,nPlayListID);
	}
	if(!this->Commit())
		return assert(false),false;
	return true;
}
/*����ָ�������б�ID�ĸ���*/
bool CUserDBase::UpdatePlayListSongInfo(spSongInfoT spSongInfo, unsigned int nPlayListID/* = 0*/)
{   
	assert(spSongInfo);
		
	char szBuf[MAX_PATH] = {0};
	sprintf_s(szBuf,MAX_PATH,"%s_%d",SongListTableName,nPlayListID);
	sLog(_T("[Begin]���¸���<table:%s>----���ݿ�"),szBuf);

	static const char sSql[] ="update %Q set song_id = %u, song_key = %Q, song_name = %Q,"
		"artist_id = %u,artist_name = %Q,album_id = %u, album_name = %Q,"
		"cover_url = %Q,cover_path = %Q,listen_url = %Q,audio_length = %u,"
		"is_disable = %u,lrc_url = %Q,audio_type = %u,audio_size = %u,"
		"audio_start_pos = %u,md5 = %Q,local_path = %Q,is_local = %u,"
		"gmt_last_play = %u,gmt_last_modify = %u,play_count = %u where song_local_id = %u;";
	int nErrCode = this->ExecSql(
		sSql,
		szBuf,
		spSongInfo->GetSongId(),
		TransToStdString(spSongInfo->GetSongKey()).c_str(),
		TransToStdString(spSongInfo->GetSongName()).c_str(),			
		spSongInfo->GetArtistId(),
		TransToStdString(spSongInfo->GetArtistName()).c_str(),
		spSongInfo->GetAlbumId(),
		TransToStdString(spSongInfo->GetAlbumName()).c_str(),
		TransToStdString(spSongInfo->GetCoverUrl()).c_str(),
		TransToStdString(spSongInfo->GetCoverLocalPath()).c_str(),
		TransToStdString(spSongInfo->GetListenFileUrl()).c_str(),
		spSongInfo->GetAudioLength(),
		spSongInfo->IsEnable() ? 0 : 1,
		TransToStdString(spSongInfo->GetLrcUrl()).c_str(),
		spSongInfo->GetFileType(),
		spSongInfo->GetFileSize(),
		spSongInfo->GetFileStartPos(),	
		TransToStdString(spSongInfo->GetMD5()).c_str(),
		TransToStdString(spSongInfo->GetLocalPath()).c_str(),
		spSongInfo->IsLocal() ? 1 : 0,
		spSongInfo->GetLastPlayTime(),
		(int)time(NULL),			
		spSongInfo->GetPlayCount(),
		spSongInfo->GetLocalId()
		);
	sLog(_T("[End]���¸���<table:%s>----���ݿ�"),szBuf);
	return SQLERROR != nErrCode;
}
bool CUserDBase::UpdatePlayListSongInfo(std::vector<spSongInfoT> SongArray,unsigned int nPlayListID /*= 0*/)
{
	if(!this->BeginTransaction())
		return assert(false),false;
	for (auto it=SongArray.begin();it!=SongArray.end();++it)
	{
		this->UpdatePlayListSongInfo(*it,nPlayListID);
	}
	if(!this->Commit())
		return assert(false),false;
	return true;
}
/*���¸���·��*/
bool CUserDBase::UpdatePlayListSongPath(spSongInfoT spSongInfo, unsigned int nPlayListID/* = 0*/)
{
	assert(spSongInfo);
	if(!spSongInfo) return false;
	static const char sSql[] = "update %Q set local_path = %Q where song_local_id = %u;";
	char szBuf[MAX_PATH] = {0};
	sprintf_s(szBuf,MAX_PATH,"%s_%d",SongListTableName,nPlayListID);
	return SQLERROR != this->ExecSql(sSql,
		szBuf,
		TransToStdString(spSongInfo->GetLocalPath()).c_str(),
		spSongInfo->GetLocalId()
	) ;
}
/*�Ƴ�ָ�������б�ID�ĸ���*/
bool CUserDBase::RemovePlayListSongs(spSongInfoT spSongInfo, unsigned int nPlayListID/* = 0*/)
{
	assert(spSongInfo);
	if(!spSongInfo) return false;
	static const char sSql[] = "delete from %Q where song_local_id = %u;";
	char szBuf[MAX_PATH] = {0};
	sprintf_s(szBuf,MAX_PATH,"%s_%d",SongListTableName,nPlayListID);

	sLog(_T("[Begin]�Ƴ�����<table:%s>----���ݿ�"),szBuf);

	bool bRet = (this->ExecSql(
		sSql,
		szBuf,
		spSongInfo->GetLocalId()
		) != SQLERROR);

	sLog(_T("[End]�Ƴ�����<table:%s>----���ݿ�"),szBuf);

	return bRet;
}
bool CUserDBase::RemovePlayListSongs(std::vector<spSongInfoT> &SongList, unsigned int nPlayListID/* = 0*/)
{
	if(!this->BeginTransaction())
		return assert(false),false;

	for (std::vector<spSongInfoT>::iterator iter=SongList.begin();iter!=SongList.end();iter++)
	{
		this->RemovePlayListSongs(*iter,nPlayListID);
	}

	return this->Commit();
}
bool CUserDBase::RemoveAllPlayListSongs( unsigned int nPlayListID/* = 0*/)
{
	char szBuf[MAX_PATH] = {0};
	sprintf_s(szBuf,MAX_PATH,"%s_%d",SongListTableName,nPlayListID);
	static const char sSql[] = "delete from %Q;";
	
	return SQLERROR != this->ExecSql(sSql,szBuf);
}
bool CUserDBase::DeletePlayList(unsigned int nPlayListID /* = 0 */)
{
	char szBuf[MAX_PATH] = {0};
	sprintf_s(szBuf,MAX_PATH,"%s_%d",SongListTableName,nPlayListID);
		
	sLog(_T("[Begin]ɾ�������б�<table:%s>----���ݿ�"),szBuf);

	static const char sSql[] = "drop table %Q;";

	int nErrCode = this->ExecSql(sSql,szBuf);

	sLog(_T("[End]ɾ�������б�<table:%s>----���ݿ�"),szBuf);

	return SQLERROR != nErrCode;
}
bool CUserDBase::CreateFavTable()
{
	sLog(_T("[Begin]�����ղ��б�<table:%s>----���ݿ�"),FavoritesTableName);
	static const char sSql[] = "create table %Q (favorites_id Integer primary key autoincrement,"
		"favorites_name text ,type int,gmt_create int,playlist_id int,song_id int);";
	try
	{
		if(!this->IsExistTable(FavoritesTableName))
			this->ExecSql(sSql,FavoritesTableName);
	}
	catch(CppSQLite3Exception e)
	{
		sLogError(_T("[End]�����ղ��б�<table:%s>----���ݿ�"),FavoritesTableName);
		assert(false);
		return SQLERROR;
	}
	catch(...)
	{
		sLogError(_T("[End]�����ղ��б�<table:%s>----���ݿ�"),FavoritesTableName);
		assert(false);
		return SQLERROR;
	}
	sLog(_T("[End]�����ղ��б�<table:%s>----���ݿ�"),FavoritesTableName);

	return true;
}
bool CUserDBase::AddDefaultFav()
{
	sLog(_T("[Begin]�½��ҵ��ղ��б�<table:%s>----���ݿ�"),FavoritesTableName);
	if(IsPlayListExists(_T("�ҵ��ղ�")))
		return true;
	spPlayListInfoT spListInfo = spPlayListInfoT(new CPlayListInfo);
	assert(spListInfo);
	if(!spListInfo) return false;
	spListInfo->SetListType(CPlayListInfo::plt_sys);
	spListInfo->SetShow(true);
	spListInfo->SetComment(_T("�ҵ��ղ�"));
	spListInfo->SetName(_T("�ҵ��ղ�"));

	bool bRet = AddPlayList(spListInfo);
	if(!bRet)
		return	assert(false),sLogError(_T("[End]�½��ҵ��ղ��б�<table:%s>----���ݿ�"),FavoritesTableName), false;

	sLog(_T("[End]�½��ҵ��ղ��б�<table:%s>----���ݿ�"),FavoritesTableName);
	return bRet;
}
bool CUserDBase::AddFavorites(spFavInfoT spFavInfo)
{
	sLog(_T("[Begin]����ղظ���<table:%s>----���ݿ�"),FavoritesTableName);
	static const char sSql[] = "replace into  %Q (favorites_id,favorites_name,type,gmt_create) values (%u,%Q,%u,%u);";

	std::string sNameUTF8 = TransToStdString(spFavInfo->GetFavName());

	int nErrCode = this->ExecSql(sSql,FavoritesTableName,
		spFavInfo->GetFavId(),
		sNameUTF8.c_str(),
		(int)(spFavInfo->GetType()),
		(int) time(NULL));

	sLog(_T("[End]�½��ҵ��ղ��б�<table:%s>----���ݿ�"),FavoritesTableName);

	return SQLERROR != nErrCode;
}
bool CUserDBase::AddFavorites(std::vector<spFavInfoT> &FavList)
{
	if(!this->BeginTransaction())
		return assert(false),false;

	for (auto iter=FavList.begin();iter!=FavList.end();iter++)
	{
		this->AddFavorites(*iter);
	}

	return this->Commit();
}
bool CUserDBase::RemoveFavorites(unsigned int nId,unsigned int nType)
{
	static const char sSql[] = "delete from %Q where favorites_id = %u and type = %u;";

	return SQLERROR != this->ExecSql(sSql,FavoritesTableName,nId,nType);
}
bool CUserDBase::RemoveFavorites(std::vector<spFavInfoT> &FavList)
{
	if(!this->BeginTransaction())
		return assert(false),false;

	for (auto iter=FavList.begin();iter!=FavList.end();iter++)
	{
		this->RemoveFavorites((*iter)->GetFavId(),(*iter)->GetType());
	}

	return this->Commit();
}
bool CUserDBase::RemoveAllFavorites()
{
	static const char sSql[] = "delete from %Q ;";
	return SQLERROR != this->ExecSql(sSql,FavoritesTableName);
}
bool CUserDBase::GetAllFavorites(std::vector<spFavInfoT> &FavList)
{
	static const char sSql[] = "select favorites_id,favorites_name,type from %Q ;";

	SQLResult rs = this->QuerySql(sSql,FavoritesTableName);
	while(!rs.eof())
	{
		spFavInfoT spInfo(new CFavInfo);
		if(spInfo)
		{
			spInfo->SetFavId(rs.getIntField(0));
			spInfo->SetFavName(TransToDuiString(rs.getStringField(1)));
			spInfo->SetType((CFavInfo::FavType)(rs.getIntField(2)));
			FavList.push_back(spInfo);
		}			
		rs.nextRow();
	}

	return true;
}

bool CUserDBase::AddPlayDetailRecord(unsigned int nSongId,unsigned int nSecond,bool bSkip)
{
	static const char sSql[] = "insert into %Q (song_id,play_second,gmt_play_time,type) values (%u,%u,%u,%u);";

	bool bResult = (SQLERROR != this->ExecSql(sSql,DetailPlayRecordTableName,
		nSongId,nSecond,(int)time(NULL),
		bSkip ? 1 : 0));

	if (bResult)
	{
		//��record����޸�;
	}

	return bResult;
}

bool CUserDBase::GetRecentFavRecord(std::vector<spFavInfoT> &FavList)
{
	static const char sSql[] = "select favorites_id,favorites_name,type from %Q order by gmt_create desc limit 0,50;";

	SQLResult rs = this->QuerySql(sSql,FavoritesTableName);
	while (!rs.eof())
	{
		spFavInfoT spInfo(new CFavInfo);
		if(spInfo)
		{
			spInfo->SetFavId(rs.getIntField(0));
			spInfo->SetFavName(TransToDuiString(rs.getStringField(1)));
			spInfo->SetType((CFavInfo::FavType)(rs.getIntField(2)));
			FavList.push_back(spInfo);
		}			
		rs.nextRow();
	}
	return true;
}

bool CUserDBase::GetPlayRecord(std::vector<std::pair<int,int> >& DataArray,bool bSkipRecord)
{
	static const char sSql[] = "select song_id,count(song_id) as play_count from %Q where type = %u group by song_id ;";
		
	SQLResult rs = this->QuerySql(sSql,DetailPlayRecordTableName,bSkipRecord ? 1 :0);
	while(!rs.eof())
	{
		DataArray.push_back(std::pair<int,int>(rs.getIntField(0),rs.getIntField(1)));
		rs.nextRow();
	}

	return true;

}

bool CUserDBase::GetLocalMusicPacket(unsigned int nPlayListLocalId,std::vector<std::pair<UINT,UINT> >& PacketArray,UINT nStartPos,UINT nLimit,bool bSong)
{
	static const char sSqlSong[] = "select packet_id,type from %Q where play_list_local_id = %u and type=4 order by gmt_create desc limit %u ,%u;";
	static const char sSqlNotSong[] = "select packet_id,type from %Q where play_list_local_id = %u and type in (1,2,3) order by gmt_create desc limit %u ,%u;";

	SQLResult rs = this->QuerySql(bSong ? sSqlSong : sSqlNotSong,LocalMusicPacketTableName,nPlayListLocalId,nStartPos,nLimit);

	while(!rs.eof())
	{
		PacketArray.push_back(std::pair<int,int>(rs.getIntField(0),rs.getIntField(1)));
		rs.nextRow();
	}
	return true;
}

bool CUserDBase::AddMusicPacket(unsigned int nPlayListLocalId,const std::pair<UINT,UINT>& Packet)
{
	static const char sSql[] = "replace into %Q (play_list_local_id,packet_id,type,gmt_create) values (%u,%u,%u,%u);";

	bool bResult = (SQLERROR != this->ExecSql(sSql,LocalMusicPacketTableName,
		nPlayListLocalId,Packet.first,Packet.second,(int)time(NULL)));

	return bResult;

}

bool CUserDBase::AddMusicPacket(unsigned int nPlayListLocalId,const std::vector<std::pair<UINT,UINT> >& PacketArray)
{
	if(!this->BeginTransaction())
		return assert(false),false;

	for (auto iter=PacketArray.begin();iter!=PacketArray.end();iter++)
	{
		this->AddMusicPacket(nPlayListLocalId,*iter);
	}

	return this->Commit();
}

bool CUserDBase::RemoveMusicPacket(unsigned int nPlayListLocalId,const std::pair<UINT,UINT>& Packet)
{
	static const char sSql[] = "delete from %Q where play_list_local_id = %u and packet_id = %u and type = %u;";

	return SQLERROR != this->ExecSql(sSql,LocalMusicPacketTableName,nPlayListLocalId,Packet.first,Packet.second);
}

UINT CUserDBase::GetMusicPacketCount(unsigned int nPlayListLocalId,bool bSong)
{
	UINT nCount = 0;
	static const char sSqlSong[] = "select count(*) from %Q where play_list_local_id = %u and type = 4;";
	static const char sSqlPacket[] = "select count(*) from %Q where play_list_local_id = %u and type in (1,2,3);";
	SQLResult rs = this->QuerySql(bSong ? sSqlSong : sSqlPacket,LocalMusicPacketTableName,nPlayListLocalId);
	if (!rs.eof())
	{
		nCount = rs.getIntField(0);
	}
	return nCount;
}
//Operation�����;
bool CUserDBase::CreateOperationTable()
{
	sLog("[Begin]���������б�(%s)----���ݿ�",OperationTableName);
	static const char sSql[] = "create table %Q (auto_id Integer primary key autoincrement,"
		"object_id int,object_type int,operate_type int,gmt_create int,flag int);";
	try
	{
		if(!this->IsExistTable(OperationTableName))
			this->ExecSql(sSql,OperationTableName);
	}
	catch(CppSQLite3Exception e)
	{
		assert(false);
		return SQLERROR;
	}
	catch(...)
	{
		assert(false);
		return SQLERROR;
	}
	sLog("[End]���������б�(%s)----���ݿ�",OperationTableName);

	return true;
}
bool CUserDBase::GetAllOperation(std::vector<spSynOperationT>& OperationArray)
{
	OperationArray.clear();
	static const char sSql[] = "select * from %Q";

	SQLResult rs = this->QuerySql(sSql,OperationTableName);

	while(!rs.eof())
	{
		spSynOperationT spSynOperation(new CSynOperation);
		spSynOperation->SetAutoID(rs.getIntField(0));
		spSynOperation->SetObjectID(rs.getIntField(1));
		spSynOperation->SetObjectType((CSynOperation::ObjectType)rs.getIntField(2));
		spSynOperation->SetOperateType((CSynOperation::OperateType)rs.getIntField(3));
		spSynOperation->SetFlag(rs.getIntField(4));

		OperationArray.push_back(spSynOperation);
		rs.nextRow();
	}
	return true;

}

bool CUserDBase::AddOperation(spSynOperationT spSynOperation)
{
	static const char sSql[] = "insert into %Q (object_id,object_type,operate_type,gmt_create,flag) values (%u,%u,%u,%u,%u);";

	bool bResult = (SQLERROR != this->ExecSql(sSql,OperationTableName,spSynOperation->GetObjectID(),
		spSynOperation->GetObjectType(),spSynOperation->GetOperateType(),(int)time(NULL),spSynOperation->GetFlag()));

	if (bResult)
	{
		SQLResult rs = this->QuerySql("select last_insert_rowid() from %Q",OperationTableName);
		if(rs.eof())
			return false;
		spSynOperation->SetAutoID(rs.getIntField(0));
	}

	return bResult;
}

bool CUserDBase::DelOperation(spSynOperationT spSynOperation)
{
	static const char sSql[] = "delete from %Q where auto_id = %u;";
	return SQLERROR != this->ExecSql(sSql,OperationTableName,spSynOperation->GetAutoID());
}

bool CUserDBase::OnCreateTables()
{
	bool bRet1 = CreateTablePlayLists();//���������б�
	assert(bRet1);
	bool bRet2 = CreateFavTable();//�����ղ��б�
	assert(bRet2);
	bool bRet3 = CreateOperationTable();//���������б�
	assert(bRet3);
	return bRet1&&bRet2&&bRet3;
}
bool CUserDBase::OnUpdateTables(int oldVer,int newVer)
{
	return true;
}



