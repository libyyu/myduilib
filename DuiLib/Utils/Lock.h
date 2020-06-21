#ifndef __LOCK_H__
#define __LOCK_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CDuiLock
	{
	public:
		CDuiLock();
		~CDuiLock();
		bool Lock();
		bool Unlock();
	private:
		struct critical_impl;
		critical_impl* mp_impl;
	};

	class UILIB_API CDuiAutoLocker
	{
	public:
		explicit CDuiAutoLocker(CDuiLock* pLock);
		~CDuiAutoLocker();
	private:
		CDuiLock * m_pLock;
	};
}

#endif//__LOCK_H__