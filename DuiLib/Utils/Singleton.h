#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#pragma once
#include <memory>

#define DECLARE_SINGLETON_CLASS( type ) \
	friend class std::auto_ptr< type >; \
	friend class DuiLib::Singleton< type >;

namespace DuiLib
{
	template <class T>
	class Singleton
	{
	public:
		static inline T* Instance()
		{
			m_pcs.Lock();
			{
				/*if (0 == _instance.get())
				{
					_instance.reset(new T);
				}*/
				if (!_inst) _inst = new T;
			}
			m_pcs.Unlock();
			//return _instance.get();
			return _inst;
		}

		Singleton()
		{
		}
		virtual ~Singleton()
		{
			delete _inst;
			_inst = NULL;
		}
	private:
		Singleton(const Singleton&) {}
		Singleton& operator= (const Singleton&) { return *this; }
	protected:

#pragma warning(push)
#pragma warning(disable:4251)
#pragma warning(disable:4661)
		//static std::auto_ptr<T> _instance;
		static T* _inst;
#pragma warning(pop) 

		static CDuiLock m_pcs;
	};

	//template <class T>
	//std::auto_ptr<T> Singleton<T>::_instance;
	template <class T>
	CDuiLock Singleton<T>::m_pcs;
	template <class T>
	T* Singleton<T>::_inst = NULL;
}


#endif//__SINGLETON_H__