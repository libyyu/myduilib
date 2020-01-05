#pragma once
#include "tag_base.h"
#include <string>
#include <boost/shared_array.hpp>

namespace em_mp3_tag
{

class lyrics3_tag : public mp3_tag_interface
{
public:
	static const unsigned char FOOTER_SIZE = 9;
	static const unsigned char V2_SIZE_BUF = 6;
	static const unsigned char V1_HEADER_SIZE = 11;
	static const unsigned int V1_BEFORE_FOOTER = 5100;
public:
	lyrics3_tag();
	~lyrics3_tag();
	bool init(io_base* p_file, unsigned int n_begin, unsigned int n_footer_begin, bool b_ver_2);
	virtual int get_length() const;
	virtual int get_offset() const;
	virtual bool is_append() const;
	//下面两个函数未实现
	virtual tag_base_data* get_base_tag();
	virtual tag_base_data_w* get_base_tag_wstring();//新加
	virtual std::string get_tag_name() ;
	virtual std::wstring get_tag_name_wstring() ;//新加
	virtual unsigned int get_tag_serial();
private:
	boost::shared_array<unsigned char> msp_buf;
	unsigned int mn_begin;
	unsigned int mn_length;
	float mf_version;					// format x.yz
};

class lyrics_tag_factory : public mp3_tag_factory
{
public:
	lyrics_tag_factory();
	~lyrics_tag_factory();
	virtual mp3_tag_interface* create_tag(io_base* p_file, unsigned int n_begin, unsigned int n_end);
};

}
