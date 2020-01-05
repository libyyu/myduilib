#include "stdafx.h"
#include "Lock.h"

namespace DuiLib
{
	struct CDuiLock::critical_impl
	{
		CRITICAL_SECTION* mp_cs;
	};
	CDuiLock::CDuiLock()
	{
		mp_impl = new critical_impl;
		mp_impl->mp_cs = new CRITICAL_SECTION();
		assert(mp_impl->mp_cs != NULL);
		::InitializeCriticalSection(mp_impl->mp_cs);
	}
	CDuiLock::~CDuiLock()
	{
		::DeleteCriticalSection(mp_impl->mp_cs);
		delete mp_impl->mp_cs;
		mp_impl->mp_cs = NULL;
		delete mp_impl;
		mp_impl = NULL;
	}
	bool CDuiLock::Lock()
	{
		if (mp_impl != NULL && mp_impl->mp_cs != NULL)
		{
			::EnterCriticalSection(mp_impl->mp_cs);
			return true;
		}
		return false;
	}
	bool CDuiLock::Unlock()
	{
		if (mp_impl != NULL && mp_impl->mp_cs != NULL)
		{			
			::LeaveCriticalSection(mp_impl->mp_cs);
		}
		return true;
	}

	
	CDuiAutoLocker::CDuiAutoLocker(CDuiLock* pLock):m_pLock(NULL)
	{
		if (pLock)
		{
			m_pLock = pLock;
			m_pLock->Lock();
		}
	}

	CDuiAutoLocker::~CDuiAutoLocker()
	{
		if(m_pLock) m_pLock->Unlock();
	}
}