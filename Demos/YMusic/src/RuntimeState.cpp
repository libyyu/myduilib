#include <stdafx.h>
#include "RunTimeState.h"
#include "Marco.hpp"
#include "YApp.h"
CUserContext::CUserContext(spRuntimeStateT runtime,spLogonUserInfoT user,spUserDBaseT userdb,LPCTSTR sFlag /* = NULL */)
	:_runtimeState(runtime),_user(user),_userDB(userdb),_tag(0)
{
	ASSERT(IsValid());
}
CUserContext::~CUserContext()
{
	
}
bool CUserContext::IsValid() const
{
	return (_runtimeState && _userDB);
}
spRuntimeStateT  CUserContext::GetRuntimeState() const
{
	return _runtimeState;
}
spLogonUserInfoT CUserContext::GetUser() const
{
	return _user;
}
DuiLib::CDuiString CUserContext::GetUserId() const
{
	spLogonUserInfoT theUserInfo = GetUser();
	return (theUserInfo ? theUserInfo->sUserID : _T(""));
}
bool CUserContext::IsUserLogined() const
{
	return !!_user;
}
spUserDBaseT CUserContext::GetUserDB() const
{
	return _userDB;
}
bool CUserContext::IsMainThread() const
{
	return GetRuntimeState()->IsMainThread();
}

bool CUserContext::IsSameContext(spUserContextT& ctx) const
{
	ASSERT(ctx);
	if(!ctx)
		return false;
	return 
		(ctx->GetRuntimeState() == this->GetRuntimeState()) &&
		(ctx->GetUser() == this->GetUser()) &&
		(ctx->GetUserDB() == this->GetUserDB());
}
bool CUserContext::IsCurrentContext() const
{
	return IsSameContext(this->GetRuntimeState()->GetCurrentContext(NULL));
}
void CUserContext::SetTag(int tag)
{
	_tag = tag;
}
int CUserContext::GetTag() const
{
	return _tag;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/

CRuntimeState::~CRuntimeState()
{
	UnInit();
}
void CRuntimeState::Init(DWORD dwMainThreadId)
{
	//��ʼδ��¼
	_isOnline = false;
	//�ļ�������
	_spFilePathMgr = spFilePathMgrT(new CFilePathMgr());
	//���������
	_spCacheMgr = spCacheMgrT(new CCacheMgr());
	_spCacheMgr->Init(_spFilePathMgr->GetDefaultCachePath());
	//��־������
	DuiLib::CDuiString strLogFileName = _spFilePathMgr->GetLogPath();
	strLogFileName += _T("_YMusic.log");
	_spLogMgr = spLogMgrT(new CLog(strLogFileName.GetData()));
	assert(_spLogMgr->isValid());
	//���ݿ������
	_spDbMgr = spDbMgrT(new CDatabaseManager());
	assert(_spDbMgr);
	_spDbMgr->SetDatabasePath(_spFilePathMgr->GetDatabasePath());
	_spDbMgr->GetGlobalDBase();	
	_spDbMgr->SetCurrentUser(_spUserInfo);
	_spDbMgr->GetUserDB();
	
	//���������
	_spWebServiceMgr = spWebServiceMgrT(new CWebServiceMgr());
	_spWebServiceMgr->SetNotifyMsg(WM_WEB_SERVICE);
	//��������
	_spPlayer = spPlayerT(new CAVPlayer());
	assert(_spPlayer);
	//�����б������
	_spPlayListMgr = spPlayListMgrT(new PlayListMgr());
	assert(_spPlayListMgr);
	_spPlayListMgr->LoadAllPlayList();
	//���ù�����
	_spConfigMgr = spConfigMgrT(new CConfigMgr);
	assert(_spConfigMgr);
	_spConfigMgr->Load();
	//��ʹ�����
	_spLrcMgr = spLrcMgrT(new CLrcMgr);
	assert(_spLrcMgr);
	//�ļ����ع�������
	_spHttpDowner = spHttpDownerT(new em_utility::down_http_file);
	assert(_spHttpDowner);
	_spHttpDowner->start_thread();
	//��ʼ��libcurl����
	HttpManagerProto::Instance()->CurlGlobalInit();
////	StreamThread::Instance()->Startup();
}
void CRuntimeState::UnInit()
{
	_spHttpDowner->stop_thread();
	HttpManagerProto::Instance()->CurlGlobalCleanup();
//	StreamThread::Instance()->Shutdown();
}
bool CRuntimeState::IsMainThread() const
{
	return true;
}
bool CRuntimeState::IsOnline() const
{
	return _isOnline;
}

CRuntimeState::spCacheMgrT CRuntimeState::GetCacheMgr() const
{
	return _spCacheMgr;
}

CRuntimeState::spFilePathMgrT CRuntimeState::GetFilePathMgr() const
{
	return _spFilePathMgr;
}

CRuntimeState::spLogMgrT CRuntimeState::GetLogMgr() const
{
	return _spLogMgr;
}
CRuntimeState::spPlayerT CRuntimeState::GetPlayer() const
{
	return _spPlayer;
}
CRuntimeState::spPlayListMgrT CRuntimeState::GetPlayListMgr() const
{
	return _spPlayListMgr;
}

CRuntimeState::spDbMgrT CRuntimeState::GetDbMgr() const
{
	return _spDbMgr;
}
spUserContextT CRuntimeState::GetCurrentContext(LPCTSTR sflag/* = NULL*/)
{
	spLogonUserInfoT user(_spUserInfo);
	spUserDBaseT userdb(GetDbMgr()->GetUserDB());

	return spUserContextT(new CUserContext(theRuntimeState,user,userdb));
}
spLogonUserInfoT CRuntimeState::GetLogUserInfo() const
{
	return _spUserInfo;
}
void CRuntimeState::SetUserInfo(spLogonUserInfoT user)
{
	if(_spUserInfo != user)
	{
		if(_spUserInfo) //ע��
		{
			//_spUserInfo->bKeepLogged = false;
			sLog(_T("ע��"));
			sLog(_T("theDbMgr->GetGlobalDBase():: %x"),theDbMgr->GetGlobalDBase());
			ASSERT(theDbMgr->GetGlobalDBase());
			ASSERT(theDbMgr->GetGlobalDBase()->UpdateUserInfo(_spUserInfo));
		}

		theDbMgr->SetCurrentUser(user);
		_spUserInfo = user;
		theDbMgr->GetUserDBase();
		if(_spUserInfo)
			ASSERT(theDbMgr->GetGlobalDBase()->UpdateLastUserInfo(_spUserInfo));
		assert(_spPlayListMgr);
		_spPlayListMgr->LoadAllPlayList();
		_spConfigMgr->Load();
		PostMessage(theMainWin->GetHWND(),WM_LOGIN,(unsigned long)&user,0);
	}
}

CRuntimeState::spConfigMgrT CRuntimeState::GetConfigMgr() const
{
	return _spConfigMgr;
}

CRuntimeState::spLrcMgrT CRuntimeState::GetLrcMgr() const
{
	return _spLrcMgr;
}

CRuntimeState::spWebServiceMgrT CRuntimeState::GetWebServiceMgr() const
{
	return _spWebServiceMgr;
}

CRuntimeState::spHttpDownerT CRuntimeState::GetHttpDonwer() const
{
	return _spHttpDowner;
}