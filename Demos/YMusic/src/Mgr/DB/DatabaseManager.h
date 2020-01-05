#ifndef _DatabaseManager_H_
#define _DatabaseManager_H_
#pragma once
#include <vector>
#include "DB.h"

class CDatabaseManager
{
	static const TCHAR * USER_DB; 
	static const TCHAR * DEFAULT_DB; 
	static const TCHAR * GLOBEL_DB; 
public:
	CDatabaseManager();
	virtual ~CDatabaseManager();
public:
	void SetDatabasePath(const DuiLib::CDuiString &sDatabasePath);
	DuiLib::CDuiString &GetDatabasePath();

	bool SetCurrentUser(spLogonUserInfoT spUserInfo);

	spGlobalDBaseT GetGlobalDBase();
	spUserDBaseT GetUserDBase();
	spUserDBaseT GetDefaultUserDBase();
	spUserDBaseT GetUserDB();
private:

	spLogonUserInfoT m_spUserInfo; 
	DuiLib::CDuiString m_sDatabasePath;

	spGlobalDBaseT  m_spGlobalDBase;
	spUserDBaseT	m_spUserDBase;
	spUserDBaseT	m_spDefaultUserDBase;

};
	
#endif//_DatabaseManager_H_
