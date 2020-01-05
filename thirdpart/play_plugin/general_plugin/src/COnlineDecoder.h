#pragma once
#include "CDecoderBase.h"

class COnlineDecoder : public CDecoderBase
{
	static const size_t MIN_OFFSET = 1024*16;
public:
	COnlineDecoder();
	~COnlineDecoder();
public:
	virtual bool   open(const char* pszFileName,int nBegin,int nEnd);
	virtual void   close();
	virtual bool   getInfo(struct player_audio_info* p_info) const;
	virtual bool   decodeOnce();
	virtual size_t writeData(unsigned char *p_data,int n_buf_size);
	virtual size_t downBytes() const;
	virtual bool   isFinish() const;
	virtual __int64 ReadPosition() const;
public:
	virtual int64_t onSeek(int64_t offset, int whence);
protected:
	bool isDownFinish() const;
protected:
	HANDLE      _hWrite;
	size_t      _nFileSize;
	size_t      _nDownSize;
};