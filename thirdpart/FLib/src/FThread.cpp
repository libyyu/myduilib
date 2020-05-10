#define FLIB_DLL
#include "FThread.h"
#if FLIB_COMPILER_WINDOWS
#include <Windows.h>
#endif
_FStdBegin

FThread::FThread(const FThread::FThreadCallback& cb)
	: _cb(cb)
	, _state(kInit)
#if FLIB_COMPILER_WINDOWS
	, _handle(NULL)
	, _threadId(0)
#endif
{}

FThread::~FThread()
{
	join();
	_state = kStop;
}

bool FThread::start()
{
	if (kInit != _state)
	{
		return false;
	}

	bool result = false;
#if FLIB_COMPILER_WINDOWS
	_handle = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadProc, (LPVOID)this, 0, &_threadId);
	result = (NULL != _handle);
#else
	int ret = pthread_create(&_thread, NULL, threadProc, (void*)this);
	result = (0 == ret);
#endif
	_state = kStart;
	return result;
}

bool FThread::stop()
{
	if (kStop == _state || kInit == _state)
	{
		return true;
	}

	bool result = true;
#if FLIB_COMPILER_WINDOWS
	if (0 == ::TerminateThread(_handle, 0))
	{
		result = false;
	}
#elif FLIB_COMPILER_ANDROID
	if (0 != pthread_kill(_thread, 0))
	{
		result = false;
	}
#else
	if (0 != pthread_cancel(_thread))
	{
		result = false;
	}
#endif
	if (result)
	{
		_state = kStop;
	}

	return result;
}

bool FThread::join()
{
	if (kStart != _state)
	{
		return false;
	}
	bool result = false;
#if FLIB_COMPILER_MSVC || FLIB_COMPILER_CYGWIN
	if (NULL != _handle)
	{
		DWORD ret = ::WaitForSingleObject(_handle, INFINITE);
		if (WAIT_OBJECT_0 == ret || WAIT_ABANDONED == ret)
		{
			result = true;
			_handle = NULL;
		}
	}
#else
	int ret = pthread_join(_thread, NULL);
	if (0 == ret)
	{
		result = true;
	}
#endif
	_state = kJoined;
	return result;
}

#if FLIB_COMPILER_WINDOWS
ulong _STDCALL FThread::threadProc(void* param)
#else
void* FThread::threadProc(void* param)
#endif
{
	FThread* pThis = reinterpret_cast<FThread*>(param);
	pThis->_cb();
	return 0;
}
////////////////////////////////////////////////////
////
FThreadGroup::FThreadGroup()
{}
FThreadGroup::~FThreadGroup()
{
	joinAll();
	dispose();
}

void FThreadGroup::dispose()
{
	for (size_t i = 0; i < _threads.size(); ++i)
	{
		if (_threads[i]._isHeap)
			delete _threads[i]._pThread;
	}
	_threads.clear();
}

FThread* FThreadGroup::createThread(const FThread::FThreadCallback& threadfunc)
{
	FThread* thread = new FThread(threadfunc);
	addThread(thread, true);
	return thread;
}

void FThreadGroup::addThread(FThread* thread, bool isHeap/* = false*/)
{
	_FThreadNode node;
	node._isHeap = isHeap;
	node._pThread = thread;
	_lock.lock();
	_threads.push_back(node);
	_lock.unlock();
}

void FThreadGroup::startAll()
{
	for (size_t i = 0; i < _threads.size(); ++i)
	{
		_threads[i]._pThread->start();
	}
}

void FThreadGroup::joinAll()
{
	for (size_t i = 0; i < _threads.size(); ++i)
	{
		_threads[i]._pThread->join();
	}
}

void FThreadGroup::stopAll()
{
	for (size_t i = 0; i < _threads.size(); ++i)
	{
		_threads[i]._pThread->stop();
	}
}
////////////////////////////////////////////////////
////
FThreadPool::FThreadPool()
	: _inited(false)
{}
FThreadPool::~FThreadPool()
{}

bool FThreadPool::init(int threadNum /*= kDefaultThreadNum*/)
{
	if (_inited)
	{
		return false;
	}
	_inited = true;
	addWorker(threadNum);
	return _inited;
}

void FThreadPool::addTask(const FThread::FThreadCallback& task)
{
	_tasks.put(task);
}

void FThreadPool::join()
{
	for (_FThreadVector::const_iterator it = _threads.begin(); it != _threads.end(); ++it)
	{
		(*it)->join();
	}

	_threads.clear();
}

void FThreadPool::terminate()
{
	for (_FThreadVector::const_iterator it = _threads.begin(); it != _threads.end(); ++it)
	{
		(*it)->stop();
	}
}

FThreadPool::_FTaskQueue::_FTaskQueue() {}
FThreadPool::_FTaskQueue::~_FTaskQueue() {}

void FThreadPool::_FTaskQueue::put(const FThread::FThreadCallback& task)
{
	lock_wrapper lock(&_mutex);
	_tasks.push(task);
	_sem.signal();
}
FThread::FThreadCallback FThreadPool::_FTaskQueue::get()
{
	FThread::FThreadCallback task;
	_sem.wait(FSemaphore::kInfinite);
	lock_wrapper lock(&_mutex);
	task = _tasks.front();
	_tasks.pop();
	return task;
}

// 添加工作线程
void FThreadPool::addWorker(int threadNum)
{
	_lock.lock();
	for (int i = 0; i < threadNum; ++i)
	{
		FThread::FThreadCallback _run = std::bind(&FThreadPool::taskRunner, this);
		std::shared_ptr<FThread> thread(new FThread(_run));
		_threads.push_back(thread);
		thread->start();
	}
	_lock.unlock();
}

// 运行任务
void FThreadPool::taskRunner()
{
	while (true)
	{
		FThread::FThreadCallback task = _tasks.get();
		task();
	}
}
////////////////////////////////////////////////////
////
FAsync::FAsync(const std::function<void()>& action) : _thread(nullptr)
{
	_thread = new FThread(action);
	_thread->start();
}
FAsync::~FAsync()
{
	_thread->join();
	delete _thread;
}

std::function<void()> AsyncCallback(const std::function<void()>& action)
{
	auto func = [action]()
	{
		auto s = new FAsync(action);
		delete s;
	};
	return func;
}

_FStdEnd

