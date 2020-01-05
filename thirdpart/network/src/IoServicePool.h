#pragma once
#include "std.h"

class IoServicePool :
	public boost::noncopyable
{
public:
	explicit IoServicePool(size_t iocnt,size_t threadCntPreIo):_next(0)
	{
		if(0 == iocnt && 0 == threadCntPreIo)
		{
			iocnt = 1;
			threadCntPreIo = boost::thread::hardware_concurrency()*2;
		}
		iocnt = std::max<size_t>(iocnt,1);
		threadCntPreIo = std::max<size_t>(threadCntPreIo,1);
		_threadCntPreIo = threadCntPreIo;
		for (size_t i = 0;i<iocnt;++i)
		{
			spioSrvT theIo(new ioSrvT());
			spworkT  theWork(new workT(*theIo));
			_iosrvs.push_back(theIo);
			_works.push_back(theWork);
		}
	}
	~IoServicePool(){};

	void run(bool asyn = false)
	{
		assert(_threadCntPreIo);
		assert(_iosrvs.size());
		std::vector<spthreadT> threads;
		for (size_t i = 0;i<_iosrvs.size();++i)
		{
			for (size_t n = 0;n<_threadCntPreIo;++n)
			{
				spthreadT t(new threadT(boost::bind(&ioSrvT::run,_iosrvs[i])));
				threads.push_back(t);
			}
		}

		if(!asyn)
		{
			for (size_t i = 0;i<threads.size();++i)
				threads[i]->join();
		}
	}
	void stop()
	{
		for (size_t i = 0;i<_iosrvs.size();++i)
			_iosrvs[i]->stop();
	}

	ioSrvT& getIoSrv()
	{
		ioSrvT& iosrv = *_iosrvs[_next];
		++_next;
		if(_next >= _iosrvs.size())
			_next = 0;

		return iosrv;
	}
private:
	std::vector<spioSrvT>   _iosrvs;
	std::vector<spworkT>    _works;
	std::size_t             _next;
	std::size_t             _threadCntPreIo;
};