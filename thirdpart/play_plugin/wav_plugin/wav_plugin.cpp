#include "decoder_plugin.h"
#include <list>
#include <string>
#include <algorithm>
#ifdef _MSC_VER
#include <Windows.h>
#else
#endif
#include "flib.h"

class pcm_data_buf;

struct wav_decoder_data 
{
	int mn_size;
	char buf_head[44];
	char buf_data[1024*64];
	int nSampleRate;
	int byte_read;
	int mn_availiable_bytes_count;
	__int64 mn_cur_position;
	float mf_cur_percent;
	bool mb_online;
	bool mb_data_completed;
	int mn_audio_begin;
	int mn_audio_end;
	int mn_file_size;
	std::string s_file_name;
	int mn_download_file_size;
	int mn_cur_rec_bytes;
	int mn_cur_download_bytes;

	std::list<pcm_data_buf *> m_raw_pcm_data_list;
	_FStd(FFile) mh_write;
	_FStd(FFile) mh_read;
	player_audio_info *mp_audio_info;
};


static decoder_handle wav_open(const char * sz_file_name,bool b_is_online,int nFileType,int nBegin,int nEnd);
static int wav_get_audio_info(decoder_handle handle,struct player_audio_info* p_info);
static int wav_decode_once(decoder_handle handle);
static int wav_get_available_samples_count(decoder_handle handle);
static int wav_get_data(decoder_handle handle,unsigned char *p_data,int n_buf_size);
static int wav_write_data(decoder_handle handle,unsigned char *p_data,int n_buf_size);
static __int64 wav_seek(decoder_handle handle,float f_percent);
static __int64 wav_get_download_data_length(decoder_handle handle);
static __int64 wav_get_current_position(decoder_handle handle);
static bool wav_is_finish(decoder_handle handle);
static void wav_close(decoder_handle handle);
static float wav_get_percent(decoder_handle handle);
static int wav_get_decoder_type();

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
		p_plugin->open = wav_open;
		p_plugin->get_audio_info = wav_get_audio_info;
		p_plugin->decode_once = wav_decode_once;
		p_plugin->get_available_samples_count = wav_get_available_samples_count;
		p_plugin->get_data = wav_get_data;
		p_plugin->write_data = wav_write_data;
		p_plugin->get_download_data_length = wav_get_download_data_length;
		p_plugin->get_current_position = wav_get_current_position;
		p_plugin->is_finish = wav_is_finish;
		p_plugin->close = wav_close;
		p_plugin->seek = wav_seek;
		p_plugin->get_percent = wav_get_percent;
		p_plugin->get_decoder_type = wav_get_decoder_type;
		return p_plugin;
	}
#ifdef __cplusplus
}
#endif /* __cplusplus */ 


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
		memcpy(p_data, mp_data + mn_cur_position, n_copy);
		mn_cur_position += n_copy;
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

static bool init_wav_decoder_data(wav_decoder_data* p_decoder_data, _FStd(FFile)* mh_read)
{
	p_decoder_data->mn_file_size = mh_read->GetSize();
	p_decoder_data->mp_audio_info->n_file_size = p_decoder_data->mn_file_size;

	long dwRead = mh_read->Read(p_decoder_data->buf_head, 44);
	if (dwRead < 44)
	{
		return false;
	}

	if ((p_decoder_data->buf_head[0] != 'R' ||
		p_decoder_data->buf_head[1] != 'I' ||
		p_decoder_data->buf_head[2] != 'F' ||
		p_decoder_data->buf_data[3] != 'F') &&
		(p_decoder_data->buf_head[8] != 'W' ||
			p_decoder_data->buf_head[9] != 'A' ||
			p_decoder_data->buf_head[10] != 'V' ||
			p_decoder_data->buf_head[11] != 'E'))
	{
		return false;
	}
	p_decoder_data->mp_audio_info->n_channal = *((p_decoder_data->buf_head) + 22);
	p_decoder_data->mp_audio_info->n_sample_size_in_bit = *(p_decoder_data->buf_head + 34);
	if (p_decoder_data->mp_audio_info->n_sample_size_in_bit == 0)
	{
		return false;
	}
	mh_read->Seek(-20, _FStd(FFile)::ENUM_SEEK::SEEK_FILE_CURRENT);
	dwRead = mh_read->Read(&p_decoder_data->nSampleRate, sizeof(int));

	p_decoder_data->mp_audio_info->n_sample_rate = p_decoder_data->nSampleRate;
	p_decoder_data->byte_read = 0;
	p_decoder_data->mn_cur_position = 44;
	p_decoder_data->mf_cur_percent = 0.0;
	if (p_decoder_data->mp_audio_info->n_channal != 0 &&
		p_decoder_data->mp_audio_info->n_sample_size_in_bit != 0 &&
		p_decoder_data->mp_audio_info->n_sample_rate != 0
		)
	{
		__int64 nTemp1 = ((__int64)p_decoder_data->mp_audio_info->n_file_size - 44) * 8000;
		p_decoder_data->mp_audio_info->n_total_play_time_in_ms = nTemp1 / (p_decoder_data->mp_audio_info->n_channal * p_decoder_data->mp_audio_info->n_sample_rate * p_decoder_data->mp_audio_info->n_sample_size_in_bit);
	}
	p_decoder_data->mn_audio_begin = p_decoder_data->mn_cur_position;
	p_decoder_data->mn_audio_end = p_decoder_data->mp_audio_info->n_total_play_time_in_ms;
	p_decoder_data->mb_data_completed = true;

	return true;
}

static decoder_handle wav_open(const char * sz_file_name,bool is_online,int nFileType,int nBegin,int nEnd)
{
	if(sz_file_name == NULL)
		return NULL;
	std::string s_file_name = sz_file_name;
	if (s_file_name.length() < 4)
		return NULL;
	std::transform(s_file_name.begin(), s_file_name.end(), s_file_name.begin(), ::tolower);
	auto pos2 = s_file_name.rfind('.');
	std::string ext = pos2 != s_file_name.npos ? s_file_name.substr(pos2) : "";
	if (ext != ".wav")
		return NULL;

	wav_decoder_data *p_decoder_data = new wav_decoder_data();
	if(p_decoder_data == NULL)
		return NULL;
	p_decoder_data->mn_size = sizeof(wav_decoder_data);
	p_decoder_data->s_file_name = s_file_name;
	p_decoder_data->mp_audio_info = NULL;
	p_decoder_data->mn_cur_position = 0;
	p_decoder_data->mf_cur_percent = 0.0;
	p_decoder_data->mn_availiable_bytes_count = 0;
	p_decoder_data->mn_file_size = 0;
	p_decoder_data->mn_audio_begin = 0;
	p_decoder_data->mn_audio_end = 0;
	p_decoder_data->mn_download_file_size = 0;
	p_decoder_data->mn_cur_download_bytes = 0;
	p_decoder_data->mn_cur_rec_bytes = 0;
	p_decoder_data->mb_online = is_online;
	p_decoder_data->mp_audio_info = new player_audio_info();
	if(p_decoder_data->mp_audio_info == NULL)
	{
		delete p_decoder_data;
		return NULL;
	}
	
	memset(p_decoder_data->buf_head,0,sizeof(p_decoder_data->buf_head));
	memset(p_decoder_data->buf_data,0,sizeof(p_decoder_data->buf_data));

	if (p_decoder_data->mb_online)
	{
		p_decoder_data->mh_write.Open(sz_file_name, false);
		if (!(p_decoder_data->mh_write))
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
		p_decoder_data->mh_read.Open(sz_file_name, true);
		if (!(p_decoder_data->mh_read))
		{
			delete p_decoder_data;
			return NULL;
		}
		
		if (!init_wav_decoder_data(p_decoder_data, &(p_decoder_data->mh_read)))
		{
			p_decoder_data->mh_read.Close();
			delete p_decoder_data;
			return NULL;
		}
	}
	return p_decoder_data;
}
static int wav_get_audio_info(decoder_handle handle,struct player_audio_info* p_info)
{
	if(p_info == NULL)
		return DECODER_ERROR;
	wav_decoder_data *p_decoder_data = (wav_decoder_data *)handle;
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
static int wav_decode_once(decoder_handle handle)
{
	wav_decoder_data *p_decoder_data = (wav_decoder_data *)handle;

	if(p_decoder_data == NULL)
		return DECODER_ERROR;
	p_decoder_data->mh_read.Seek(p_decoder_data->mn_cur_position, _FStd(FFile)::ENUM_SEEK::SEEK_FILE_BEGIN);
	p_decoder_data->byte_read = p_decoder_data->mh_read.Read(p_decoder_data->buf_data,sizeof(p_decoder_data->buf_data));
	p_decoder_data->mf_cur_percent = (float)p_decoder_data->mn_cur_position / p_decoder_data->mp_audio_info->n_file_size;
	
	if (p_decoder_data->byte_read > 0)
	{
		pcm_data_buf *p_data_buf = new pcm_data_buf(p_decoder_data->byte_read);
		if(p_data_buf == NULL)
			return DECODER_ERROR;
		memcpy(p_data_buf->get_buf(),p_decoder_data->buf_data,p_decoder_data->byte_read);
		p_decoder_data->m_raw_pcm_data_list.push_back(p_data_buf);
		p_decoder_data->mn_cur_position += p_decoder_data->byte_read;
		int nRes = p_decoder_data->mp_audio_info->n_sample_size_in_bit/8;
		if(nRes > 0)
			return p_decoder_data->byte_read/(nRes);
	}
	return DECODER_WAIT;
}
static int wav_get_available_samples_count(decoder_handle handle)
{
	wav_decoder_data *p_decoder_data = (wav_decoder_data *)handle;
	if (p_decoder_data == NULL)
	{
		return DECODER_ERROR;
	}
	else
	{
		//return  (p_decoder_data->byte_read)/(p_decoder_data->mp_audio_info->n_sample_size_in_bit/8);
		return p_decoder_data->m_raw_pcm_data_list.size();
	}

}
static int wav_get_data(decoder_handle handle,unsigned char *p_data,int n_buf_size)
{
	wav_decoder_data *p_decoder_data = (wav_decoder_data*)handle;
	if (!p_decoder_data)
	{
		return DECODER_ERROR;
	}
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
	p_decoder_data->mn_availiable_bytes_count-=n_bytes_write;

	return n_bytes_write;
}
static int wav_write_data(decoder_handle handle,unsigned char *p_data,int n_buf_size)
{
	wav_decoder_data* p_decoder_data = (wav_decoder_data*)handle;
	if (p_decoder_data == NULL)
		return DECODER_ERROR;
	if (p_data == NULL)
	{
		if (p_decoder_data->mb_online == true)
		{
			if (n_buf_size == 0) //download finished
			{
				p_decoder_data->mb_data_completed = true;
				//获得wav时长、音频部分的开始位置和结束位置 

				if (!(p_decoder_data->mh_write))
				{
					p_decoder_data->mh_write.Close();
				}
				_FStd(FFile) mh_read;
				mh_read.Open(p_decoder_data->s_file_name.c_str(), true);
				if ((mh_read))
				{
					init_wav_decoder_data(p_decoder_data, &(mh_read));
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
	while (n_cur_pos < n_buf_size)
	{
		if (!(p_decoder_data->mh_write))
			return 0;
		long n_result = 0;
		n_result = p_decoder_data->mh_write.Write(p_data + n_cur_pos, n_buf_size - n_cur_pos);
		p_decoder_data->mh_write.Flush();

		if (n_result < 0)
			return DECODER_ERROR;
		n_cur_pos += n_result;
		p_decoder_data->mn_cur_download_bytes += n_result;
	}
	return n_cur_pos;

	return DECODER_ERROR;
}
static __int64 wav_seek(decoder_handle handle,float f_percent)
{
	wav_decoder_data *p_decoder_data = (wav_decoder_data*)handle;
	if (p_decoder_data == NULL)
	{
		return DECODER_ERROR;
	}

	p_decoder_data->mn_cur_position = f_percent * (p_decoder_data->mp_audio_info->n_file_size - 44);
    switch (p_decoder_data->mp_audio_info->n_sample_size_in_bit)
    {
	case 8:
    case 16:
		p_decoder_data->mn_cur_position = p_decoder_data->mn_cur_position / 2 * 2;
    	break;
	case 24:
		p_decoder_data->mn_cur_position = p_decoder_data->mn_cur_position / 3 * 3 + 44;
		break;
	case 32:
		p_decoder_data->mn_cur_position = p_decoder_data->mn_cur_position / 4 * 4;
    }
    
	while(p_decoder_data->m_raw_pcm_data_list.size() > 0)
	{
		pcm_data_buf *p_buf =  *p_decoder_data->m_raw_pcm_data_list.begin();
		delete p_buf;
		p_decoder_data->m_raw_pcm_data_list.pop_front();
	}
	return DECODER_SUCCEED;
}
static __int64 wav_get_download_data_length(decoder_handle handle)
{
	return DECODER_ERROR;
}
static __int64 wav_get_current_position(decoder_handle handle)
{
	wav_decoder_data *p_decoder_data = (wav_decoder_data *)handle;
	if(p_decoder_data == NULL)
		return DECODER_ERROR;
	return p_decoder_data->mn_cur_position;
}
static bool wav_is_finish(decoder_handle handle)
{
	wav_decoder_data *p_decoder_data = (wav_decoder_data *)handle;
	if(p_decoder_data == NULL)
		return true;
	if(p_decoder_data->m_raw_pcm_data_list.size() > 0)
		return false;
	if(p_decoder_data->mb_online)
	{
		return p_decoder_data->mp_audio_info->n_file_size == p_decoder_data->mn_cur_position && p_decoder_data->mb_data_completed;
	}
	else
	{
		return p_decoder_data->mp_audio_info->n_file_size == p_decoder_data->mn_cur_position ;
	}
}
static void wav_close(decoder_handle handle)
{
	wav_decoder_data *p_decoder_data = (wav_decoder_data *)handle;
	if(p_decoder_data == NULL)
		return;

	if(!(p_decoder_data->mh_read))
	{
		p_decoder_data->mh_read.Close();
	}
	if ((p_decoder_data->mh_write))
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
	delete p_decoder_data;
}
static float wav_get_percent(decoder_handle handle)
{
	wav_decoder_data *p_decoder_data = (wav_decoder_data *)handle;
	if(p_decoder_data == NULL)
		return 0;
	return p_decoder_data->mf_cur_percent;
}
static int wav_get_decoder_type()
{
	return DT_WAV;
}