#include "decoder_plugin.h"

#include"mp3_plugin.h"
#include "mad.h"
#include "id3/mp3_tag_mgr.h"
#include "zlib.h"


#include <list>
#include <algorithm>
#define INPUT_BUFFER_SIZE	(8192)
#include <string>
#ifdef _MSC_VER
#include <Windows.h>
#else
#endif
#include "flib.h"

class pcm_data_buf;

struct mp3_decoder_data
{
	int mn_size;
	struct mad_stream	m_stream;
	struct mad_frame	m_frame;
	struct mad_synth	m_synth;
	mad_timer_t			m_timer;
	bool mb_online;
	bool mb_data_completed;
	__int64 mn_cur_position;
	float mf_cur_percent;
	__int64 mn_availiable_bytes_count;
	int mn_audio_begin;
	int mn_audio_end;
	int mn_file_size;
	unsigned int mn_mp3_second;
	std::string s_file_name;
	int mn_download_file_size;
	int mn_cur_rec_bytes;
	int mn_cur_download_bytes;
	unsigned char	mp_input_buffer[INPUT_BUFFER_SIZE+MAD_BUFFER_GUARD];
	unsigned char * mp_guard_ptr;

	std::list<pcm_data_buf *> m_raw_pcm_data_list;
	_FStd(FFile) mh_write;
	_FStd(FFile) mh_read;
	player_audio_info*mp_audio_info;

	int  nErrorCnt;
	bool b_FirstDecode;
	
};

static decoder_handle mp3_open(const char * sz_file_name,bool is_online,int nFileType,int nBegin,int nEnd);
static int mp3_get_audio_info(decoder_handle handle,struct player_audio_info* p_info);
static int mp3_decode_once(decoder_handle handle);
static int mp3_get_available_samples_count(decoder_handle handle);
static int mp3_get_data(decoder_handle handle,unsigned char *p_data,int n_buf_size);
static int mp3_write_data(decoder_handle handle,unsigned char *p_data,int n_buf_size);
static __int64 mp3_seek(decoder_handle handle,float f_percent);
static __int64 mp3_get_download_data_length(decoder_handle handle);
static __int64 mp3_get_current_position(decoder_handle handle);
static bool mp3_is_finish(decoder_handle handle);
static void mp3_close(decoder_handle handle);
static float mp3_get_percent(decoder_handle handle);
static int mp3_get_decoder_type();

class pcm_data_buf
{
private:
	int mn_size;
	unsigned char * mp_data;
	int mn_cur_position;
public:
	pcm_data_buf(int n_size)
	{
		mn_size = n_size;
		mp_data = NULL;
		mn_cur_position = 0;
		if(n_size > 0)
		{
			mp_data = new unsigned char[n_size];
		}
	}

	~pcm_data_buf()
	{
		if(mp_data != NULL)
		{
			delete []mp_data;
			mp_data = NULL;
		}
	}

	unsigned char *get_buf()
	{
		return mp_data;
	}

	int set_cur_position(int n_pos)
	{
		return mn_cur_position = n_pos;
	}
	
	int get_cur_position(int n_pos)
	{
		return mn_cur_position ;
	}
	
	int copy_data(unsigned char *p_data,int n_size)
	{
		int n_copy = get_data_size();
		if(n_copy > n_size)
		{
			n_copy = n_size;
		}
		memcpy(p_data,mp_data + mn_cur_position,n_copy);
		mn_cur_position +=n_copy;
		return n_copy;
	}

	int get_data_size()
	{
		return mn_size - mn_cur_position ;
	}

	int empty()
	{
		return mn_cur_position == mn_size; 
	}
};

static decoder_handle mp3_open(const char * sz_file_name,bool b_is_online,int nFileType,int nBegin,int nEnd)
{
	if(sz_file_name == NULL)
		return NULL;
	std::string s_file_name = sz_file_name;
	if(s_file_name.length() < 4)
		return NULL;
	std::transform(s_file_name.begin(), s_file_name.end(), s_file_name.begin(), ::tolower);
	auto pos2 = s_file_name.rfind('.');
	std::string ext = pos2 != s_file_name.npos ? s_file_name.substr(pos2) : "";
	if(ext != ".mp3")
		return NULL;

	mp3_decoder_data *p_decoder_data = new mp3_decoder_data();
	if(p_decoder_data == NULL)
		return NULL;

	p_decoder_data->mn_size = sizeof(mp3_decoder_data);
	p_decoder_data->mb_online = b_is_online;
	p_decoder_data->mp_audio_info = NULL;
	p_decoder_data->mn_cur_position = 0;
	p_decoder_data->mf_cur_percent = 0.0;
	p_decoder_data->mn_availiable_bytes_count = 0;
	p_decoder_data->mp_guard_ptr =NULL;
	p_decoder_data->mn_mp3_second = 0;
	p_decoder_data->mn_audio_begin = 0;
	p_decoder_data->mn_audio_end = 0;
	p_decoder_data->s_file_name = s_file_name;
	p_decoder_data->mn_file_size = 0;
	p_decoder_data->mn_download_file_size = 0;
	p_decoder_data->mn_cur_download_bytes = 0;
	p_decoder_data->mn_cur_rec_bytes = 0;
	p_decoder_data->nErrorCnt = 0;
	p_decoder_data->b_FirstDecode = true;

	if(p_decoder_data->mb_online )
	{
		p_decoder_data->mh_write.Open(sz_file_name, false);
		if(!(p_decoder_data->mh_write))
		{
			//DWORD DW_Res = GetLastError();
			//sLog(_T("创建在线文件失败:%s,错误码<%d>"),s_file_name.GetBuffer(),DW_Res);
			delete p_decoder_data;
			return NULL;
		}
		p_decoder_data->mb_data_completed = false;
		p_decoder_data->mh_read.Open(sz_file_name, false);
		if (!(p_decoder_data->mh_read))
		{
			//DWORD DW_Res = GetLastError();
			//sLog(_T("打开在线文件失败:%s,错误码<%d>"),s_file_name.GetBuffer(),DW_Res);
			p_decoder_data->mh_write.Close();
			delete p_decoder_data;
			return NULL;
		}
	}
	else
	{
		
		//获得mp3时长、音频部分的开始位置和结束位置 
		em_mp3_tag::mp3_file temp_mp3_file;
		temp_mp3_file.load(s_file_name.c_str(),true,true);
		em_mp3_tag::check_result check_info = temp_mp3_file.check_content(false, false);
		p_decoder_data->mn_mp3_second = check_info.n_seconds_length;
		p_decoder_data->mn_audio_begin = temp_mp3_file.get_audio_begin();
		p_decoder_data->mn_audio_end = temp_mp3_file.get_audio_end();
		p_decoder_data->mn_cur_position = p_decoder_data->mn_audio_begin;
		//bool bRet = temp_mp3_file.rip_id3v2_pic(L"d:\\temp\\my.jpg");
		temp_mp3_file.unload();
		
		p_decoder_data->mh_read.Open(s_file_name.c_str(), true);
		if(!(p_decoder_data->mh_read))
		{
			//DWORD DW_Res = GetLastError();
			//sLog(_T("打开文件失败:%s,错误码<%d>"),s_file_name.GetBuffer(),DW_Res);
			delete p_decoder_data;
			return NULL;
		}

		p_decoder_data->mn_file_size = p_decoder_data->mh_read.GetSize();
		p_decoder_data->mb_data_completed = true;
	}
	mad_stream_init(&p_decoder_data->m_stream);
	mad_frame_init(&p_decoder_data->m_frame);
	mad_synth_init(&p_decoder_data->m_synth);
	mad_timer_reset(&p_decoder_data->m_timer);


	//sLog(L"Mp3Open<this:%d,filename:%s>",p_decoder_data,s_file_name.GetBuffer());
	return p_decoder_data;
}

static int mp3_get_audio_info(decoder_handle handle,struct player_audio_info* p_info)
{
	if(p_info == NULL)
		return DECODER_ERROR;
	mp3_decoder_data *p_decoder_data = (mp3_decoder_data *)handle;
	if(p_decoder_data == NULL)
		return DECODER_ERROR;
	if(p_decoder_data->mp_audio_info == NULL)
	{
		return DECODER_WAIT;
	}
	else
	{
		memcpy(p_info,p_decoder_data->mp_audio_info ,sizeof(struct player_audio_info));
		return DECODER_SUCCEED;
	}
}

static inline signed short MadFixedToSshort(mad_fixed_t Fixed) 
{
    /* A fixed point number is formed of the following bit pattern:
     *
     * SWWWFFFFFFFFFFFFFFFFFFFFFFFFFFFF
     * MSB                          LSB
     * S ==> Sign (0 is positive, 1 is negative)
     * W ==> Whole part bits
     * F ==> Fractional part bits
     *
     * This pattern contains MAD_F_FRACBITS fractional bits, one
     * should alway use this macro when working on the bits of a fixed
     * point number. It is not guaranteed to be constant over the
     * different platforms supported by libmad.
     *
     * The signed short value is formed, after clipping, by the least
     * significant whole part bit, followed by the 15 most significant
     * fractional part bits. Warning: this is a quick and dirty way to
     * compute the 16-bit number, madplay includes much better
     * algorithms.
     */

    /* Clipping */
    if(Fixed>=MAD_F_ONE)
        return(SHRT_MAX);
    if(Fixed<=-MAD_F_ONE)
        return(-SHRT_MAX);

    /* Conversion. */
    Fixed=Fixed>>(MAD_F_FRACBITS-15);
    return((signed short)Fixed);
}

static int mp3_decode_once(decoder_handle handle)
{
	mp3_decoder_data *p_decoder_data = (mp3_decoder_data *)handle;
	
	if(p_decoder_data == NULL)
		return DECODER_ERROR;
	if(p_decoder_data->m_stream.buffer==NULL || p_decoder_data->m_stream.error==MAD_ERROR_BUFLEN)
	{
		int	n_read_size,
			n_remaining;
		unsigned char *p_read_start = NULL;
	
		if(p_decoder_data->m_stream.next_frame!=NULL)
		{
			n_remaining=p_decoder_data->m_stream.bufend - p_decoder_data->m_stream.next_frame;
			memmove(p_decoder_data->mp_input_buffer,p_decoder_data->m_stream.next_frame,n_remaining);
			p_read_start=p_decoder_data->mp_input_buffer+n_remaining;
			n_read_size=INPUT_BUFFER_SIZE-n_remaining;
		}
		else
		{
			n_read_size=INPUT_BUFFER_SIZE;
			p_read_start=p_decoder_data->mp_input_buffer;
			n_remaining=0;
		}

		
		int dwRet = p_decoder_data->mh_read.Seek(p_decoder_data->mn_cur_position, _FStd(FFile)::ENUM_SEEK::SEEK_FILE_BEGIN);
		if (dwRet == -1)
		{
			return DECODER_WAIT;
		}

		n_read_size = p_decoder_data->mh_read.Read(p_read_start,n_read_size);
		if(n_read_size == 0)
		{	
			return DECODER_WAIT;
		}
		p_decoder_data->mn_cur_position +=n_read_size;

		mad_stream_buffer(&p_decoder_data->m_stream,p_decoder_data->mp_input_buffer,n_read_size+n_remaining);
		p_decoder_data->m_stream.error=(mad_error)0;
	}

	if(mad_frame_decode(&p_decoder_data->m_frame,&p_decoder_data->m_stream))
	{
		if(MAD_RECOVERABLE(p_decoder_data->m_stream.error))
		{
			if(p_decoder_data->m_stream.error!=MAD_ERROR_LOSTSYNC ||p_decoder_data->m_stream.this_frame!=p_decoder_data->mp_guard_ptr)
			{
				/*if (p_decoder_data->b_FirstDecode)
				{
					p_decoder_data->b_FirstDecode = false;
					return XIAMI_DECODER_ERROR;
				}*/
				p_decoder_data->nErrorCnt++;
				fflush(stderr);
				if(p_decoder_data->nErrorCnt++ > 100)
					return DECODER_ERROR;
			}
			return DECODER_WAIT;
		}
		else
		{
			if(p_decoder_data->m_stream.error==MAD_ERROR_BUFLEN)
			{
				return DECODER_WAIT;
			}
			else
			{
				return DECODER_ERROR;
			}
		}
	}
	p_decoder_data->b_FirstDecode = false;
	p_decoder_data->nErrorCnt = 0;
	mad_timer_add(&p_decoder_data->m_timer,p_decoder_data->m_frame.header.duration);
	mad_synth_frame(&p_decoder_data->m_synth,&p_decoder_data->m_frame);

	int n_channels = p_decoder_data->m_synth.pcm.channels;
	if(n_channels < 2)
	{
		n_channels = 2;
	}
	int n_samples = p_decoder_data->m_synth.pcm.length * n_channels ;
	if(n_samples <= 0)
	{
		return DECODER_WAIT;
	}
	pcm_data_buf *p_data_buf = new pcm_data_buf(n_samples*sizeof(unsigned short));
	if(p_data_buf == NULL)
		return DECODER_ERROR;

	unsigned short *p_pcm_16 = (unsigned short *)p_data_buf->get_buf();
	if(p_pcm_16 == NULL)
		return DECODER_ERROR;
	
	for(int c=0;c < p_decoder_data->m_synth.pcm.channels;c++)
	{
		for(int k=0;k<p_decoder_data->m_synth.pcm.length;k++)
		{
			p_pcm_16[k * n_channels + c]=MadFixedToSshort(p_decoder_data->m_synth.pcm.samples[c][k]);
		}
	}
	for(int c=p_decoder_data->m_synth.pcm.channels;c < n_channels;c++)
	{
		for(int k=0;k<p_decoder_data->m_synth.pcm.length;k++)
		{
			p_pcm_16[k * n_channels +c]=MadFixedToSshort(p_decoder_data->m_synth.pcm.samples[0][k]);
		}
	}
	
	p_decoder_data->m_raw_pcm_data_list.push_back(p_data_buf);
	p_decoder_data->mn_availiable_bytes_count += n_samples*sizeof(unsigned short);
	if(p_decoder_data->mp_audio_info == NULL)
	{
		p_decoder_data->mp_audio_info = new player_audio_info();
		p_decoder_data->mp_audio_info->n_size = sizeof(player_audio_info);
		p_decoder_data->mp_audio_info->n_channal =  n_channels;
		p_decoder_data->mp_audio_info->n_sample_rate = p_decoder_data->m_synth.pcm.samplerate;
		p_decoder_data->mp_audio_info->n_sample_size_in_bit = sizeof(short)*8;
		p_decoder_data->mp_audio_info->n_total_play_time_in_ms = 0;
		p_decoder_data->mp_audio_info->n_file_size = p_decoder_data->mn_file_size;
		p_decoder_data->mp_audio_info->n_total_play_time_in_ms = p_decoder_data->mn_mp3_second * 1000;
	}
	if(p_decoder_data->mb_online && p_decoder_data->mn_download_file_size > 0)
	{
		p_decoder_data->mn_file_size = p_decoder_data->mn_download_file_size;
		p_decoder_data->mp_audio_info->n_file_size = p_decoder_data->mn_download_file_size;
		if (p_decoder_data->mn_download_file_size - p_decoder_data->mn_audio_begin != 0)
			p_decoder_data->mf_cur_percent = (float)(p_decoder_data->mn_cur_position - p_decoder_data->mn_audio_begin)/ (p_decoder_data->mn_download_file_size - p_decoder_data->mn_audio_begin);
	}
	else
	{
		if (p_decoder_data->mn_audio_end - p_decoder_data->mn_audio_begin != 0)
			p_decoder_data->mf_cur_percent = (float)(p_decoder_data->mn_cur_position - p_decoder_data->mn_audio_begin)/ (p_decoder_data->mn_audio_end - p_decoder_data->mn_audio_begin);
	}
	return n_samples; 
}

static int mp3_get_available_samples_count(decoder_handle handle)
{
	mp3_decoder_data *p_decoder_data = (mp3_decoder_data *)handle;
	
	if(p_decoder_data != NULL)
	{
		//return p_decoder_data->mn_availiable_bytes_count/sizeof(short);
		return p_decoder_data->m_raw_pcm_data_list.size();
	}
	return DECODER_ERROR;
}

static int mp3_get_data(decoder_handle handle,unsigned char *p_data,int n_buf_size)
{
	mp3_decoder_data *p_decoder_data = (mp3_decoder_data *)handle;
	
	if(p_decoder_data == NULL)
		return 0;
	int n_bytes_write = 0;
	while(n_bytes_write < n_buf_size && p_decoder_data->m_raw_pcm_data_list.size() > 0)
	{
		int n_remain_count = n_buf_size - n_bytes_write;
		pcm_data_buf *p_buf =  *p_decoder_data->m_raw_pcm_data_list.begin();
		if(p_buf == NULL)
			return DECODER_ERROR;
		n_bytes_write += p_buf->copy_data(p_data+n_bytes_write,n_remain_count);
		if(p_buf->empty())
		{
			p_decoder_data->m_raw_pcm_data_list.pop_front();
			delete p_buf;
		}
	}
	p_decoder_data->mn_availiable_bytes_count-=(n_bytes_write);

	return n_bytes_write;
}

static int mp3_write_data(decoder_handle handle,unsigned char *p_data,int n_buf_size)
{
	mp3_decoder_data *p_decoder_data = (mp3_decoder_data *)handle;
	if(p_decoder_data == NULL)
		return DECODER_ERROR;
	if(p_data == NULL )
	{
		if(p_decoder_data->mb_online == true)
		{
			if(n_buf_size == 0) //download finished
			{
				p_decoder_data->mb_data_completed = true;
				//获得mp3时长、音频部分的开始位置和结束位置 

				if (!(p_decoder_data->mh_write))
				{
					p_decoder_data->mh_write.Close();
				}
				em_mp3_tag::mp3_file temp_mp3_file;
				temp_mp3_file.load(p_decoder_data->s_file_name.c_str(),true,true);
				em_mp3_tag::check_result check_info = temp_mp3_file.check_content(false, false);
				p_decoder_data->mn_mp3_second = check_info.n_seconds_length;
				temp_mp3_file.unload();
				if(p_decoder_data->mp_audio_info != NULL)
				{
					p_decoder_data->mp_audio_info->n_total_play_time_in_ms = p_decoder_data->mn_mp3_second * 1000;
				}
				return DECODER_WAIT;
			}
			else
			{
				p_decoder_data->mn_download_file_size = n_buf_size;
				p_decoder_data->mn_file_size = n_buf_size;
				return DECODER_WAIT;
			}
		}
		else
		{
			return DECODER_ERROR;
		}
	}
	int n_cur_pos = 0;
	while(n_cur_pos < n_buf_size)
	{
		if (!(p_decoder_data->mh_write))
			return 0;
		long n_result = 0;
		n_result = p_decoder_data->mh_write.Write(p_data+n_cur_pos,n_buf_size - n_cur_pos);
		p_decoder_data->mh_write.Flush();

		if(n_result < 0)
			return DECODER_ERROR;
		n_cur_pos += n_result;
		p_decoder_data->mn_cur_download_bytes += n_result;
	}
	return n_cur_pos;
}

static __int64 mp3_seek(decoder_handle handle,float f_percent)
{
	mp3_decoder_data *p_decoder_data = (mp3_decoder_data *)handle;
	if(p_decoder_data == NULL)
		return DECODER_ERROR;
	if(f_percent > 1.0f) return DECODER_ERROR;
	int nTemp = f_percent*p_decoder_data->mp_audio_info->n_file_size;
	if (p_decoder_data->mb_online && nTemp > p_decoder_data->mn_cur_download_bytes)
		return DECODER_ERROR;
	int n_position = (nTemp / 2) *2;
	p_decoder_data->mn_cur_position = n_position;

	while(p_decoder_data->m_raw_pcm_data_list.size() > 0)
	{
		pcm_data_buf *p_buf =  *p_decoder_data->m_raw_pcm_data_list.begin();
		delete p_buf;
		p_decoder_data->m_raw_pcm_data_list.pop_front();
	}
	return DECODER_SUCCEED;
}

static bool mp3_is_finish(decoder_handle handle)
{
	mp3_decoder_data *p_decoder_data = (mp3_decoder_data *)handle;
	if(p_decoder_data == NULL)
		return false;
	if(p_decoder_data->m_raw_pcm_data_list.size() > 0)
		return false;
	
	if(p_decoder_data->mb_online )
	{
		if(p_decoder_data->mb_data_completed)
		{
			assert(p_decoder_data->mn_file_size >= p_decoder_data->mn_cur_position);
		}
		bool bRet = (p_decoder_data->mn_file_size <= p_decoder_data->mn_cur_position && p_decoder_data->mb_data_completed);
		return  bRet;
	}
	else
	{
		return  p_decoder_data->mn_file_size == p_decoder_data->mn_cur_position ;
	}
}

static __int64 mp3_get_download_data_length(decoder_handle handle)
{
	mp3_decoder_data *p_decoder_data = (mp3_decoder_data *)handle;
	if(p_decoder_data == NULL)
		return DECODER_ERROR;
	if(p_decoder_data->mb_online)
	{

	}
	else
	{
		return DECODER_ERROR ;
	}
}

static __int64 mp3_get_current_position(decoder_handle handle)
{

	mp3_decoder_data *p_decoder_data = (mp3_decoder_data *)handle;
	if(p_decoder_data == NULL)
		return DECODER_ERROR;
	return p_decoder_data->mn_cur_position;
}

static void mp3_close(decoder_handle handle)
{

	mp3_decoder_data *p_decoder_data = (mp3_decoder_data *)handle;
	if(p_decoder_data == NULL)
	{
		assert(false);
		return;
	}

//	sLog(L"Mp3Close<this:%d filename:%s>",p_decoder_data,p_decoder_data->s_file_name.GetBuffer());

	mad_synth_finish(&p_decoder_data->m_synth);
	mad_frame_finish(&p_decoder_data->m_frame);
	mad_stream_finish(&p_decoder_data->m_stream);

	if ((p_decoder_data->mh_read) )
	{
		p_decoder_data->mh_read.Close();
	}
	if ((p_decoder_data->mh_write) )
	{
		p_decoder_data->mh_write.Close();
	}

	if(p_decoder_data->mp_audio_info != NULL)
	{
		delete p_decoder_data->mp_audio_info;
	}
	while (p_decoder_data->m_raw_pcm_data_list.size())
	{
		pcm_data_buf *p_buf =  *p_decoder_data->m_raw_pcm_data_list.begin();
		p_decoder_data->m_raw_pcm_data_list.pop_front();
		delete p_buf;
	}
//	sLog(L"Mp3CloseEnd<this:%d filename:%s>",p_decoder_data,p_decoder_data->s_file_name.GetBuffer());
	delete p_decoder_data;

}
static float mp3_get_percent(decoder_handle handle)
{
	mp3_decoder_data *p_decoder_data = (mp3_decoder_data *)handle;
	if(p_decoder_data == NULL)
		return 0;
	return p_decoder_data->mf_cur_percent;
}
static int mp3_get_decoder_type()
{
	return DT_MPEG;
}

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
	p_plugin->open = mp3_open;
	p_plugin->get_audio_info = mp3_get_audio_info;
	p_plugin->decode_once = mp3_decode_once;
	p_plugin->get_available_samples_count = mp3_get_available_samples_count;
	p_plugin->get_data = mp3_get_data;
	p_plugin->write_data = mp3_write_data;
	p_plugin->get_download_data_length = mp3_get_download_data_length;
	p_plugin->get_current_position = mp3_get_current_position;
	p_plugin->is_finish = mp3_is_finish;
	p_plugin->close = mp3_close;
	p_plugin->seek = mp3_seek;
	p_plugin->get_percent = mp3_get_percent;
	p_plugin->get_decoder_type = mp3_get_decoder_type;
	return p_plugin;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */ 