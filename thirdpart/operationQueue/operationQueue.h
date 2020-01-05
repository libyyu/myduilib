#ifndef OPERATIONQUEUE_H
#define OPERATIONQUEUE_H
#pragma once
#include <memory>
#include <functional>

#ifdef OPERATIONQUEUE_STATIC_EXPORT
#define OPERATIONQUEUE_DLL_API
#else
#ifdef _MSC_VER
#ifdef OPERATIONQUEUE_DLL_EXPORT
#define OPERATIONQUEUE_DLL_API __declspec(dllexport)
#else
#define OPERATIONQUEUE_DLL_API __declspec(dllimport)
#pragma comment(lib,"operationQ.lib")
#endif
#else
#define OPERATIONQUEUE_DLL_API
#endif
#endif

class OPERATIONQUEUE_DLL_API COperationQueue
{
	friend class COperationQueueImpl;
    class COperationQueueImpl* _impl;

public:
	typedef std::function<void (void)> STDOperation;
	COperationQueue(unsigned int MaxConcurrency = 1);
	virtual ~COperationQueue();
public:
	void addOperation(const STDOperation& op,bool atfont = false);

    void stop();

	void cancle();

	size_t size() const;
};

typedef std::shared_ptr<COperationQueue> spOperationQueueT;

#endif//OPERATIONQUEUE_H