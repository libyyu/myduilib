#pragma once
#include <string>

namespace em_mp3_tag
{
class io_base;

enum support_tag_e
{
	st_id3_v1,
	st_id3_v2,
	st_ape ,
	st_lyric3
};
enum mpeg_version_e
{
	mv_mpeg_25 = 0,
	mv_mpeg_reserved,
	mv_mpeg_2,
	mv_mpeg_1		
};

enum mpeg_layer_e
{
	ml_layer_1,
	ml_layer_2,
	ml_layer_3,
	ml_layer_reserved
};

enum emphasis_mode_e
{
	em_none = 0,
	em_5015,
	em_reserved,
	em_CCITJ17
};

//通道模式枚举
enum channel_mode_e
{
	cm_stereo,							//立体声
	cm_joint_stereo,					//联合立体声
	cm_dual_channel,				//双声道
	cm_single_channel				//单声道
};

enum check_error_e
{
	ce_CRC_error,					//CRC校验错
	ce_sample_change,				//采样率改变
	ce_layer_change,				//layer改变
	ce_version_change,				//mpeg版本改变
	ce_frame_offset,					//帧偏移异常
	ce_last_truncated,				//尾帧被截断
	ce_free_space,					//尾帧后有剩余空间
	ce_seek_failed,					//无法定位到帧
	ce_success							//文件正常
};

#define TAG_ID3_V1_LEN 128
#define TAG_ID3_V2_MIN 10
#define INVALID_INDEX 0xFFFFFFFF
#define TAG_ID3_V2_LEN 512

struct tag_base_data
{
	static const unsigned short MAX_SONG_COUNT = 128;
	static const unsigned short MAX_DISK_COUNT = 64;
	std::string s_artist;						//演唱歌手(多个歌手的话，有分号分隔)
	std::string s_album;						//专辑名称
	std::string s_title;						//歌曲名称
	std::string s_date;						//发行日期(格式"2005" or "200502" or "2005.02" or "2005.2")
	unsigned short n_track;				//该歌曲在所属CD里的序号(从1开始)
	unsigned short n_song_count;		//该歌曲所属CD的歌曲数量
	std::string s_genre;						//流派
	int n_genre_index;						//流派序号
	std::string s_album_artist;			//专辑歌手
	unsigned int n_disk_serial;			//该歌曲所属的CD序号(如该值为0，则该专辑只有一张CD，等同于1)
	unsigned int n_disk_count;			//该歌曲所属专辑的CD总数（如该值为0，则该专辑只有一张CD，等同于1）
	unsigned int n_gmt_publish;		//gmt格式的发布时间
	std::string s_comment;				//注释(对于多CD专辑，注释的格式为“专辑名-CD名”，如CD没有独立的名字，则用"Disc n"代替)
	void clear();
	void generate_gmt_publish();
};
struct tag_base_data_w
{
	static const unsigned short MAX_SONG_COUNT = 128;
	static const unsigned short MAX_DISK_COUNT = 64;
	std::wstring s_artist;						//演唱歌手(多个歌手的话，有分号分隔)
	std::wstring s_album;						//专辑名称
	std::wstring s_title;						//歌曲名称
	std::wstring s_date;						//发行日期(格式"2005" or "200502" or "2005.02" or "2005.2")
	unsigned short n_track;				//该歌曲在所属CD里的序号(从1开始)
	unsigned short n_song_count;		//该歌曲所属CD的歌曲数量
	std::wstring s_genre;						//流派
	int n_genre_index;						//流派序号
	std::wstring s_album_artist;			//专辑歌手
	unsigned int n_disk_serial;			//该歌曲所属的CD序号(如该值为0，则该专辑只有一张CD，等同于1)
	unsigned int n_disk_count;			//该歌曲所属专辑的CD总数（如该值为0，则该专辑只有一张CD，等同于1）
	unsigned int n_gmt_publish;		//gmt格式的发布时间
	std::wstring s_comment;				//注释(对于多CD专辑，注释的格式为“专辑名-CD名”，如CD没有独立的名字，则用"Disc n"代替)
	void clear();
	void generate_gmt_publish();
};
struct const_enum_map
{
public:
	static const char* ma_mpeg_versions[];
	static const char* ma_mpeg_layers[];
	static const char* ma_channel_modes[];
	static const char* ma_emphasis[];
	static const char* ma_tags[];
	static const char* ma_check_results[];
};

//mp3 tag通用接口
class mp3_tag_interface
{
public:
	virtual ~mp3_tag_interface() {}
	virtual int get_length() const = 0;
	virtual int get_offset() const = 0;
	virtual bool is_append() const = 0;
	virtual tag_base_data* get_base_tag() = 0;
	virtual tag_base_data_w* get_base_tag_wstring() = 0;//新加

	virtual std::string get_tag_name() = 0;
	virtual std::wstring get_tag_name_wstring() = 0;//新加

	virtual unsigned int get_tag_serial() = 0;
public:
	static const int MAX_FIELD_VALUE_SIZE = 1024 * 10;
	static const int MAX_TAG_ITEM_SIZE = 1024 * 1024;			//无论任何元素，1M最大
};

//mp3 tag工厂接口
class mp3_tag_factory
{
public:
	virtual ~mp3_tag_factory() {}
	//n_begin和n_end都是相对文件头的偏移
	virtual mp3_tag_interface* create_tag(io_base* p_file, unsigned int n_begin, unsigned int n_end) = 0;
};

}		//end namespace em_mp3_tag
