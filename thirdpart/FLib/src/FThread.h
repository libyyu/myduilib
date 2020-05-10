#ifndef __FLIB_HREAD_H__
#define __FLIB_HREAD_H__
#pragma once
#include <stdio.h>
#include <queue>
#include <functional>
#include <memory>
#include "FLock.h"
#include "FSemaphore.h"
#if FLIB_COMPILER_WINDOWS
#else
#include <pthread.h>
#endif

_FStdBegin

class F_DLL_API FThread
{
public:
    typedef std::function<void()> FThreadCallback;
    enum FStateT { kInit, kStart, kJoined, kStop };
	explicit FThread(const FThreadCallback& cb);

	virtual ~FThread();

	bool start();
    
	bool stop();

	bool join();

#if FLIB_COMPILER_WINDOWS
    ulong tid() const { return _threadId; }
    operator FLIB_HANDLE() { return _handle; }
#else
    pthread_t tid() const { return _thread; }
#endif

private:
    FThread(const FThread&){}
    void operator=(const FThread&){}

#if FLIB_COMPILER_WINDOWS
	static ulong _STDCALL threadProc(void* param);
#else
	static void* threadProc(void* param);
#endif

    FThreadCallback _cb;
    FStateT _state;

#if FLIB_COMPILER_WINDOWS
	FLIB_HANDLE _handle;
	ulong _threadId;
#else
    pthread_t _thread;
#endif
};

class F_DLL_API FThreadGroup
{
    struct _FThreadNode
    {
        FThread* _pThread;
        bool _isHeap;
    };
public:
	FThreadGroup();
	~FThreadGroup();

	void dispose();
    
	FThread* createThread(const FThread::FThreadCallback& threadfunc);
    
	void addThread(FThread* thread, bool isHeap = false);
    
	void startAll();
    
	void joinAll();
    
	void stopAll();
    
    size_t size() const { return _threads.size(); }  
private:
    FThreadGroup(const FThreadGroup&){}
    void operator=(const FThreadGroup&){}

    std::vector<_FThreadNode> _threads;
    FLock _lock;
};


class F_DLL_API FThreadPool
{
public:
	FThreadPool();
	~FThreadPool();
	
	bool init(int threadNum = kDefaultThreadNum);
	
	void addTask(const FThread::FThreadCallback& task);
	
	void join();
	
	void terminate();

	size_t size() const { return _threads.size(); }
	
private:
	FThreadPool(const FThreadPool&){}
    void operator=(const FThreadPool&){}
	typedef std::vector<std::shared_ptr<FThread> > _FThreadVector;
	
	class _FTaskQueue
	{
	public:
		_FTaskQueue();
		~_FTaskQueue();
		
		void put(const FThread::FThreadCallback& task);
		FThread::FThreadCallback get();
		
	private:
		_FTaskQueue(const _FTaskQueue&){}
    	void operator=(const _FTaskQueue&){}
		typedef std::queue<FThread::FThreadCallback> _FTasks;
		_FTasks _tasks;
		FLock _mutex;
		FSemaphore _sem;
	};
	
	// 添加工作线程
	void addWorker(int threadNum);
	
	// 运行任务
	void taskRunner();
	
	bool _inited;
	_FThreadVector _threads;
	_FTaskQueue _tasks;
	FLock _lock;

	static const int32 kDefaultThreadNum = 4;
};

class F_DLL_API FAsync
{
public:
	explicit FAsync(const std::function<void()>& action);
	~FAsync();
protected:
    FThread* _thread;
};
std::function<void()> F_DLL_API AsyncCallback(const std::function<void()>& action);

_FStdEnd


#endif//__FLIB_HREAD_H__
