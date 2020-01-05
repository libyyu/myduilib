#define FLIB_DLL
#include "FDReadAWriteLock.h"
#include "FFunc.h"

_FStdBegin

FDReadAWriteLock::FDReadAWriteLock(void)
{
	m_Lock.nReadCount = 0;
	m_Lock.bWriteFlag = false;
}
FDReadAWriteLock::~FDReadAWriteLock(void)
{
	m_Lock.lock.lock();
	m_Lock.lock.unlock();
}

void FDReadAWriteLock::EnterWrite()
{
    while(1)
    {
        m_Lock.lock.lock();
        if ( !m_Lock.bWriteFlag )
        {
            m_Lock.bWriteFlag = true;
            m_Lock.lock.unlock();
            goto _Start_Write;
        }
        m_Lock.lock.unlock();
        FSleep(0/*1*/);
    }
_Start_Write:
    while(GetReadCount())
    {
        FSleep(/*1*/0);
    }
}

void FDReadAWriteLock::LeaveWrite()
{
    m_Lock.lock.lock();
    m_Lock.bWriteFlag = false;
    m_Lock.lock.unlock();
}

int FDReadAWriteLock::EnterRead()
{
    int n = 0;
    while(1)
    {
        m_Lock.lock.lock();
        if (!m_Lock.bWriteFlag)
        {		
            n = ++m_Lock.nReadCount;
            goto _Start_Read;
        }
        m_Lock.lock.unlock();
        FSleep(0/*1*/);
    }
_Start_Read:
    m_Lock.lock.unlock();
    return n;
}

int FDReadAWriteLock::LeaveRead()
{
    int n = 0;
    m_Lock.lock.lock();
    if ( 0 < m_Lock.nReadCount )
    {
        n = --m_Lock.nReadCount;
    }
    m_Lock.lock.unlock();
    return n;
}

int FDReadAWriteLock::GetReadCount()
{
    int n = 0;
    m_Lock.lock.lock();
    n = m_Lock.nReadCount;
    m_Lock.lock.unlock();
    return n;
};
bool FDReadAWriteLock::IsWrite()
{
    bool bVar = false;
    m_Lock.lock.lock();
    bVar = m_Lock.bWriteFlag;
    m_Lock.lock.unlock();
    return bVar;
}

_FStdEnd