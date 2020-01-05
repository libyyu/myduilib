
#pragma once

#include "Utility/log.h"
#include "Mgr/Mgr.h"

class CRuntimeState;
class CUserContext;
typedef std::shared_ptr<CRuntimeState> spRuntimeStateT;
typedef std::shared_ptr<CUserContext>  spUserContextT;


/************************************************************************/
/* 控制运行时的各种变量                                                 */
/************************************************************************/

class CUserContext
{
public:
	~CUserContext();
	bool IsValid() const;
	spRuntimeStateT  GetRuntimeState() const;
	spLogonUserInfoT GetUser() const;
	DuiLib::CDuiString GetUserId() const;
	bool IsUserLogined() const;
	spUserDBaseT GetUserDB() const;
	bool IsMainThread() const;
	bool IsSameContext(spUserContextT& ctx) const;
	bool IsCurrentContext() const;

	void SetTag(int tag);
	int GetTag() const;
private:
	friend class CRuntimeState;
	explicit CUserContext(spRuntimeStateT runtime,spLogonUserInfoT user,spUserDBaseT userdb,LPCTSTR sFlag = NULL);
private:
	spRuntimeStateT   _runtimeState;
	spLogonUserInfoT  _user;
	spUserDBaseT      _userDB;
	int               _tag;
};

class CRuntimeState 
{
public:
	typedef std::shared_ptr<CCacheMgr>                  spCacheMgrT;
	typedef std::shared_ptr<CDatabaseManager>           spDbMgrT;
	typedef std::shared_ptr<CFilePathMgr>               spFilePathMgrT;
	typedef std::shared_ptr<CLog>						spLogMgrT;
//	typedef std::shared_ptr<CPlayCenter>                spPlayerCenterT;//以后替换为AVPlayer
	typedef std::shared_ptr<CAVPlayer>                  spPlayerT;
	typedef std::shared_ptr<PlayListMgr>                spPlayListMgrT;
//	typedef std::shared_ptr<CWinNotifyCenter>           spWinNotifyCenterT;
	typedef std::shared_ptr<CConfigMgr>                 spConfigMgrT;
	typedef std::shared_ptr<CWebServiceMgr>             spWebServiceMgrT;
	typedef std::shared_ptr<em_utility::down_http_file> spHttpDownerT;
//	typedef std::shared_ptr<CFavMgr>                    spFavMgrT;
	typedef std::shared_ptr<CLrcMgr>                    spLrcMgrT;

public:
	virtual ~CRuntimeState();
	void Init(DWORD dwMainThreadId);
	void UnInit();
public:
	bool IsMainThread() const;
	bool IsOnline() const;
	spUserContextT GetCurrentContext(LPCTSTR sflag = NULL);
	void SetUserInfo(spLogonUserInfoT user);

	spLogonUserInfoT GetLogUserInfo() const;
	spCacheMgrT GetCacheMgr() const;
	spFilePathMgrT GetFilePathMgr() const;
	spLogMgrT    GetLogMgr() const;
	spDbMgrT GetDbMgr() const;
//	spWinNotifyCenterT GetNotifyCenter() const;
	spConfigMgrT GetConfigMgr() const;
	spWebServiceMgrT GetWebServiceMgr() const;
//	spPlayerCenterT GetPlayerCenter() const;
	spPlayerT GetPlayer() const;
	spPlayListMgrT GetPlayListMgr() const;
	spHttpDownerT GetHttpDonwer() const;
//	spFavMgrT GetFavMgr()const;
	spLrcMgrT GetLrcMgr()const;
protected:
	bool                     _isOnline;

	spCacheMgrT              _spCacheMgr;
	spFilePathMgrT           _spFilePathMgr;
	spLogMgrT                _spLogMgr;
	spDbMgrT                 _spDbMgr;
//	spWinNotifyCenterT       _spWinNotifyCenter;
	spLogonUserInfoT         _spUserInfo;
	spConfigMgrT             _spConfigMgr;
	spWebServiceMgrT         _spWebServiceMgr;
//	spPlayerCenterT          _spPlayerCenter;//以后替换
	spPlayerT                _spPlayer;
	spPlayListMgrT			 _spPlayListMgr;
	spHttpDownerT            _spHttpDowner;
//	spFavMgrT                _spFavMgr;
	spLrcMgrT                _spLrcMgr;

};
