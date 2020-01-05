#pragma  once
#include <boost/noncopyable.hpp>
#include "traits.h"

template<typename T>
class CScopeValue
{
public:
	typedef CScopeValue<T> self_type;
	typedef typename trait::type_traits<T>::reference   reference;
	typedef typename trait::type_traits<T>::value_type  value_type;

	explicit CScopeValue(reference v):_refValue(v),_oldValue(v){}
	CScopeValue(reference v,value_type newvalue):_refValue(v),_oldValue(v){setvalue(newvalue);}
	~CScopeValue()
	{
		_refValue = _oldValue;
	}
public:
	operator value_type()const{return *_refValue;}
	value_type setvalue(value_type v)
	{
		_refValue = v;
		return _oldValue;
	}
private:
	reference            _refValue;
	const value_type     _oldValue;
};


template <typename T> 
class CSingletonT: public boost::noncopyable
{
public:
	typedef typename trait::type_traits<T>::value_type            value_type; 
	typedef typename trait::type_traits<value_type>::pointer      pointer;
	static pointer Instance()
	{
		if (_pInstance == NULL) _pInstance = pointer(new value_type);
		return _pInstance;
	};
	static void DestroyInstance()
	{
		if ( _pInstance != NULL)
			delete _pInstance;
		_pInstance = NULL;
	};

private:
	static pointer _pInstance; 
};

template<typename T>
class CSingletonT<boost::shared_ptr<T> > : public boost::noncopyable
{
public:
#ifdef _MSC_VER
	typedef typename CSingletonT<boost::shared_ptr<T> >  self_type;
#else
    typedef CSingletonT<boost::shared_ptr<T> >  self_type;
#endif
	typedef typename trait::type_traits<T>::value_type   value_type; 
	typedef typename boost::shared_ptr<value_type>       pointer;
	static pointer Instance()
	{
		static pointer _pInstance(new value_type);
		return _pInstance;
	};
};

template <typename T> typename CSingletonT<T>::pointer  CSingletonT<T>::_pInstance = CSingletonT<T>::pointer();
