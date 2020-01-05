#include "decoder_plugin.h"
#include <cassert>
#include <string>

#ifdef _WIN32
#	include <winsock2.h>
#	include <windows.h>
#endif
#include "iDecoder.h"
#include "CDecoderBase.h"
#include "COnlineDecoder.h"

/************************************************************************/
/*                                                                      */
/************************************************************************/
static decoder_handle i_open(const char * sz_file_name,bool is_online,int nFileType,int nBegin,int nEnd);
static int i_get_audio_info(decoder_handle handle,struct player_audio_info* p_info);
static int i_decode_once(decoder_handle handle);
static int i_get_available_samples_count(decoder_handle handle);
static int i_get_data(decoder_handle handle,unsigned char *p_data,int n_buf_size);
static int i_write_data(decoder_handle handle,unsigned char *p_data,int n_buf_size);
static __int64 i_seek(decoder_handle handle,float f_percent);
static __int64 i_get_download_data_length(decoder_handle handle);
static __int64 i_get_current_position(decoder_handle handle);
static bool i_is_finish(decoder_handle handle);
static void i_close(decoder_handle handle);
static float i_get_percent(decoder_handle handle);
static int i_get_decoder_type();
/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */ 
#ifdef _MSC_VER
	__declspec(dllexport)
#endif
	void* get_xiami_decoder_proc()
	{
		decoder_plugin* p_plugin = new decoder_plugin();
		p_plugin->n_size = sizeof(decoder_plugin);
		p_plugin->open = i_open;
		p_plugin->get_audio_info = i_get_audio_info;
		p_plugin->decode_once = i_decode_once;
		p_plugin->get_available_samples_count = i_get_available_samples_count;
		p_plugin->get_data = i_get_data;
		p_plugin->write_data = i_write_data;
		p_plugin->get_download_data_length = i_get_download_data_length;
		p_plugin->get_current_position = i_get_current_position;
		p_plugin->is_finish = i_is_finish;
		p_plugin->close = i_close;
		p_plugin->seek = i_seek;
		p_plugin->get_percent = i_get_percent;
		p_plugin->get_decoder_type = i_get_decoder_type;
		return p_plugin;
	}
#ifdef __cplusplus
}
#endif /* __cplusplus */ 
/************************************************************************/
/*                                                                      */
/************************************************************************/
static decoder_handle i_open(const char * sz_file_name,bool is_online,int nFileType,int nBegin,int nEnd)
{
	assert(sz_file_name);
	if(!sz_file_name)
		return NULL;

	IDecoder* pDec = (is_online ?  (new COnlineDecoder()) : (new CDecoderBase()));
	assert(pDec);
	if(pDec)
	{
		if(!pDec->open(sz_file_name,nBegin,nEnd))
		{
			assert(pDec);
			delete pDec;
			pDec = NULL;
		}
	}

	return pDec;
}

static int i_get_audio_info(decoder_handle handle,struct player_audio_info* p_info)
{
	IDecoder* pDec = static_cast<IDecoder*>(handle);
	assert(pDec);

	if(pDec && pDec->getInfo(p_info))
	{
		return DECODER_SUCCEED;
	}
	
	return DECODER_ERROR;
}
static int i_decode_once(decoder_handle handle)
{
	IDecoder* pDec = static_cast<IDecoder*>(handle);
	assert(pDec);

	if(pDec && pDec->decodeOnce())
	{
		return DECODER_SUCCEED;
	}

	return DECODER_ERROR;
}
static int i_get_available_samples_count(decoder_handle handle)
{
	IDecoder* pDec = static_cast<IDecoder*>(handle);
	assert(pDec);

	if(pDec)
	{
		return pDec->samplesCnt();
	}

	return 0;
}
static int i_get_data(decoder_handle handle,unsigned char *p_data,int n_buf_size)
{
	IDecoder* pDec = static_cast<IDecoder*>(handle);
	assert(pDec);

	if(pDec)
	{
		return pDec->getData(p_data,n_buf_size);
	}
	return 0;
}
static int i_write_data(decoder_handle handle,unsigned char *p_data,int n_buf_size)
{
	IDecoder* pDec = static_cast<IDecoder*>(handle);
	assert(pDec);

	if(pDec)
	{
		return pDec->writeData(p_data,n_buf_size);
	}
	return 0;
}
static __int64 i_seek(decoder_handle handle,float f_percent)
{
	IDecoder* pDec = static_cast<IDecoder*>(handle);
	assert(pDec);

	if(pDec && pDec->seek(f_percent))
	{
		return DECODER_SUCCEED;
	}
	return DECODER_ERROR;
}
static __int64 i_get_download_data_length(decoder_handle handle)
{
	IDecoder* pDec = static_cast<IDecoder*>(handle);
	assert(pDec);

	if(pDec)
	{
		return pDec->downBytes();
	}
	return 0;
}
static __int64 i_get_current_position(decoder_handle handle)
{

	IDecoder* pDec = static_cast<IDecoder*>(handle);
	assert(pDec);

	if(pDec)
	{
		return pDec->ReadPosition();
	}
	return 0;
}
static bool i_is_finish(decoder_handle handle)
{
	IDecoder* pDec = static_cast<IDecoder*>(handle);
	assert(pDec);

	if(pDec)
	{
		return pDec->isFinish();
	}
	return true;
}
static void i_close(decoder_handle handle)
{
	IDecoder* pDec = static_cast<IDecoder*>(handle);
	assert(pDec);
	if(pDec)
	{
		pDec->close();
		delete pDec;
	}
}
static float i_get_percent(decoder_handle handle)
{
	return 1.0;
}
static int i_get_decoder_type()
{
	return -1;
}

