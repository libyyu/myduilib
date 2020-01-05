#ifndef _DECODER_PLUGIN_
#define _DECODER_PLUGIN_

#define DECODER_ERROR -1
#define DECODER_WAIT 0
#define DECODER_SUCCEED 1

#include "DecoderType.h"	

typedef void * decoder_handle;
struct player_audio_info
{
	int n_size;
	int n_sample_rate;
	int n_sample_size_in_bit;
	int n_channal;
	int n_total_play_time_in_ms;
	int n_file_size;
	int n_total_blocks;//just for ape file
};

struct decoder_plugin
{
	unsigned int n_size;
	decoder_handle (*open)(const char * sz_file_name,bool is_online,int nFileType,int nBegin,int nEnd);//如果返回是NULL，表示不支持
	int (*get_audio_info)(decoder_handle handle,struct player_audio_info* p_info);//获取音频信息,如果失败返回DECODER_ERROR,成功返回 DECODER_SUCCEED
	int (*decode_once)(decoder_handle handle);//进行一次解码操作，如果失败返回DECODER_ERROR，如果没有数据可以解码返回0,成功返回解码样本数量
	int (*get_available_samples_count)(decoder_handle handle);//获取已经解码的样本数量，如果已经出错返回DECODER_ERROR
	int (*get_data)(decoder_handle handle,unsigned char *p_data,int n_buf_size);//以16位格式，获取已经解码的数据。失败返回DECODER_ERROR，没有数据返回0
	int (*write_data)(decoder_handle handle,unsigned char *p_data,int n_buf_size);//用户在线播放模式，写入需要解码的数据，如果写入长度为0表示数据已经全部写入。失败返回DECODER_ERROR
	__int64 (*seek)(decoder_handle handle,float f_percent); //直接跳到指定位置播放，如果超出文件范围，或且指定位置还没有下载完成，则返回DECODER_ERROR
	__int64 (*get_download_data_length)(decoder_handle handle);//取得已经下载完成的数据的长度
	__int64 (*get_current_position)(decoder_handle handle);//取得当前解码位置
	bool (*is_finish)(decoder_handle handle);//本地模式，全部数据解码完成，解码完的数据并被全部取走;在线模式，数据全部写入并解码完成，而且解码后的数据全部被取走后，返回true，否则返回false
	void (*close)(decoder_handle handle);//关闭解码句柄
	float(*get_percent)(decoder_handle handle);//获得播放进度百分比 
	int (*get_decoder_type)();//获取解码器类型
};

typedef void * (*GET_DECODER_PROC)();
#endif //_DECODER_PLUGIN_