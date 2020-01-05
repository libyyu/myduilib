#pragma once
#include "tag_base.h"
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace em_mp3_tag
{
class CMPAFrame;

enum encode_bitrate_e
{
	eb_CBR,								//固定编码率
	eb_ABR,								//段内固定编码率
	eb_VBR								//可变编码率
};

struct check_result
{
	check_error_e e_result; // 检查结果
	unsigned int n_err_frame; // 出错帧
	unsigned int n_frames; // 总帧数
	unsigned int n_unpadding_frames; // 未填充帧数
	unsigned int n_padding_frames; // 填充帧数
	encode_bitrate_e e_bitrate_type; // 编码类型
	unsigned short n_bitrate; // 比特率
	unsigned int max_bitrate; // 最大编码率
	unsigned int min_bitrate; // 最小编码率
	unsigned int average_bitrate; // 平均编码率
	unsigned int n_seconds_length; // 总时间
	int n_extend; // 扩展值(由各个检查结果自定义该值含义)
	int n_adjust_result; // 1为进行了adjust；-1为尾部有tag，无法直接adjust；0为没有adjust
};

struct mpeg_info
{
	mpeg_version_e mpa_version;			//mpeg版本
	mpeg_layer_e mpa_layer;				//layer
	channel_mode_e channel_mode;		//立体声
	unsigned int bit_rate;						//比特率
	unsigned int sample_rate;				//采样率
	encode_bitrate_e e_bitrate_type;		//编码类型
	unsigned int bit_rate_in_vbr;			//VBR header中的比特率
	unsigned int seconds_length;		//播放时间
};

class mp3_file
{
	friend class tag_manager;
public:
	static const unsigned int CHECK_MINI_FRAMES = 1;
	typedef boost::shared_ptr<mp3_tag_interface> tag_ptr_type;
	typedef std::map<unsigned int, tag_ptr_type> tag_container_type;
	typedef tag_container_type::iterator tag_iter;
public:
	mp3_file(unsigned int n_check_frames = CHECK_MINI_FRAMES);
	~mp3_file();

	bool load(const char* p_file_name, bool b_parse, bool b_read_only,bool b_parse_tag_only = false);
	bool load(const wchar_t* p_file_name, bool b_parse, bool b_read_only,bool b_parse_tag_only = false);

	void unload();
	//检查mp3 tag，定位audio_begin/audio_end，并查找第一帧
	bool parse(bool b_parse_tag_only = false);

	boost::shared_ptr<CMPAFrame> get_first_frame();
	boost::shared_ptr<mpeg_info> get_mpeg_info();
	//取得文件长度
	unsigned int get_size();
	//取得tag总数
	unsigned int get_tag_count();
	//取得指定的tag
	tag_ptr_type get_tag(unsigned int n_index);
	tag_ptr_type get_tag_by_serial(unsigned int n_serial);
	//文件内容检查(按帧进行遍历)
	check_result check_content(bool b_check_CRC, bool b_adjust);
	//写入（增加/更新/删除）id3 v2 tag
	bool write_id3v2_tag(const tag_base_data* p_data);
	bool write_id3v2_tag_wstring(const tag_base_data_w* p_data);
	//写入（增加/更新/删除）id3 v1 tag
	bool write_id3v1_tag(const tag_base_data* p_data);
	bool write_id3v1_tag_wstring(const tag_base_data_w* p_data);
	bool rip_id3v2_pic(const char* p_dir);
	bool rip_id3v2_pic(const wchar_t* s_raw_name);
	bool has_id3v2_pic();
	bool add_picture(const wchar_t *file_name);
	//删除尾部的tag(id3v1 & ape)
	int del_tail_tags();
	//取得播放时间
	unsigned int get_len_seconds(CMPAFrame* p_first_frame);
	unsigned int get_audio_begin() const;
	unsigned int get_audio_end() const;
	
	//文件中是否有加密数据
	bool  is_have_tag_encrypt_data();
	bool write_tag_encrypt_data(void *buf,int buflen);
	bool read_tag_encrypt_data(void *buf,int &buflen);

	//文件中是否有加密数据
	bool  is_have_encrypt_data();
	//向文件中写加密数据,如果数据中已经存在加密数据,则将原来的覆盖
	//文件必须以非只读方式打开,非则真接返回false,成功返回 true
	//如果buf==NULL 或缓存空间长度小于等于0,则只是删除加密数据
	bool write_encrypt_data(void *buf,int buflen);
	bool read_encrypt_data(void *buf,int &buflen);
	
private:

	//找到加密数据在文件中的位置
	int find_encrypt_data(const char *tag_name);
	//找到文件尾部的tag 的开始位置
	int find_endtag_start_pos() ;

	//更新已有的id3 v2 tag
	bool update_id3v2_tag(const tag_base_data* p_data);
	bool update_id3v2_tag_wstring(const tag_base_data_w* p_data);

	//增加或删除id3 v2 tag
	bool add_id3v2_tag(const tag_base_data* p_data);
	bool add_id3v2_tag_wstring(const tag_base_data_w* p_data);

	//定位第一帧
	bool seek_first_frame();
private:
	tag_container_type m_tag_list;
	boost::shared_ptr<io_base> msp_file;
	unsigned int mn_audio_begin;
	unsigned int mn_audio_end;
	unsigned int mn_last_frame_end;
	bool mb_read_only;
	boost::shared_ptr<CMPAFrame> msp_first_frame;
	unsigned int mn_check_frames;
	bool mb_encrypt_data;//是否有加密数据
public:
	typedef boost::shared_ptr<mp3_tag_factory> tag_factory_ptr;
	typedef std::vector<tag_factory_ptr> factory_container_type;
	typedef factory_container_type::iterator factory_iter;
public:
	//注册所有的默认tag factory
	void register_default_factory();
private:
	factory_container_type m_tag_factories;
};

class tag_manager
{
public:
	tag_manager();
	~tag_manager();
	static boost::shared_ptr<mp3_file> load_file(const char* p_file_name, 
		unsigned int n_check_frames = mp3_file::CHECK_MINI_FRAMES, bool b_read_only = true);
	static boost::shared_ptr<mp3_file> load_file(const wchar_t* p_file_name, 
		unsigned int n_check_frames = mp3_file::CHECK_MINI_FRAMES, bool b_read_only = true);

	static bool attach_id3v2_tag(const char* p_file_name, const tag_base_data* p_data);
	static bool attach_id3v2_tag_wstring(const wchar_t* p_file_name, const tag_base_data_w* p_data);

	static bool attach_id3v1_tag(const char* p_file_name, const tag_base_data* p_data);
	static bool attach_id3v1_tag_wstring(const wchar_t* p_file_name, const tag_base_data_w* p_data);
public:
	static void test_mp3_tag(const char* p_file_name);
	static void test_mp3_tag(const wchar_t* p_file_name);
	static unsigned int rip_picture(const char* p_file_name, const char* p_dir);
};

}		//end namespace em_mp3_tag
