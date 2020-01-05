#define FLIB_DLL
#include "FBuffer.h"
#include <algorithm>
#include <bitset>
#include <functional>
#include <memory>
#include <assert.h>

_FStdBegin

static const uint8 buffer_flag_isuser        = 1<<1;
static const uint8 buffer_flag_iserror       = 1<<2;

FBuffer::FBuffer(const uint8 * pBuffer, size_t len, FAlloctorPoolBase* pool/* = NULL*/)
	:_flag(buffer_flag_isuser),
	_rpos(0),
	_wpos(0),
	_size(len),
	_cnt(len),
	_pdata((uint8 *)pBuffer),
	_tag(0),
	_pool(pool)
{
	assert(pBuffer);
	assert(len > 0);
}

FBuffer::FBuffer(size_t len/* = DEFAULT_SIZE*/, FAlloctorPoolBase* pool/* = NULL*/)
	:_flag(0),
	_rpos(0),
	_wpos(0),
	_size(len),
	_cnt(0),
	_pdata(NULL),
	_tag(0),
	_pool(pool)
{
	if (0 >= _size) _size = DEFAULT_SIZE;

	_pdata = getBuf(_size);
	memset(_pdata, 0, _size);
}
FBuffer::~FBuffer(void)
{
	if (!isUserBuffer() && _pdata)
	{
		releaseBuf(_pdata);
		_pdata = NULL;
	}
}

bool FBuffer::isUserBuffer() {
    return (_flag & buffer_flag_isuser) > 0;
}
uint8 * FBuffer::getBuf(size_t size)
{
    assert(size > 0);
    return ((NULL == _pool) ? (uint8 *)FPoolMalloc(size) : (uint8 *)_pool->Alloc(size));
}
void FBuffer::releaseBuf(uint8 * p) 
{
    assert(NULL != p);
    (NULL == _pool) ? FPoolFree(p) : _pool->Free(p);
}
size_t  FBuffer::tag() const
{
	return _tag;
}
void FBuffer::setTag(size_t t)
{
	_tag = t;
}
size_t  FBuffer::size() const
{
    return _size;
}
size_t FBuffer::cnt() const
{
    return _cnt;
}
bool   FBuffer::full() const 
{
    return _size == _cnt;
}
bool   FBuffer::empty() const
{
    return 0 == _cnt;
}
size_t  FBuffer::rpos() const
{
    return _rpos;
}
size_t   FBuffer::wpos() const
{
    return _wpos;
}
size_t  FBuffer::rpos(size_t pos,bool advance /*=true*/)
{
    assert(!(_rpos == _wpos && pos == _rpos));
    _rpos = (pos%_size);
    if(_rpos == _wpos) {
        _cnt = advance ? 0 : size();
    }else {
        _cnt = (_rpos > _wpos) ? (size() - (_rpos - _wpos)) : (_wpos - _rpos);
    }
    return _rpos;
}
size_t  FBuffer::wpos(size_t pos,bool advance/*=true*/)
{
    assert(!(_rpos == _wpos && pos == _wpos));
    _wpos = (pos%_size);
    if(_wpos == _rpos) {
        _cnt = advance ? size() : 0;
    }else {
        _cnt = (_rpos > _wpos) ? (size() -(_rpos - _wpos)) : (_wpos - _rpos);
    }
    return _wpos;
}
void   FBuffer::clear() 
{
    _rpos = 0;
    _wpos = 0;
    _cnt  = 0;
}
void   FBuffer::resize(size_t size /*= 0*/) {
    if(size <= cnt()) size = 2*_size;
    if(0 == size) size = DEFAULT_SIZE;
    // if(size == _size) return;

    uint8 * pnew = getBuf(size);
    assert(pnew);
    //memset(pnew, 0, size);

    if(_cnt > 0) memcpyFromRing(pnew, _cnt, _pdata, _rpos, _size);

    if (!isUserBuffer() && _pdata)
        releaseBuf(_pdata);

    _pdata   = pnew;
    _rpos    = 0;
    _wpos    = _cnt;
    _size    = size;

    _flag &= ~buffer_flag_isuser;
}

size_t  FBuffer::nextreadblocksize() {
    if(empty()) return 0;
    return (_wpos > _rpos) ? _cnt : (_size - _rpos);
}
size_t  FBuffer::nextwriteblocksize() {
    if(full()) return 0;
    return (_wpos >= _rpos) ? (_size - _wpos) : (_rpos - _wpos);
}
int FBuffer::find(const uint8* dest,size_t n) const
{
    assert(dest);
    assert(n);
    if(!dest || 0>= n) return -1;
    if(cnt() < n) return -1;


    size_t pos = rpos();
    char* pdst = (char*)dest;
    size_t k = n;
    size_t j = _cnt;
    while(j-- >= k)
    {
        if(*((*this)[pos++]) != *pdst++)
        {
            pdst = (char*)dest;
            k    = n;
        }else
        {
            --k;
            if(0 == k)
                return (int)((pos - n)%_size);
        }
    }
    return -1;
}
void FBuffer::swap(FBuffer& buffer)
{
    std::swap(this->_pdata,buffer._pdata);
    std::swap(this->_cnt,buffer._cnt);
    std::swap(this->_size,buffer._size);
    std::swap(this->_rpos,buffer._rpos);
    std::swap(this->_wpos,buffer._wpos);
    std::swap(this->_flag,buffer._flag);
}
const uint8* FBuffer::data() const
{
    return (*this)[rpos()];
}
const char* FBuffer::c_str() const
{
    return (const char*)data();
}
size_t FBuffer::Read(uint8 *dest,const size_t cnt)
{
    assert(dest);
    assert(cnt >0);
    if(HasError()) return 0;
    if (empty() || NULL == dest || _cnt < cnt) {
        SetError();
        return 0;
    }

    memcpyFromRing(dest, cnt, _pdata, _rpos, _size);
    _rpos = (_rpos + cnt)%_size;


    _cnt -= cnt;
    assert(_cnt >= 0);
    assert(_cnt <= _size);
    return cnt;
}
size_t   FBuffer::Write(const uint8 *src,const size_t cnt) 
{
    assert(src);
    assert(cnt >0);
    if(NULL == src || 0 >= cnt) return 0;

    if (cnt > (_size - _cnt)) {
        const size_t a = 2*(cnt + _cnt);
        const size_t b = 2*_size;
        const size_t newsize = (a > b ? a : b);
        this->resize(newsize);
    }

    memcpyToRing(src, cnt, _pdata, _wpos, _size);
    _wpos = (_wpos + cnt)%_size;

    _cnt += cnt;
    assert(_cnt >= 0);
    assert(_cnt <= _size);
    return cnt;
}
void    FBuffer::SetError(bool error/*=true*/) {
    error ? (_flag |= buffer_flag_iserror) : (_flag &= ~buffer_flag_iserror);
}
bool    FBuffer::HasError() {
    return (_flag&buffer_flag_iserror) > 0;
}
bool FBuffer::operator==(FBuffer& src) 
{
    if(this == &src) 
        return true;
    if(cnt() != src.cnt())
        return false;

    for (size_t i = _rpos; i <_cnt+_rpos; ++i) {
        if(*(*this)[i] != *src[i])
            return false;
    }

    return true;
}
uint8* FBuffer::operator[](size_t pos) const {
    return &_pdata[pos%_size];
}

//////////////////////////////////////////////////////////////////////////
FBuffer& FBuffer::operator<<(int8 v)
{
    Write<int8>(v);
    return *this;
}
FBuffer& FBuffer::operator<<(int16 v)
{
    Write<int16>(v);
    return *this;
}
FBuffer& FBuffer::operator<<(int32 v)
{
    Write<int32>(v);
    return *this;
}
FBuffer& FBuffer::operator<<(int64 v)
{
    Write<int64>(v);
    return *this;
}
FBuffer& FBuffer::operator<<(uint8 v)
{
    Write<uint8>(v);
    return *this;
}
FBuffer& FBuffer::operator<<(uint16 v)
{
    Write<uint16>(v);
    return *this;
}
FBuffer& FBuffer::operator<<(uint32 v)
{
    Write<uint32>(v);
    return *this;
}
FBuffer& FBuffer::operator<<(uint64 v)
{
    Write<uint64>(v);
    return *this;
}
#if FLIB_COMPILER_64BITS
FBuffer& FBuffer::operator<<(int v)
{
    Write<int>(v);
    return *this;
}
FBuffer& FBuffer::operator<<(uint v)
{
    Write<uint>(v);
    return *this;
}
#else
FBuffer& FBuffer::operator<<(long v)
{
	Write<long>(v);
	return *this;
}
FBuffer& FBuffer::operator<<(ulong v)
{
	Write<ulong>(v);
	return *this;
}
#endif
FBuffer& FBuffer::operator<<(bool v)
{
    Write<bool>(v);
    return *this;
}
FBuffer& FBuffer::operator<<(float v)
{
    Write<float>(v);
    return *this;
}
FBuffer& FBuffer::operator<<(double v)
{
    Write<double>(v);
    return *this;
}
FBuffer& FBuffer::operator<<(const char *str)
{
    assert(str);
    size_t len = strlen(str);
    Write<uint32>(len);
    Write((uint8 const *)str, len);
    return *this;
}
FBuffer& FBuffer::operator<<(char v[])
{
    size_t len = strlen(v);
    Write<uint32>(len);
    Write((uint8 const *)v, len);
    return *this;
}

FBuffer& FBuffer::operator<<(FBuffer &v)
{
    if(v.empty()) return *this;

    size_t pos = v.rpos();
    while (!v.empty()) {
        uint8 * src = v[v.rpos()];
        size_t  cnt = v.nextreadblocksize();
        this->Write(src, cnt);
        v.rpos(v.rpos() + cnt,true);
    }
    v.rpos(pos,false);
    return *this;
}
FBuffer& FBuffer::operator<< (FBuffer& (*_f)(FBuffer&))
{
	return _f(*this);
}
inline FBuffer& endl(FBuffer& v)
{
    v << '\n';
	return v;
}

//////////////////////////////////////////////////////////////////////////
FBuffer& FBuffer::operator>>(int8 &v)
{
    Read<int8>(v);
    return *this;
}
FBuffer& FBuffer::operator>>(int16 &v)
{
    Read<int16>(v);
    return *this;
}
FBuffer& FBuffer::operator>>(int32 &v)
{
    Read<int32>(v);
    return *this;
}
FBuffer& FBuffer::operator>>(int64 &v)
{
    Read<int64>(v);
    return *this;
}
FBuffer& FBuffer::operator>>(uint8 &v)
{
    Read<uint8>(v);
    return *this;
}
FBuffer& FBuffer::operator>>(uint16 &v)
{
    Read<uint16>(v);
    return *this;
}
FBuffer& FBuffer::operator>>(uint32 &v)
{
    Read<uint32>(v);
    return *this;
}
FBuffer& FBuffer::operator>>(uint64 &v)
{
    Read<uint64>(v);
    return *this;
}
#if FLIB_COMPILER_64BITS
FBuffer& FBuffer::operator>>(int &v)
{
    Read<int>(v);
    return *this;
}
FBuffer& FBuffer::operator>>(uint &v)
{
    Read<uint>(v);
    return *this;
}
#else
FBuffer& FBuffer::operator>>(long &v)
{
	Read<long>(v);
	return *this;
}
FBuffer& FBuffer::operator>>(ulong &v)
{
	Read<ulong>(v);
	return *this;
}
#endif
FBuffer& FBuffer::operator>>(bool &v)
{
    Read<bool>(v);
    return *this;
}
FBuffer& FBuffer::operator>>(float &v)
{
    Read<float>(v);
    return *this;
}
FBuffer& FBuffer::operator>>(double &v)
{
    Read<double>(v);
    return *this;
}

FBuffer& FBuffer::operator>>(const char *dst)
{
    uint32 len;
    Read<uint32>(len);
    uint8* p = (uint8*)dst;
    Read(p, len);
    return *this;
}
FBuffer& FBuffer::operator>>(char dst[])
{
    uint32 len;
    Read<uint32>(len);
    uint8* p = (uint8*)dst;
    Read(p, len);
    return *this;
}
FBuffer& FBuffer::operator>>(FBuffer &v)
{
    v<<*this;
    return *this;
}
void FBuffer::posShow() const
{
    printf("size:%ld,cnt:%ld,rpos:%ld,wpos:%ld\n",
        _size,
        _cnt,
        _rpos,
        _wpos);
}

void FBuffer::textShow() const
{
    posShow();
    size_t pos = _rpos;

    for (size_t i=0; i<_cnt; ++i) {
        char c = *(char*)((*this)[pos++]);
        switch (c)
        {
        case '\r':
            printf("/r\t");
            break;
        case '\n':
            printf("/n\t");
            break;
        case '\t':
            printf("/t\t");
            break;
        case '\0':
            printf("/0\t");
            break;
        default:
            printf("%c\t",c);
        }
        pos %= _size;
    }
    printf("\n");
}
void FBuffer::hexShow() const
{
    posShow();
    size_t pos = _rpos;

    for (size_t i=0; i<_cnt; ++i) {
        printf("%X\t",_pdata[pos++]);
        pos %= _size;
    }
    printf("\n");
}


_FStdEnd
