#ifdef _WIN32
#	include <winsock2.h>
#	include <windows.h>
#endif
#include "CDecoderBase.h"
#include "Utility/log.h"


static int readFun(void *userdata, uint8_t *buf, int buf_size)
{
	CDecoderBase * theDec = static_cast<CDecoderBase*>(userdata);
	assert(theDec);

	return (theDec  ? theDec->onRead(buf,buf_size) : 0);
}

static int writeFun(void *userdata, uint8_t *buf, int buf_size)
{
	CDecoderBase * theDec = static_cast<CDecoderBase*>(userdata);
	assert(theDec);

	return (theDec  ? theDec->onWrite(buf,buf_size) : 0);
}

static int64_t seekFun(void *userdata, int64_t offset, int whence)
{
	CDecoderBase * theDec = static_cast<CDecoderBase*>(userdata);
	assert(theDec);

	return (theDec  ? theDec->onSeek(offset,whence) : -1);
}

#ifndef AVCODEC_MAX_AUDIO_FRAME_SIZE
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000
#endif
#define USE_NEW_FFMPEG 0
#if USE_NEW_FFMPEG
int my_avcodec_decode_audio3(AVCodecContext* avctx, int16_t* samples, int* frame_size_ptr,AVPacket* avpkt)
{
	AVFrame* frame = av_frame_alloc();
	int ret, got_frame = 0;
	if (!frame)
		return AVERROR(ENOMEM);
	ret = avcodec_decode_audio4(avctx, frame, &got_frame, avpkt);
	if (ret >= 0 && got_frame) {
		int ch, plane_size;
		int planar = av_sample_fmt_is_planar(avctx->sample_fmt);
		int data_size = av_samples_get_buffer_size(&plane_size, avctx->channels,
			frame->nb_samples,
			avctx->sample_fmt, 1);
		if (*frame_size_ptr < data_size) 
		{
			av_log(avctx, AV_LOG_ERROR, "output buffer size is too small for "
				"the current frame (%d < %d)\n", *frame_size_ptr, data_size);
			av_frame_free(&frame);
			return AVERROR(EINVAL);
		}
		memcpy(samples, frame->extended_data[0], plane_size);
		if (planar && avctx->channels > 1) {
			uint8_t* out = ((uint8_t*)samples) + plane_size;
			for (ch = 1; ch < avctx->channels; ch++) {
				memcpy(out, frame->extended_data[ch], plane_size);
				out += plane_size;
			}
		}
		*frame_size_ptr = data_size;
	}
	else {
		*frame_size_ptr = 0;
	}

	av_frame_free(&frame);
	return ret;
}
#endif
/************************************************************************/
/*                                                                      */
/************************************************************************/

CDecoderBase::CDecoderBase()
	:_pFmtCtx(NULL),
	_pCodecCtxA(NULL),
	_pCodecA(NULL),
	_pStreamA(NULL),
	_stream_index_a(-1),
	_hRead(INVALID_HANDLE_VALUE),
	_nBegin(0),
	_nEnd(0),
	_buffer(AVCODEC_MAX_AUDIO_FRAME_SIZE)
{
	static bool isInit = false;
	if(!isInit)
	{
		av_register_all();
		isInit = true;
	}
}

CDecoderBase::~CDecoderBase()
{
	this->close();
}

bool CDecoderBase::open(const char* pszFileName,int nBegin,int nEnd)
{
	this->_nBegin = nBegin;
	this->_nEnd   = nEnd;
	this->_hRead  = ::CreateFileA(pszFileName,GENERIC_READ,  FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	sLog("CDecoderBase::open fun,Error=%d",::GetLastError());
	return (INVALID_HANDLE_VALUE != this->_hRead) && (initStreamFormat());
}
void CDecoderBase::close()
{
	clearPacketList();

	if(_pCodecCtxA)
	{
		avcodec_close(_pCodecCtxA);
		_pCodecCtxA = NULL;
	}

	if(_pFmtCtx)
	{
		/*
		if(_pFmtCtx->pb) //bug ffmpeg
		{
			if(_pFmtCtx->pb->buffer)
			{
				av_free(_pFmtCtx->pb->buffer);
				_pFmtCtx->pb->buffer = NULL;
			}
			av_free(_pFmtCtx->pb);
			_pFmtCtx->pb = NULL;
		}
		*/
		avformat_close_input(&_pFmtCtx);
		//_pFmtCtx = NULL;
	}

	if(INVALID_HANDLE_VALUE != _hRead)
	{
		::CloseHandle(_hRead);
		_hRead = INVALID_HANDLE_VALUE;
	}

	this->_nBegin = 0;
	this->_nEnd   = 0;
	this->_buffer.clear();
}
bool CDecoderBase::getInfo(struct player_audio_info* p_info) const
{
	assert(p_info);
	if(p_info && isInitStream())
	{
		p_info->n_size                  = sizeof(player_audio_info);
		p_info->n_file_size             = _pFmtCtx->pb->maxsize;
		p_info->n_channal               = FFMIN(2, _pCodecCtxA->channels);
		p_info->n_sample_rate           = _pCodecCtxA->sample_rate;
		p_info->n_sample_size_in_bit    = _pCodecCtxA->bits_per_raw_sample;
		p_info->n_total_play_time_in_ms = _pFmtCtx->duration/1000;
		p_info->n_total_blocks          = 0;
		
		if(p_info->n_sample_size_in_bit == 24)
			p_info->n_sample_size_in_bit = 32;

		switch (_pCodecCtxA->sample_fmt)
		{
		case AV_SAMPLE_FMT_U8:   p_info->n_sample_size_in_bit = sizeof(char);  break;
		case AV_SAMPLE_FMT_S16:  p_info->n_sample_size_in_bit = sizeof(short);  break;
		case AV_SAMPLE_FMT_S32:  p_info->n_sample_size_in_bit = sizeof(int);    break;
		case AV_SAMPLE_FMT_FLT:  p_info->n_sample_size_in_bit = sizeof(float);  break;
		case AV_SAMPLE_FMT_DBL:  p_info->n_sample_size_in_bit = sizeof(double); break;
		case AV_SAMPLE_FMT_FLTP: p_info->n_sample_size_in_bit = sizeof(float);  break;
		default: assert(false);  break;
		}

		p_info->n_sample_size_in_bit *= 8;

		assert(p_info->n_sample_size_in_bit);
		assert(p_info->n_channal);

		return true;
	}

	return false;
}
size_t CDecoderBase::samplesCnt() const
{
	if(!isInitStream())
		return 0;

	if(_pkList.empty() && _buffer.empty())
	{
		CDecoderBase* pThis = const_cast<CDecoderBase*>(this);
		pThis->decodeOnce();
	}
	return std::max<size_t>(_pkList.size(),_buffer.cnt());
}
bool CDecoderBase::decodeOnce()
{
	assert(isInitStream());
	if(!_buffer.empty() || !_pkList.empty())
		return true;

	AVPacket* pk = (AVPacket*)av_malloc(sizeof(AVPacket));
	assert(pk);
	if (!pk)
		return false;

	int errCnt = MAX_ERROR;
	do 
	{
		if(av_read_frame(_pFmtCtx,pk) == 0)
		{
			assert(pk->data);
			assert(pk->size);
			if(pk->stream_index != this->_stream_index_a)
			{
				av_free_packet(pk);
				continue;
			}
			
			_pkList.push_back(pk);
			return true;
		}
	} while (--errCnt);

	av_free(pk);
	return true;
}
size_t CDecoderBase::getData(unsigned char *p_data,int n_buf_size)
{
	if(_buffer.empty() && !_pkList.empty())
	{
		_buffer.clear();
		AVPacket* pk = *(_pkList.begin());
		assert(pk);
		while(pk && pk->size > 0)
		{
			int16_t* pdata  = (int16_t*)(_buffer[0]);
			int cnt			= _buffer.nextwriteblocksize();
#if USE_NEW_FFMPEG
			int nlen        = my_avcodec_decode_audio3(_pCodecCtxA, pdata, &cnt, pk);
#else
			int nlen		= avcodec_decode_audio3(_pCodecCtxA, pdata, &cnt, pk);
#endif

			if(nlen < 0)
			{
				sLog("CDecodecBase::getData fun,avcodec_decode_audio3 nlen=%d", nlen);
				pk->size = 0;
				break;
			}
			if (nlen < pk->size && nlen > 0)
			{
				const int nrem = pk->size - nlen;
				memmove(pk->data, pk->data + nlen, nrem);
				//	memset(pk->data + nlen,0,nrem);
			}

			pk->size -= nlen;

			if (cnt)
			{
				_buffer.wpos(cnt);
				break;
			}
		}

		if(pk && 0 >= pk->size)
		{
			assert(0 == pk->size);
			av_free_packet(pk);
			av_free(pk);
			pk = NULL;
			_pkList.pop_front();
		}
	}

	if(!_buffer.empty())
	{
		size_t cnt = std::min<size_t>(_buffer.cnt(),n_buf_size);
		return _buffer.Read(p_data,cnt);
	}

	return 0;
}
bool CDecoderBase::seek(float fpercent)
{
	if(!isInitStream())
		return false;

	AVRational av = {1,AV_TIME_BASE};
	int64_t timestamp = av_rescale_q(_pFmtCtx->duration * fpercent,av,_pStreamA->time_base);
	//_pCodec->flush(_pCodecCtxA);
	if(av_seek_frame(_pFmtCtx, _stream_index_a,timestamp,AVSEEK_FLAG_ANY) >= 0)
	{
		_buffer.clear();
		clearPacketList();
		return true;
	}

	return false;
}
size_t CDecoderBase::writeData(unsigned char *p_data,int n_buf_size)
{
	return 0;
}
size_t CDecoderBase::downBytes() const
{
	return 0;
}
bool CDecoderBase::isFinish() const
{
	if(!isInitStream())
		return false;

	if(_pkList.empty() && _buffer.empty())
	{
		CDecoderBase* pThis = const_cast<CDecoderBase*>(this);
		pThis->decodeOnce();
	}

	return _pkList.empty() && _buffer.empty();
}

__int64 CDecoderBase::ReadPosition() const
{
	if(_pFmtCtx && _pFmtCtx->pb)
		return _pFmtCtx->pb->pos;
	else
		return 0;
}

bool CDecoderBase::isInitStream() const
{
	return _pFmtCtx;
}
void CDecoderBase::clearPacketList()
{
	for (PacketListIterT it = _pkList.begin(),iend = _pkList.end();it != iend;++it)
		av_free_packet(*it),av_free(*it);

	_pkList.clear();
}

bool CDecoderBase::initStreamFormat()
{
	assert(!_pFmtCtx);
	assert(_hRead);
	if(!_hRead)
		return false;
	if(_pFmtCtx)
		return true;

	AVFormatContext* fmt = NULL;
	AVCodecContext* codeCtxA = NULL;
	AVCodec* decA = NULL;
	AVStream* streamA = NULL;
	int stream_index_a = -1;

	fmt = avformat_alloc_context();
	assert(fmt);
	if(!fmt)
		return false;

	fmt->pb = avio_alloc_context(NULL,0,0,this,readFun,writeFun,seekFun);
	assert(fmt->pb);
	if(!fmt->pb)
	{
		avformat_free_context(fmt);
		return false;
	}

	if(0 != avformat_open_input(&fmt,NULL,NULL,NULL))
	{
		avformat_free_context(fmt);
		return false;
	}

	if(0 > avformat_find_stream_info(fmt, NULL))
	{
		avformat_close_input(&fmt);
		avformat_free_context(fmt);
		return false;
	}

	for(int i = 0; i < fmt->nb_streams; ++i) 
	{ 
		if ((fmt->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) && (stream_index_a == -1))
		{
			stream_index_a = i;
			// A3. ´ò¿ªÒôÆµÁ÷
			//open_audio_stream(fmt, p_acodec_ctx, a_idx);

			streamA = fmt->streams[i];
			codeCtxA = streamA->codec;
		}

		if (stream_index_a != -1)
		{
			break;
		}
	} 

	assert(codeCtxA);
	if(!codeCtxA)
	{
		avformat_close_input(&fmt);
		avformat_free_context(fmt);
		return false;
	}

	decA = avcodec_find_decoder(codeCtxA->codec_id);
	if(!decA || avcodec_open2(codeCtxA, decA, NULL) < 0)
	{
		avformat_close_input(&fmt);
		avformat_free_context(fmt);
		return false;
	}

	assert(fmt);
	assert(codeCtxA);
	assert(decA);
	assert(streamA);
	assert(-1 != stream_index_a);

#ifdef _DEBUG
	av_dump_format(fmt, 0, "", 0);
#endif // _DEBUG

	this->_pFmtCtx      = fmt;
	this->_pCodecCtxA   = codeCtxA;
	this->_pCodecA		= decA;
	this->_pStreamA      = streamA;
	this->_stream_index_a = stream_index_a;

	return !!this->_pFmtCtx;
}


int CDecoderBase::onRead(uint8_t *buf, int buf_size)
{
	DWORD dw = 0;
	::ReadFile(this->_hRead,buf,buf_size,&dw,NULL);
	return dw;
}
int CDecoderBase::onWrite(uint8_t *buf, int buf_size)
{
	return 0;
}
int64_t CDecoderBase::onSeek(int64_t offset, int whence)
{
	HANDLE h = this->_hRead;
	whence &= ~AVSEEK_FORCE;
	switch(whence)
	{
	case SEEK_SET:
		return  ::SetFilePointer(h,offset,NULL,FILE_BEGIN);
	case SEEK_CUR:
		return ::SetFilePointer(h,offset,NULL,FILE_CURRENT);
	case SEEK_END:
		return ::SetFilePointer(h,offset,NULL,FILE_END);
	case AVSEEK_SIZE:
		return  ::GetFileSize(h,NULL);
	default:
		break;
	}

	return -1;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/