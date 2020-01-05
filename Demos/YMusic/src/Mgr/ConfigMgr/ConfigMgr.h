#pragma once
#include "ConfigBase.h"

class CConfigMgr
{
public:
	typedef std::shared_ptr<CNormalConfig>  spNormalConfigT;
	typedef std::shared_ptr<CAlertConfig>   spAlertConfigT;
	typedef std::shared_ptr<CHotKeyConfig>  spHotKeyConfigT;
	typedef std::shared_ptr<CCacheConfig>   spCacheConfigT;
	typedef std::shared_ptr<CSkinConfig>    spSkinConfigT;
	typedef std::shared_ptr<CUpdateConfig>  spUpdateConfigT;
	typedef std::shared_ptr<CLrcConfig>     spLrcConfigT;
public:
	CConfigMgr();
	~CConfigMgr();
	void  Load();
	spNormalConfigT  GetNormalConfig();
	spAlertConfigT   GetAlertConfig();
	spHotKeyConfigT  GetHotKeyConfig();
	spCacheConfigT   GetCacheConfig();
	spSkinConfigT    GetSkinConfig();
	spUpdateConfigT  GetUpdateConfig();
	spLrcConfigT     GetLrcConfig();
private:
	spNormalConfigT   m_pNormal;
	spAlertConfigT    m_pAlert;
	spHotKeyConfigT   m_pHotKey;
	spCacheConfigT    m_pCache;
	spSkinConfigT     m_pSkin;
	spUpdateConfigT   m_pUpdate;
	spLrcConfigT      m_pLrc;
};
