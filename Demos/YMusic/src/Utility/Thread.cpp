#include "StdAfx.h"
#include "Thread.h"
#include <process.h>
#include <Windows.h>

CThread::CThread(): m_hThread(NULL)
{

}


CThread::~CThread()
{
	if(m_hThread)
	{
		::CloseHandle((HANDLE)m_hThread);
		m_hThread = NULL;
	}
}


unsigned int CThread::ThreadProc( void* pArgs)
{

	CThread* pthread = (CThread*)pArgs;
	pthread->Run(NULL);

	//<By Jackie 2013-6-8>���߳���, ����heap�������, ����������delete��.
	// �������Ϊ�ֲ�����, ��ô�ͻ������ǰ���������.
	delete pthread;
	return 0;
}

void CThread::Start()
{
	assert(m_hThread == NULL);
	unsigned int threadId = 0;
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, this, 0, &threadId);
	
}

unsigned int CThread::Run(void*)
{
	//<By Jackie 2013-6-8>�ڼ̳�������չ�������.
	return 0;
}
