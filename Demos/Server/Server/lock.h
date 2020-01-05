/********************************************************************
    FileName :  Lock.h   
    Version  :  0.10
    Date     :	2010-1-26 18:50:16
    Author   :  С�һ�
    Comment  :  ������

*********************************************************************/
#ifndef  __SLOCK_H__
#define  __SLOCK_H__
#pragma once
//#include <windows.h>
class  CLock
{
public:
	typedef CRITICAL_SECTION    LOCK;
	CLock(void);
	
	virtual ~CLock(void);
	
private:
	LOCK m_lock;
public:
	void Lock();
	
	void UnLock();

};

#endif//__SLOCK_H__

