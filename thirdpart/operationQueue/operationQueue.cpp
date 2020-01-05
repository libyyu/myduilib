#include "operationQueue.h"
#include <algorithm>
#ifdef HAVE_BOOST
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#endif
#include <queue>
#include <set>
#include <list>
#include <cassert>


class COperationQueueImpl
{
	static const size_t TimeOutSecond = 10; 
	static const size_t MaxTimeOutCnt = 6;
public:
#ifdef HAVE_BOOST
	typedef boost::function<void (void)> BoostOperation;
#endif
	typedef std::function<void (void)> STDOperation;
	typedef std::function<void(void)> OperationT;
	COperationQueueImpl(unsigned int MaxConcurrency = 1):_isRunning(true),_curThreadCnt(0)
	{
		_maxConcurrency = std::max<size_t>(1, MaxConcurrency);
	}
	
	virtual ~COperationQueueImpl()
	{
		stop();
	}

	void addOperation(const STDOperation& op,bool atfont = false)
	{
		assert(op);
		if(!op) return;
		if(!_isRunning) return;
		{
			boost::mutex::scoped_lock lock(_mu);
			if(!_isRunning) return;
			atfont ? _opList.push_front(op) : _opList.push_back(op);
			if(_opList.size() > _curThreadCnt && _curThreadCnt < _maxConcurrency)
			{
				++_curThreadCnt;
				boost::thread t(boost::bind(&COperationQueueImpl::onOpThreadPro, this));
			}
		}
		_con.notify_all();
	}

	void stop()
	{
		_isRunning = false;
		_con.notify_all();
		cancle();
	
		bool isOpThread =  false;
		{
			boost::mutex::scoped_lock lock(_mu);
			isOpThread = (_threadIdSet.end() !=  _threadIdSet.find(boost::this_thread::get_id()));
		}
		//	assert(!isOpThread);
		do 
		{
			_con.notify_all();
			boost::this_thread::sleep(boost::posix_time::seconds(1));
			boost::mutex::scoped_lock lock(_mu);
			if(_curThreadCnt == (isOpThread ? 1 : 0))
				break;
		} while (true);
	}

	void cancle()
	{
		boost::mutex::scoped_lock lock(_mu);
		_opList.clear();
	}

	size_t size() const
	{
		COperationQueueImpl* pThis = const_cast<COperationQueueImpl*>(this);
		boost::mutex::scoped_lock lock(pThis->_mu);
		return pThis->_opList.size();
	}
protected:
	bool hasOperation() const {return !(_opList.empty()&&_isRunning);}
	void onOpThreadPro()
	{
      	{
      		boost::mutex::scoped_lock lock(_mu);
      		_threadIdSet.insert(boost::this_thread::get_id());
	  	}
		size_t timeOutCnt = 0;
		while(_isRunning)
		{
			OperationT op;
			{
				boost::mutex::scoped_lock lock(_mu);
				if(!_con.timed_wait(lock,boost::posix_time::seconds(TimeOutSecond),boost::bind(&COperationQueueImpl::hasOperation,this)))
				{
					++timeOutCnt;
					if(timeOutCnt >= MaxTimeOutCnt)
					{
						timeOutCnt = 0;
						if(_curThreadCnt > 1)
						{
                            _threadIdSet.erase(boost::this_thread::get_id());
							--_curThreadCnt;
							return;
						}
					}
					continue;
				}
				if(!_isRunning)
				{
                    _threadIdSet.erase(boost::this_thread::get_id());
					--_curThreadCnt;
					return;
				}

				assert(!_opList.empty());
				op = *(_opList.begin());
				_opList.erase(_opList.begin());
			}

			assert(op);
			if(op) op();
		}

		boost::mutex::scoped_lock lock(_mu);
        _threadIdSet.erase(boost::this_thread::get_id());
		--_curThreadCnt;
	}
protected:
	typedef std::list<OperationT>                 OpListT;
	typedef OpListT::const_iterator               OpListIterT;

    typedef std::set<boost::thread::id>           ThreadIDSetT;
    typedef ThreadIDSetT::const_iterator          ThreadIDSetIterT;

	volatile bool                      _isRunning;
	OpListT                            _opList;
    ThreadIDSetT                       _threadIdSet;
	unsigned int                       _maxConcurrency;
	volatile unsigned int              _curThreadCnt;
	boost::mutex                       _mu;
	boost::condition_variable          _con;
};
/////////////////////////////////////////////////////////////////////////////
//////
COperationQueue::COperationQueue(unsigned int MaxConcurrency)
:_impl(NULL)
{
	_impl = new COperationQueueImpl(MaxConcurrency);
	assert(_impl && "create operationQueue failed");
}

COperationQueue::~COperationQueue()
{
	stop();
	delete _impl;
	_impl = NULL;
}

void COperationQueue::addOperation(const STDOperation& op,bool atfont)
{
	assert(_impl);
	if (!_impl) return;
	_impl->addOperation(op, atfont);
}

void COperationQueue::stop()
{
	assert(_impl);
	if (!_impl) return;
	_impl->stop();
}

void COperationQueue::cancle()
{
	assert(_impl);
	if (!_impl) return;
	_impl->cancle();
}

size_t COperationQueue::size() const
{
	assert(_impl);
	if (!_impl) return 0;
	return _impl->size();
}