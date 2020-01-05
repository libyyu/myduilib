#define FLIB_DLL
#include "FCounter.h"
_FStdBegin

FCounter::FCounter() :m_i(0) {}
FCounter::~FCounter() {}

unsigned int  FCounter::Set(const unsigned int& n)
{
	unsigned int nResult = n;
	m_Lock.lock();
	m_i = n;
	m_Lock.unlock();
	return nResult;
}
unsigned int  FCounter::Add()
{
	unsigned int nResult = 0;
	m_Lock.lock();
	nResult = ++m_i;
	m_Lock.unlock();
	return nResult;
}
unsigned int  FCounter::Dec()
{
	unsigned int nResult = 0;
	m_Lock.lock();
	nResult = --m_i;
	m_Lock.unlock();
	return nResult;
}
unsigned int  FCounter::GetCount()
{
	unsigned int nResult = 0;
	m_Lock.lock();
	nResult = m_i;
	m_Lock.unlock();
	return nResult;
}

_FStdEnd

