#pragma once
#include <tchar.h>


namespace em_utility
{
class i_lock
{
public:
	i_lock() {}
	virtual ~i_lock() {}
	virtual bool lock() = 0;
	virtual bool unlock() = 0;
	virtual int lock_ex() { return 0; }
	virtual int unlock_ex() { return 0; }
};

class lock_wrapper
{
public:
	lock_wrapper(i_lock* p_lock);
	~lock_wrapper();
	void unlock();
private:
	i_lock* mp_lock;
};

//������������������waitforXXXobject�����ܿ���̣����ں˶���
class critical_session : public i_lock
{
public:
	critical_session(/*CLogObj* p_log = NULL*/);
	virtual ~critical_session();
	virtual bool lock();
	virtual bool unlock();
	bool try_lock();
	int get_ref() const;
	virtual int lock_ex();
	virtual int unlock_ex();
private:
	struct critical_impl;
	critical_impl* mp_impl;
	volatile int mn_ref;
/*	CLogObj* mp_log;*/
};

//������
class mutex_lock : public i_lock
{
public:
	mutex_lock();
	virtual ~mutex_lock();
	virtual bool lock();
	virtual bool unlock();
	//����trueΪ�ȴ����źţ�falseΪ��ʱ
	bool wait(unsigned int n_seconds);
	virtual int lock_ex();
	virtual int unlock_ex();
private:
	struct mutex_impl;
	mutex_impl* mp_impl;
};

//�ź��¼���
class signal_event
{
public:
	signal_event(bool b_signal = false, const TCHAR* p_name = NULL);
	~signal_event();
	void set_signal();
	void reset_signal();
	//����trueΪ���źţ�����falseΪ��ʱ
	bool wait(unsigned int n_mill_seconds);
	void* get_handle();
private:
	struct event_impl;
	event_impl* mp_impl;
};

}
