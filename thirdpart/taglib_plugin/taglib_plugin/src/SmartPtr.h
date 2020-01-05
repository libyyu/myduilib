#ifndef __SmartPtr_h
#define __SmartPtr_h


#pragma once
namespace smart_pointer
{
	
	// ���ü�����.
	class smart_count
	{
	public:
		smart_count(int c = 0) : use_count(c) {}
		~smart_count() {}

		// �������ü���, �����ؼ���ֵ.
		int addref() { return ++use_count; }
		// �������ü���, �����ؼ���ֵ.
		int release() { return --use_count; }

	private:
		// ��������.
		int use_count;
	};

	// ����ָ��.
	template <class T>
	class SmartPtr
	{
	public:
		// ����ָ��, ��ʹ���ü�����Ϊ1.
		explicit SmartPtr (T* ptr) : p(ptr), u(new smart_count(1))
		{}

		// �����ָ��.
		explicit SmartPtr () : p(NULL), u(NULL)
		{}

		// ����ָ������.
		~SmartPtr (void)
		{
			// ������ü�������0, ��ɾ�����ݺ����ü���, ����pΪNULL.
			// �˴���Ҫע�����, ���õ�u��δ��Ϊ NULL, ������ָ������
			// ʱ, pΪNULL, �򲻻��ظ�delete.
			if (p && u->release() <= 0)
			{
				delete p;
				delete u;
				p = NULL;
			}		
		}

		// ����ָ�뿽�����캯��.
		SmartPtr (const SmartPtr<T>& t)
		{
			p = t.p;
			u = t.u;

			if (u) // �����жϿ�ֵ.
			{
				u->addref();	// �������ü���.
			}
		}

		// ָ�븳ֵ.
		void operator= (SmartPtr<T>& t)
		{
			// ���Ƚ����ü�����1, Ȼ�����ж��Ƿ�С��0, ���С��0, ��delete.			
			if (p && u->release() <= 0)
			{
				delete p;
				delete u;
			}

			// ֱ�Ӹ�ֵ.
			p = t.p;
			u = t.u;

			if (u)	// �����жϿ�ֵ.
			{
				u->addref(); // �������ü���.
			}
		}

		// ����->������*������.
		T* operator-> (void) { return p; }
		T& operator* (void) { return *p; }
		T* operator-> (void) const { return p; }
		// ����!������.
		bool operator! () const { return !p;}

		// ����ָ��boolֵ������.
		typedef SmartPtr<T> this_type;
		typedef T * this_type::*unspecified_bool_type;
		operator unspecified_bool_type() const { return !p ? 0: &this_type::p; }
		// �õ�ԭָ��.
		T* get() { return p; }
		void reset(T* ptr)
		{
			// ���Ƚ����ü�����1, Ȼ�����ж��Ƿ�С��0, ���С��0, ��delete.			
			if (p && u->release() <= 0)
			{
				delete p;
				delete u;
			}

			// ��ֵ, �����NULL, �򲻴������ü���.
			p = ptr;
			if (p)
				u = new smart_count(1);
			else
				u = NULL;
		}

		void reset(SmartPtr<T>& t)
		{
			// ���Ƚ����ü�����1, Ȼ�����ж��Ƿ�С��0, ���С��0, ��delete.			
			if (p && u->release() <= 0)
			{
				delete p;
				delete u;
			}

			// ��ֵ.
			p = t.p;
			u = t.u;

			if (u)	// �����жϿ�ֵ.
			{
				u->addref(); // �������ü���.
			}
		}

	private:
		T* p;
		smart_count* u;
	};

	// ����==������.
	template<class T, class U> 
	inline bool operator==(SmartPtr<T> & a, SmartPtr<U> & b)
	{
		return a.get() == b.get();
	}

	// ����!=������.
	template<class T, class U> 
	inline bool operator!=(SmartPtr<T> & a, SmartPtr<U> & b)
	{
		return a.get() != b.get();
	}
	

};

#endif//__SmartPtr_h
