#pragma once
#include "tag_base.h"
#include "IO.h"
#include <string>
#include <utility>
#include <boost/shared_array.hpp>
#include "id3tag.h"
#include <vector>
#include <time.h>

namespace em_mp3_tag
{
/* ID3v2 field frames */
# define ID3V2_FRAME_TITLE_DESC "TIT2"				//Title/songname/content description
# define ID3V2_FRAME_AMS_TITLE	"TALB"				//Album/Movie/Show title
# define ID3V2_FRAME_SET_PART "TPOS"					//Part of a set
# define ID3V2_FRAME_SET_TRACK "TRCK"				//Track number/Position in set
# define ID3V2_FRAME_SOLOIST "TPE1"					//Lead performer(s)/Soloist(s), 演唱歌手
# define ID3V2_FRAME_RECORDING_TIME "TDRC"	//Recording time
# define ID3V2_FRAME_CONTENT_TYPE "TCON"		//Content type

#define ID3V2_FRAME_PUBLISH_YEAR "TYER"			//发布年份

# define ID3V2_FRAME_ORCHESTRA "TPE2"				//Band/orchestra/accompaniment, 专辑歌手
# define ID3V2_FRAME_COMMENT "COMM"			//Comments, 注释
# define ID3V2_FRAME_PICTURE "APIC"					//Attached picture, 图片
# define ID3V2_FRAME_LYRICIST "TEXT"					//Lyricist/Text writer, 作词
# define ID3V2_FRAME_COMPOSER "TCOM"			//Composer, 作曲

# define ID3V2_FRAME_ENCRY	"AENC"					//Audio encryption
# define ID3V2_FRAME_SEEK_INDEX "ASPI"				//Audio seek point index
# define ID3V2_FRAME_COMF "COMR"					//Commercial frame
# define ID3V2_FRAME_ENCRY_REGISTER "ENCR"		//Encryption method registration
# define ID3V2_FRAME_EQUAL_2 "EQU2"					//Equalisation (2)
# define ID3V2_FRAME_EVENT_CODE "ETCO"			//Event timing codes
# define ID3V2_FRAME_GEN_ENC_OBJ "GEOB"			//General encapsulated object
# define ID3V2_FRAME_GROUP_ID "GRID"				//Group identification registration
# define ID3V2_FRAME_LINK "LINK"						//Linked information
# define ID3V2_FRAME_CDID "MCDI"						//Music CD identifier
# define ID3V2_FRAME_MPEG_TABLE "MLLT"			//MPEG location lookup table
# define ID3V2_FRAME_OWNERSHIP "OWNE"			//Ownership frame
# define ID3V2_FRAME_PRIVATE "PRIV"					//Private frame
# define ID3V2_FRAME_PLAY_COUNT "PCNT"			//Play counter
# define ID3V2_FRAME_POPULAR "POPM"				//Popularimeter
# define ID3V2_FRAME_POSITION_SYNC "POSS"		//Position synchronisation frame
# define ID3V2_FRAME_RECOMM_BUF_SIZE "RBUF"	//Recommended buffer size
# define ID3V2_FRAME_RELA_VOL "RVA2"				//Relative volume adjustment (2)
# define ID3V2_FRAME_REVERB "RVRB"					//Reverb
# define ID3V2_FRAME_SEEK "SEEK"						//Seek frame
# define ID3V2_FRAME_SIGNATURE "SIGN"				//Signature frame
# define ID3V2_FRAME_SYNC_LYRIC "SYLT"				//Synchronised lyric/text
# define ID3V2_FRAME_SYNC_TEMPO "SYTC"			//Synchronised tempo codes
# define ID3V2_FRAME_BPM "TBPM"						//BPM (beats per minute)
# define ID3V2_FRAME_COPYRIGHT "TCOP"				//Copyright message
# define ID3V2_FRAME_ENCODING_TIME "TDEN"		//Encoding time
# define ID3V2_FRAME_PLAYLIST_DELAY "TDLY"		//Playlist delay
# define ID3V2_FRAME_ORIGINAL_RELEASE "TDOR"	//Original release time
# define ID3V2_FRAME_RELEASE_TIME "TDRL"			//Release time
# define ID3V2_FRAME_TAGGING_TIME "TDTG"		//Tagging time
# define ID3V2_FRAME_ENCODED_BY "TENC"			//Encoded by
# define ID3V2_FRAME_FILE_TYPE "TFLT"					//File type
# define ID3V2_FRAME_INV_PEOPLE_LIST "TIPL"		//Involved people list
# define ID3V2_FRAME_CONT_GROUP_DESC "TIT1"	//Content group description
# define ID3V2_FRAME_DESC_REFINE "TIT3"				//Subtitle/Description refinement
# define ID3V2_FRAME_INIT_KEY "TKEY"					//Initial key
# define ID3V2_FRAME_LANGUAGE "TLAN"				//Language(s)
# define ID3V2_FRAME_LENGTH "TLEN"					//Length
# define ID3V2_FRAME_CREDIT_LIST "TMCL"			//Musician credits list
# define ID3V2_FRAME_MEDIA_TYPE "TMED"			//Media type
# define ID3V2_FRAME_MOOD "TMOO"					//Mood
# define ID3V2_FRAME_ORIGINAL_TITLE "TOAL"		//Original album/movie/show title
# define ID3V2_FRAME_ORIGINAL_FILE_NAME "TOFN"	//Original filename
# define ID3V2_FRAME_ORIGINAL_LYRICIST "TOLY"	//Original lyricist(s)/text writer(s)
# define ID3V2_FRAME_ORIGINAL_ARTIST "TOPE"	//Original artist(s)/performer(s), 原创
# define ID3V2_FRAME_FILE_OWNER "TOWN"			//File owner/licensee

# define ID3V2_FRAME_PERFORMER_REFINE "TPE3"	//Conductor/performer refinement
# define ID3V2_FRAME_REMIX "TPE4"						//Interpreted, remixed, or otherwise modified by
# define ID3V2_FRAME_NOTICE "TPRO"					//Produced notice
# define ID3V2_FRAME_PUBLISHER "TPUB"				//Publisher, 发布者

# define ID3V2_FRAME_RADIO_NAME "TRSN"			//Internet radio station name
# define ID3V2_FRAME_RADIO_OWNER "TRSO"		//Internet radio station owner
# define ID3V2_FRAME_ALBUM_SORT_ORDER "TSOA"			//Album sort order
# define ID3V2_FRAME_PERFORMER_SORT_ORDER "TSOP"	//Performer sort order
# define ID3V2_FRAME_TITLE_SORT_ORDER "TSOT"				//Title sort order
# define ID3V2_FRAME_RECORDING_CODE "TSRC"				//ISRC (international standard recording code)
# define ID3V2_FRME_SETTING_FOR_ENCODING "TSSE"		//Software/Hardware and settings used for encoding
# define ID3V2_FRAME_SET_SUBTITLE "TSST"						//Set subtitle
# define ID3V2_FRAME_USER_TEXT "TXXX"							//User defined text information frame

# define ID3V2_FRAME_FILE_ID "UFID"									//Unique file identifier
# define ID3V2_FRAME_USE_TERM "USER"							//Terms of use
# define ID3V2_FRAME_UNSYNC_LYRIC "USLT"						//Unsynchronised lyric/text transcription

# define ID3V2_FRAME_COMM_INFO	"WCOM"					//Commercial information
# define ID3V2_FRAME_COPYRIGHT_INFO "WCOP"				//Copyright/Legal information
# define ID3V2_FRAME_OFFICIAL_FILE_PAGE "WOAF"					//Official audio file webpage
# define ID3V2_FRAME_OFFICIAL_ARTIST_PAGE "WOAR"				//Official artist/performer webpage
# define ID3V2_FRAME_OFFICIAL_SONG_PAGE "WOAS"					//Official audio source webpage
# define ID3V2_FRAME_OFFICIAL_IRS_PAGE "WORS"						//Official Internet radio station homepage
# define ID3V2_FRAME_PAYMENT "WPAY"									//Payment
# define ID3V2_FRAME_OFFICIAL_PUBLISH_PAGE "WPUB"				//Publishers official webpage
# define ID3V2_FRAME_USER_URL "WXXX"									//User defined URL link frame

struct id3_tag_base
{
protected:
	static bool parse_base_data(id3_tag* p_input, bool b_id3_v1, tag_base_data* p_output);
	static bool parse_base_data_wstring(id3_tag* p_input, bool b_id3_v1, tag_base_data_w* p_output);//新加

	//设置字符串字段值
	static bool set_field_string(struct id3_frame* p_frame, const std::string& s_value, bool b_id3_v1);
	static bool set_field_wstring(struct id3_frame* p_frame, const std::wstring& s_value, bool b_id3_v1);//wstring版
	//增加字符串帧到tag
	static bool add_frame_string(id3_tag* p_tag, const char* p_frame_name, const std::string& s_value, bool b_id3_v1);
	static bool add_comment_frame(id3_tag* p_tag, const char* p_frame_name, const std::string& s_value, bool b_id3_v1);
	//wstring 版
	static bool add_frame_wstring(id3_tag* p_tag, const char* p_frame_name, const std::wstring& s_value, bool b_id3_v1);
	static bool add_comment_frame_wstring(id3_tag* p_tag, const char* p_frame_name, const std::wstring& s_value, bool b_id3_v1);

private:
	//取得字符串字段值
	static std::string get_field_string(struct id3_frame* p_frame);
	static std::wstring get_field_wstring(struct id3_frame* p_frame);

	//取得整数字段值
	static int get_field_int(struct id3_frame* p_frame);
	//取得genre字段值
	static std::pair<int, std::string> get_genre_field(struct id3_frame* p_frame, bool b_id3_v1);
	static std::pair<int, std::wstring> get_genre_field_wstring(struct id3_frame* p_frame, bool b_id3_v1);
private:
	static void print_frame(struct id3_frame* p_frame);
};

class id3_v1_tag : public mp3_tag_interface, private id3_tag_base
{
public:
	id3_v1_tag();
	~id3_v1_tag();
	bool init(unsigned char* p_data, unsigned int n_begin, unsigned int n_length);
	virtual int get_length() const;
	virtual int get_offset() const;
	virtual bool is_append() const;
	virtual tag_base_data* get_base_tag();
	virtual tag_base_data_w* get_base_tag_wstring();//新加
	virtual std::string get_tag_name() ;
	virtual std::wstring get_tag_name_wstring() ;//新加
	virtual unsigned int get_tag_serial();
	virtual int render(unsigned char* p_buf);
	bool import(const tag_base_data* p_data);
	bool import_wstring(const tag_base_data_w* p_data);

private:
	unsigned char mp_buf[TAG_ID3_V1_LEN];
	unsigned int mn_begin;		//起始偏移
	struct id3_tag* mp_tag;
};

class id3_v2_tag : public mp3_tag_interface, private id3_tag_base
{
public:
	id3_v2_tag();
	~id3_v2_tag();
	bool init(boost::shared_array<unsigned char> sp_buf, unsigned int n_begin, unsigned int n_length);
	virtual int get_length() const;
	virtual int get_offset() const;
	virtual bool is_append() const;
	virtual tag_base_data* get_base_tag();
	virtual tag_base_data_w* get_base_tag_wstring() ;//新加
	virtual std::string get_tag_name() ;
	virtual std::wstring get_tag_name_wstring();//新加
	virtual unsigned int get_tag_serial();
	virtual int render(unsigned char* p_buf);
	bool import(const tag_base_data* p_data);
	 bool import_wstring(const tag_base_data_w* p_data);

	void set_padding_size(unsigned int n_bytes);
	bool attach_inner_frame(mp3_tag_interface* p_source_tag,bool b_encrypt_frame=true,bool b_pic_frame=true);
	unsigned int rip_picture(const char* p_raw_name);
	unsigned int rip_picture(const wchar_t* p_raw_name);
	bool add_picture(const wchar_t * filename);
	bool add_encrypt_frame(int n_szie,char *p_buffer);
	bool has_picture();
	bool get_encrypt_frame_data(int &n_size,char *p_buffer);
private:
	//0-unknown, 1-jpg, 2-png
	static unsigned int get_pic_type(const unsigned char* p_buf, unsigned int n_len);
private:
	boost::shared_array<unsigned char> msp_buf;
	unsigned int mn_begin;
	unsigned int mn_length;
	struct id3_tag* mp_tag;
};

class id3_v1_factory : public mp3_tag_factory
{
public:
	id3_v1_factory();
	~id3_v1_factory();
	virtual mp3_tag_interface* create_tag(io_base* p_file, unsigned int n_begin, unsigned int n_end);
};

class id3_v2_factory : public mp3_tag_factory
{
public:
	id3_v2_factory();
	~id3_v2_factory();
	virtual mp3_tag_interface* create_tag(io_base* p_file, unsigned int n_begin, unsigned int n_end);
};

}		//end namespace em_mp3_tag

