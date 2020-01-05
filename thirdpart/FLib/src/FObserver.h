#ifndef __FLIB_OBSERVER_H__
#define __FLIB_OBSERVER_H__
#pragma once
#include "FType.h"
#include <map>

_FStdBegin
template <typename ReturnT, typename ParamT>
class FReceiverImplBase;

template <typename ReturnT, typename ParamT>
class FObserverImplBase
{
public:
	virtual void AddReceiver(FReceiverImplBase<ReturnT, ParamT>* receiver) = 0;
	virtual void RemoveReceiver(FReceiverImplBase<ReturnT, ParamT>* receiver) = 0;
	virtual ReturnT Broadcast(ParamT param) = 0;
	virtual ReturnT RBroadcast(ParamT param) = 0;
	virtual ReturnT Notify(ParamT param) = 0;
};

template <typename ReturnT, typename ParamT>
class FReceiverImplBase
{
public:
	virtual void AddObserver(FObserverImplBase<ReturnT, ParamT>* observer) = 0;
	virtual void RemoveObserver() = 0;
	virtual ReturnT Receive(ParamT param) = 0;
	virtual ReturnT Respond(ParamT param, FObserverImplBase<ReturnT, ParamT>* observer) = 0;
};

template <typename ReturnT, typename ParamT>
class FReceiverImpl;

template <typename ReturnT, typename ParamT>
class FObserverImpl : public FObserverImplBase<ReturnT, ParamT>
{
	template <typename ReturnT_, typename ParamT_>
	friend class Iterator;
public:
	FObserverImpl()
	{}

	virtual ~FObserverImpl()	{}

	virtual void AddReceiver(FReceiverImplBase<ReturnT, ParamT>* receiver)
	{
		if (receiver == NULL)
			return;

		receivers_.push_back(receiver);
		receiver->AddObserver(this);
	}

	virtual void RemoveReceiver(FReceiverImplBase<ReturnT, ParamT>* receiver)
	{
		if (receiver == NULL)
			return;

		typename ReceiversVector::iterator it = receivers_.begin();
		for (; it != receivers_.end(); ++it)
		{
			if (*it == receiver)
			{
				receivers_.erase(it);
				break;
			}
		}
	}

	virtual ReturnT Broadcast(ParamT param)
	{
		typename ReceiversVector::iterator it = receivers_.begin();
		for (; it != receivers_.end(); ++it)
		{
			(*it)->Receive(param);
		}

		return ReturnT();
	}

	virtual ReturnT RBroadcast(ParamT param)
	{
		typename ReceiversVector::reverse_iterator it = receivers_.rbegin();
		for (; it != receivers_.rend(); ++it)
		{
			(*it)->Receive(param);
		}

		return ReturnT();
	}

	virtual ReturnT Notify(ParamT param)
	{
		typename ReceiversVector::iterator it = receivers_.begin();
		for (; it != receivers_.end(); ++it)
		{
			(*it)->Respond(param, this);
		}

		return ReturnT();
	}

	template <typename ReturnT_, typename ParamT_>
	class Iterator
	{
		FObserverImpl<ReturnT_, ParamT_> & _tbl;
		unsigned long index;
		FReceiverImplBase<ReturnT_, ParamT_>* ptr;
	public:
		Iterator( FObserverImpl & table )
			: _tbl( table ), index(0), ptr(NULL)
		{}

		Iterator( const Iterator & v )
			: _tbl( v._tbl ), index(v.index), ptr(v.ptr)
		{}

		FReceiverImplBase<ReturnT_, ParamT_>* next()
		{
			if ( index >= _tbl.receivers_.size() )
				return NULL;

			for ( ; index < _tbl.receivers_.size(); )
			{
				ptr = _tbl.receivers_[ index++ ];
				if ( ptr )
					return ptr;
			}
			return NULL;
		}
	};

protected:
	typedef std::vector<FReceiverImplBase<ReturnT, ParamT>*> ReceiversVector;
	ReceiversVector receivers_;
};

template <typename ReturnT, typename ParamT>
class FReceiverImpl : public FReceiverImplBase<ReturnT, ParamT>
{
public:
	FReceiverImpl()
	{}

	virtual ~FReceiverImpl()	{}

	virtual void AddObserver(FObserverImplBase<ReturnT, ParamT>* observer)
	{
		observers_.push_back(observer);
	}

	virtual void RemoveObserver()
	{
		typename ObserversVector::iterator it = observers_.begin();
		for (; it != observers_.end(); ++it)
		{
			(*it)->RemoveReceiver(this);
		}
	}

	virtual ReturnT Receive(ParamT param)
	{
		return ReturnT();
	}

	virtual ReturnT Respond(ParamT param, FObserverImplBase<ReturnT, ParamT>* observer)
	{
		return ReturnT();
	}

protected:
	typedef std::vector<FObserverImplBase<ReturnT, ParamT>*> ObserversVector;
	ObserversVector observers_;
};
_FStdEnd

#endif // __FLIB_OBSERVER_H__
