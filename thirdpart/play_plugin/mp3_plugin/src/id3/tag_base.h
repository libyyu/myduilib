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

//ͨ��ģʽö��
enum channel_mode_e
{
	cm_stereo,							//������
	cm_joint_stereo,					//����������
	cm_dual_channel,				//˫����
	cm_single_channel				//������
};

enum check_error_e
{
	ce_CRC_error,					//CRCУ���
	ce_sample_change,				//�����ʸı�
	ce_layer_change,				//layer�ı�
	ce_version_change,				//mpeg�汾�ı�
	ce_frame_offset,					//֡ƫ���쳣
	ce_last_truncated,				//β֡���ض�
	ce_free_space,					//β֡����ʣ��ռ�
	ce_seek_failed,					//�޷���λ��֡
	ce_success							//�ļ�����
};

#define TAG_ID3_V1_LEN 128
#define TAG_ID3_V2_MIN 10
#define INVALID_INDEX 0xFFFFFFFF
#define TAG_ID3_V2_LEN 512

struct tag_base_data
{
	static const unsigned short MAX_SONG_COUNT = 128;
	static const unsigned short MAX_DISK_COUNT = 64;
	std::string s_artist;						//�ݳ�����(������ֵĻ����зֺŷָ�)
	std::string s_album;						//ר������
	std::string s_title;						//��������
	std::string s_date;						//��������(��ʽ"2005" or "200502" or "2005.02" or "2005.2")
	unsigned short n_track;				//�ø���������CD������(��1��ʼ)
	unsigned short n_song_count;		//�ø�������CD�ĸ�������
	std::string s_genre;						//����
	int n_genre_index;						//�������
	std::string s_album_artist;			//ר������
	unsigned int n_disk_serial;			//�ø���������CD���(���ֵΪ0�����ר��ֻ��һ��CD����ͬ��1)
	unsigned int n_disk_count;			//�ø�������ר����CD���������ֵΪ0�����ר��ֻ��һ��CD����ͬ��1��
	unsigned int n_gmt_publish;		//gmt��ʽ�ķ���ʱ��
	std::string s_comment;				//ע��(���ڶ�CDר����ע�͵ĸ�ʽΪ��ר����-CD��������CDû�ж��������֣�����"Disc n"����)
	void clear();
	void generate_gmt_publish();
};
struct tag_base_data_w
{
	static const unsigned short MAX_SONG_COUNT = 128;
	static const unsigned short MAX_DISK_COUNT = 64;
	std::wstring s_artist;						//�ݳ�����(������ֵĻ����зֺŷָ�)
	std::wstring s_album;						//ר������
	std::wstring s_title;						//��������
	std::wstring s_date;						//��������(��ʽ"2005" or "200502" or "2005.02" or "2005.2")
	unsigned short n_track;				//�ø���������CD������(��1��ʼ)
	unsigned short n_song_count;		//�ø�������CD�ĸ�������
	std::wstring s_genre;						//����
	int n_genre_index;						//�������
	std::wstring s_album_artist;			//ר������
	unsigned int n_disk_serial;			//�ø���������CD���(���ֵΪ0�����ר��ֻ��һ��CD����ͬ��1)
	unsigned int n_disk_count;			//�ø�������ר����CD���������ֵΪ0�����ר��ֻ��һ��CD����ͬ��1��
	unsigned int n_gmt_publish;		//gmt��ʽ�ķ���ʱ��
	std::wstring s_comment;				//ע��(���ڶ�CDר����ע�͵ĸ�ʽΪ��ר����-CD��������CDû�ж��������֣�����"Disc n"����)
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

//mp3 tagͨ�ýӿ�
class mp3_tag_interface
{
public:
	virtual ~mp3_tag_interface() {}
	virtual int get_length() const = 0;
	virtual int get_offset() const = 0;
	virtual bool is_append() const = 0;
	virtual tag_base_data* get_base_tag() = 0;
	virtual tag_base_data_w* get_base_tag_wstring() = 0;//�¼�

	virtual std::string get_tag_name() = 0;
	virtual std::wstring get_tag_name_wstring() = 0;//�¼�

	virtual unsigned int get_tag_serial() = 0;
public:
	static const int MAX_FIELD_VALUE_SIZE = 1024 * 10;
	static const int MAX_TAG_ITEM_SIZE = 1024 * 1024;			//�����κ�Ԫ�أ�1M���
};

//mp3 tag�����ӿ�
class mp3_tag_factory
{
public:
	virtual ~mp3_tag_factory() {}
	//n_begin��n_end��������ļ�ͷ��ƫ��
	virtual mp3_tag_interface* create_tag(io_base* p_file, unsigned int n_begin, unsigned int n_end) = 0;
};

}		//end namespace em_mp3_tag
