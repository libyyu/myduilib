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
	eb_CBR,								//�̶�������
	eb_ABR,								//���ڹ̶�������
	eb_VBR								//�ɱ������
};

struct check_result
{
	check_error_e e_result; // �����
	unsigned int n_err_frame; // ����֡
	unsigned int n_frames; // ��֡��
	unsigned int n_unpadding_frames; // δ���֡��
	unsigned int n_padding_frames; // ���֡��
	encode_bitrate_e e_bitrate_type; // ��������
	unsigned short n_bitrate; // ������
	unsigned int max_bitrate; // ��������
	unsigned int min_bitrate; // ��С������
	unsigned int average_bitrate; // ƽ��������
	unsigned int n_seconds_length; // ��ʱ��
	int n_extend; // ��չֵ(�ɸ���������Զ����ֵ����)
	int n_adjust_result; // 1Ϊ������adjust��-1Ϊβ����tag���޷�ֱ��adjust��0Ϊû��adjust
};

struct mpeg_info
{
	mpeg_version_e mpa_version;			//mpeg�汾
	mpeg_layer_e mpa_layer;				//layer
	channel_mode_e channel_mode;		//������
	unsigned int bit_rate;						//������
	unsigned int sample_rate;				//������
	encode_bitrate_e e_bitrate_type;		//��������
	unsigned int bit_rate_in_vbr;			//VBR header�еı�����
	unsigned int seconds_length;		//����ʱ��
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
	//���mp3 tag����λaudio_begin/audio_end�������ҵ�һ֡
	bool parse(bool b_parse_tag_only = false);

	boost::shared_ptr<CMPAFrame> get_first_frame();
	boost::shared_ptr<mpeg_info> get_mpeg_info();
	//ȡ���ļ�����
	unsigned int get_size();
	//ȡ��tag����
	unsigned int get_tag_count();
	//ȡ��ָ����tag
	tag_ptr_type get_tag(unsigned int n_index);
	tag_ptr_type get_tag_by_serial(unsigned int n_serial);
	//�ļ����ݼ��(��֡���б���)
	check_result check_content(bool b_check_CRC, bool b_adjust);
	//д�루����/����/ɾ����id3 v2 tag
	bool write_id3v2_tag(const tag_base_data* p_data);
	bool write_id3v2_tag_wstring(const tag_base_data_w* p_data);
	//д�루����/����/ɾ����id3 v1 tag
	bool write_id3v1_tag(const tag_base_data* p_data);
	bool write_id3v1_tag_wstring(const tag_base_data_w* p_data);
	bool rip_id3v2_pic(const char* p_dir);
	bool rip_id3v2_pic(const wchar_t* s_raw_name);
	bool has_id3v2_pic();
	bool add_picture(const wchar_t *file_name);
	//ɾ��β����tag(id3v1 & ape)
	int del_tail_tags();
	//ȡ�ò���ʱ��
	unsigned int get_len_seconds(CMPAFrame* p_first_frame);
	unsigned int get_audio_begin() const;
	unsigned int get_audio_end() const;
	
	//�ļ����Ƿ��м�������
	bool  is_have_tag_encrypt_data();
	bool write_tag_encrypt_data(void *buf,int buflen);
	bool read_tag_encrypt_data(void *buf,int &buflen);

	//�ļ����Ƿ��м�������
	bool  is_have_encrypt_data();
	//���ļ���д��������,����������Ѿ����ڼ�������,��ԭ���ĸ���
	//�ļ������Է�ֻ����ʽ��,������ӷ���false,�ɹ����� true
	//���buf==NULL �򻺴�ռ䳤��С�ڵ���0,��ֻ��ɾ����������
	bool write_encrypt_data(void *buf,int buflen);
	bool read_encrypt_data(void *buf,int &buflen);
	
private:

	//�ҵ������������ļ��е�λ��
	int find_encrypt_data(const char *tag_name);
	//�ҵ��ļ�β����tag �Ŀ�ʼλ��
	int find_endtag_start_pos() ;

	//�������е�id3 v2 tag
	bool update_id3v2_tag(const tag_base_data* p_data);
	bool update_id3v2_tag_wstring(const tag_base_data_w* p_data);

	//���ӻ�ɾ��id3 v2 tag
	bool add_id3v2_tag(const tag_base_data* p_data);
	bool add_id3v2_tag_wstring(const tag_base_data_w* p_data);

	//��λ��һ֡
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
	bool mb_encrypt_data;//�Ƿ��м�������
public:
	typedef boost::shared_ptr<mp3_tag_factory> tag_factory_ptr;
	typedef std::vector<tag_factory_ptr> factory_container_type;
	typedef factory_container_type::iterator factory_iter;
public:
	//ע�����е�Ĭ��tag factory
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
