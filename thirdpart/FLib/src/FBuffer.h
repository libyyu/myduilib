#ifndef __FLIB_BUFFER_H__
#define __FLIB_BUFFER_H__
#pragma once
#include "FFunc.h"
#include "FAlloctor.h"
#include "FConvert.h"
#include "FIOStream.h"
#include <map>
#include <set>
#include <list>

_FStdBegin

class F_DLL_API FBuffer : public FIOStream
{
public:
    const static size_t DEFAULT_SIZE = 1024*10;

	explicit FBuffer(const uint8 * pBuffer, size_t len, FAlloctorPoolBase* pool = NULL);
	FBuffer(size_t len = DEFAULT_SIZE, FAlloctorPoolBase* pool = NULL);
	virtual ~FBuffer(void);
public:
    size_t              tag() const;
	void                setTag(size_t t);
    size_t              size() const;
    size_t              cnt() const;
    bool                empty() const;
    bool                full() const;
    void                clear();
    void                resize(size_t size = 0);

    size_t              rpos() const;
    size_t              wpos() const;

    size_t              rpos(size_t pos,bool advance=true);
    size_t              wpos(size_t pos,bool advance=true);

    void                SetError(bool error=true);
    bool                HasError();

    size_t              nextreadblocksize();
    size_t              nextwriteblocksize();

    int                 find(const uint8* dest,size_t n) const;
    // size_t              rfind(const uint8* dest,size_t n);

    void                swap(FBuffer& buffer);


    const uint8*        data() const;
    const char*         c_str() const;
    size_t              Read(uint8 *dest,const size_t cnt);
    size_t              Write(const uint8 *src,const size_t cnt);

    template<typename T>
    size_t              Read(T &dst)
	{
		size_t len = Read((uint8*)& dst, sizeof(T));
		EndianConvert(dst);
		return len;
	}
    template<typename T>
	T                   Read()
	{
		T v;
		Read<T>(v);
		return v;
	}

    template<typename T>
    void                Write(const T &src)
	{
		EndianConvert(src);
		Write((uint8*)& src, sizeof(T));
	}


    uint8*   operator[](size_t pos) const;
    bool     operator==(FBuffer& src);

    void                posShow() const;
    void                textShow() const;
    void                hexShow() const;

	using FOStream::operator <<;
	virtual FOStream& operator <<(int8);
	virtual FOStream& operator <<(uint8);
	virtual FOStream& operator <<(int16);
	virtual FOStream& operator <<(uint16);
	virtual FOStream& operator <<(int32);
	virtual FOStream& operator <<(uint32);
	virtual FOStream& operator <<(int64);
	virtual FOStream& operator <<(uint64);
#if FLIB_COMPILER_64BITS
	virtual FOStream& operator <<(int);
	virtual FOStream& operator <<(uint);
#else
	virtual FOStream& operator <<(long);
	virtual FOStream& operator <<(ulong);
#endif
	virtual FOStream& operator <<(bool);
	virtual FOStream& operator <<(float);
	virtual FOStream& operator <<(double);
	virtual FOStream& operator <<(wchar_t);
	virtual FOStream& operator <<(wchar_t[]);
	virtual FOStream& operator <<(const wchar_t*);
	virtual FOStream& operator <<(char[]);
	virtual FOStream& operator <<(const char*);
	virtual FOStream& operator <<(const std::string&);
	virtual FOStream& operator <<(const std::wstring&);
    FBuffer& operator <<(FBuffer &v);
public:
	using FIStream::operator >>;
	virtual FIStream& operator >>(int8&);
	virtual FIStream& operator >>(uint8&);
	virtual FIStream& operator >>(int16&);
	virtual FIStream& operator >>(uint16&);
	virtual FIStream& operator >>(int32&);
	virtual FIStream& operator >>(uint32&);
	virtual FIStream& operator >>(int64&);
	virtual FIStream& operator >>(uint64&);
#if FLIB_COMPILER_64BITS
	virtual FIStream& operator >>(int&);
	virtual FIStream& operator >>(uint&);
#else
	virtual FIStream& operator >>(long&);
	virtual FIStream& operator >>(ulong&);
#endif
	virtual FIStream& operator >>(bool&);
	virtual FIStream& operator >>(float&);
	virtual FIStream& operator >>(double&);
	virtual FIStream& operator >>(wchar_t&);
	virtual FIStream& operator >>(wchar_t[]);
	virtual FIStream& operator >>(const wchar_t*);
	virtual FIStream& operator >>(char[]);
	virtual FIStream& operator >>(const char*);
	virtual FIStream& operator >>(std::string&);
	virtual FIStream& operator >>(std::wstring&);
	FBuffer& operator >>(FBuffer& v);

protected:
    FBuffer(const FBuffer& src){} 
    // ...类型转换构造函数
    FBuffer &operator = (const FBuffer &src){return  *this;} 

    bool         isUserBuffer();
    uint8 *      getBuf(size_t size);
    void         releaseBuf(uint8 * p);
protected:
    uint8 *             _pdata;

    size_t              _cnt;
    size_t              _size;
    size_t              _rpos;
    size_t              _wpos;

    uint8               _flag;
    size_t              _tag;
    FAlloctorPoolBase*  _pool;
};

template<class T>
inline FBuffer& operator<<(FBuffer& buffer, std::set<T>& TSet)
{
	buffer << (uint32)TSet.size();
	typename std::set<T>::const_iterator itor;
	for (itor = TSet.begin(); itor != TSet.end(); ++itor)
		buffer << *itor;

	return buffer;
}
template<class T>
inline FBuffer& operator>>(FBuffer& buffer, std::set<T>& TSet)
{
	uint32 size = 0;
	buffer >> size;
	TSet.clear();

	while (size--)
	{
		T t;
		buffer >> t;
		TSet.insert(t);
	}

	return buffer;
}

template<class T>
inline FBuffer& operator<<(FBuffer& buffer, std::vector<T>& TVector)
{
	buffer << (uint32)TVector.size();

	typename std::vector<T>::const_iterator itor;
	for (itor = TVector.begin(); itor != TVector.end(); ++itor)
		buffer << *itor;

	return buffer;
}
template<class T>
inline FBuffer& operator>>(FBuffer& buffer, std::vector<T>& TVector)
{
	uint32 size = 0;
	buffer >> size;
	TVector.clear();

	while (size--)
	{
		T t;
		buffer >> t;
		TVector.push_back(t);
	}
	return buffer;
}

template<class T>
inline FBuffer& operator<<(FBuffer& buffer, std::list<T>& TList)
{
	buffer << (uint32)TList.size();

	typename std::list<T>::const_iterator itor;
	for (itor = TList.begin(); itor != TList.end(); ++itor)
		buffer << *itor;

	return buffer;
}
template<class T>
inline FBuffer& operator>>(FBuffer& buffer, std::list<T>& TList)
{
	uint32 size = 0;
	buffer >> size;
	TList.clear();

	while (size--)
	{
		T t;
		buffer >> t;
		TList.push_back(t);
	}

	return buffer;
}

template<class K, class V>
inline FBuffer& operator<<(FBuffer& buffer, std::map<K, V>& TMap)
{
	buffer << (uint32)TMap.size();

	typename std::map<K, V>::const_iterator itor;
	for (itor = TMap.begin(); itor != TMap.end(); ++itor)
		buffer
		<< itor->first
		<< itor->second;

	return buffer;
}
template<class K, class V>
inline FBuffer& operator>>(FBuffer& buffer, std::map<K, V>& TMap)
{
	uint32 size = 0;
	buffer >> size;
	TMap.clear();

	while (size--)
	{
		K k;
		V v;
		buffer >> k >> v;
		TMap.insert(make_pair(k, v));
	}

	return buffer;
}


_FStdEnd


#endif//__FLIB_BUFFER_H__
