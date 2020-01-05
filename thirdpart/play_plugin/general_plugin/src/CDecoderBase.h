#pragma once
#include "iDecoder.h"
#include <cassert>
#include <list>
#include "flib.h"

#ifdef _MSC_VER
#include <Windows.h>
#else
#endif // _MSC_VER

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#ifdef __cplusplus
}
#endif
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")


class CDecoderBase :
	public IDecoder
{
public:
	static const size_t MAX_ERROR = 10;
public:
	CDecoderBase();
	~CDecoderBase();
public:
	virtual bool   open(const char* pszFileName,int nBegin,int nEnd);
	virtual void   close();
	virtual bool   getInfo(struct player_audio_info* p_info) const;
	virtual size_t samplesCnt() const;
	virtual bool   decodeOnce();
	virtual size_t getData(unsigned char *p_data,int n_buf_size);
	virtual bool   seek(float fpercent);
	virtual size_t writeData(unsigned char *p_data,int n_buf_size);
	virtual size_t downBytes() const;
	virtual bool   isFinish() const;
	virtual __int64 ReadPosition() const;
public:
	virtual int onRead(uint8_t *buf, int buf_size);
	virtual int onWrite(uint8_t *buf, int buf_size);
	virtual int64_t onSeek(int64_t offset, int whence);
protected:
	bool isInitStream() const;
	bool initStreamFormat();
	void clearPacketList();
protected:
	AVFormatContext*       _pFmtCtx; 
	AVCodecContext*        _pCodecCtxA; 
	AVCodec*			   _pCodecA;
	AVStream*			   _pStreamA;
	int                    _stream_index_a; 
	HANDLE                 _hRead;
	size_t                 _nBegin;
	size_t                 _nEnd;

	typedef std::list<AVPacket*>  PacketListT;
	typedef PacketListT::iterator PacketListIterT;
	              
	PacketListT            _pkList;
	_FStd(FBuffer)         _buffer;
};