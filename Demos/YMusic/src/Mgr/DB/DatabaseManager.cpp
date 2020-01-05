#include "stdafx.h"

#include "DatabaseManager.h"
#include "YApp.h"
#include "YMusic.h"
#include "Utility/Convert.h"
const TCHAR* CDatabaseManager::USER_DB = _T("user_%s.sl");
const TCHAR* CDatabaseManager::DEFAULT_DB = _T("default.sl");
const TCHAR* CDatabaseManager::GLOBEL_DB = _T("globel.sl");

CDatabaseManager::CDatabaseManager()
{

}

CDatabaseManager::~CDatabaseManager()
{

}

void CDatabaseManager::SetDatabasePath(const DuiLib::CDuiString& sDatabasePath)
{
	if (sDatabasePath.GetLength() == 0)
		return;
	m_sDatabasePath = sDatabasePath;
	m_sDatabasePath.Replace(_T('\\'), _T('/'));

	if (_T("/") != m_sDatabasePath.Right(1))
	{
		m_sDatabasePath += _T("/");
	}
}

DuiLib::CDuiString& CDatabaseManager::GetDatabasePath()
{
	return m_sDatabasePath;
}

bool CDatabaseManager::SetCurrentUser(spLogonUserInfoT spUserInfo)
{
	m_spUserInfo = spUserInfo;
	m_spUserDBase = spUserDBaseT(NULL);
	return true;
}
spGlobalDBaseT CDatabaseManager::GetGlobalDBase()
{
	if (m_spGlobalDBase)
		return m_spGlobalDBase;
	sLog(_T("开始创建全局数据库----数据库"));
	if (!m_spGlobalDBase && m_sDatabasePath.GetLength() > 0)
	{
		DuiLib::CDuiString sDBFileName = theFilePathMgr->GetDatabasePath() + GLOBEL_DB;
		if (!em_utility::mci::file_exist(sDBFileName))
		{

		}
		spGlobalDBaseT spGlobalDBase(new CGlobalDBase());
		if (spGlobalDBase && spGlobalDBase->OpenDB(TransToStdString(sDBFileName).c_str()))
		{
			m_spGlobalDBase = spGlobalDBase;
		}
	}
	sLog(_T("结束创建全局数据库----数据库"));
	return m_spGlobalDBase;
}
spUserDBaseT CDatabaseManager::GetUserDBase()
{
	if (m_spUserDBase)
		return m_spUserDBase;
	sLog(_T("开始创建用户列表----数据库"));
	if (!m_spUserDBase && m_spUserInfo && m_sDatabasePath.GetLength() > 0)
	{
		DuiLib::CDuiString sDBFileName = theFilePathMgr->GetDatabasePath();
		DuiLib::CDuiString sName;
		sName.Format(USER_DB, m_spUserInfo->sUserID);
		sDBFileName += sName;

		if (!em_utility::mci::file_exist(sDBFileName))
		{

		}
		spUserDBaseT spUserDBase(new CUserDBase());
		if (spUserDBase && spUserDBase->OpenDB(TransToStdString(sDBFileName).c_str()))
		{
			m_spUserDBase = spUserDBase;
		}
	}
	sLog(_T("结束创建用户列表----数据库"));
	return m_spUserDBase;
}

spUserDBaseT CDatabaseManager::GetDefaultUserDBase()
{
	if (m_spDefaultUserDBase)
		return m_spDefaultUserDBase;
	sLog(_T("开始创建默认列表----数据库"));
	if (!m_spDefaultUserDBase && m_sDatabasePath.GetLength() > 0)
	{
		DuiLib::CDuiString sDBFileName = theFilePathMgr->GetDatabasePath() + DEFAULT_DB;

		if (!em_utility::mci::file_exist(sDBFileName))
		{

		}
		spUserDBaseT spUserDBase(new CUserDBase());
		if (spUserDBase && spUserDBase->OpenDB(TransToStdString(sDBFileName).c_str()))
		{
			m_spDefaultUserDBase = spUserDBase;
		}
	}
	sLog(_T("结束创建默认列表----数据库"));
	return m_spDefaultUserDBase;
}
spUserDBaseT CDatabaseManager::GetUserDB()
{
	if (m_spUserInfo)
	{
		return GetUserDBase();
	}
	return GetDefaultUserDBase();
}