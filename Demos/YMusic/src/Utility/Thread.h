#ifndef __THREAD_MANAGER_H__
#define __THREAD_MANAGER_H__

#define CLASS_UNCOPYABLE(classname) \
		private: \
		classname(const classname&); \
		classname& operator=(const classname&);

class CThread 
{
	CLASS_UNCOPYABLE(CThread)
public:
	CThread();
	virtual ~CThread();
	virtual void Start();
	virtual unsigned int Run(void*);

private:
	void* m_hThread;
	static unsigned int __stdcall ThreadProc( void* pArgs);

};
#endif//__THREAD_MANAGER_H__