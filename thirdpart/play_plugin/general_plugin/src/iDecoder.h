#pragma once
#include "decoder_plugin.h"

class IDecoder
{
public:
	IDecoder(){}
	virtual ~IDecoder(){}
public:
	virtual bool   open(const char* pszFileName,int nBegin,int nEnd) = 0;
	virtual void   close() = 0;
	virtual bool   getInfo(struct player_audio_info* p_info) const = 0;
	virtual size_t samplesCnt() const = 0;
	virtual bool   decodeOnce() = 0;
	virtual size_t getData(unsigned char *p_data,int n_buf_size) = 0;
	virtual bool   seek(float fpercent) = 0;
	virtual size_t writeData(unsigned char *p_data,int n_buf_size) = 0;
	virtual size_t downBytes() const = 0;
	virtual bool   isFinish() const = 0;
	virtual __int64 ReadPosition() const = 0;
};