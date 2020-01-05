#pragma once

#include "IO.h"
#include <boost/shared_ptr.hpp>
#include <string>

namespace em_mp3_tag
{

class lame_inner_tag
{
public:
	//n_begin为帧的开始位置
	static lame_inner_tag* find_tag(boost::shared_ptr<io_base> sp_file, unsigned int n_begin);
	~lame_inner_tag();

	std::string ms_encoder;
	unsigned int mn_lowpass_filter_Hz;
	unsigned char mn_bitrate;	// in kbps
	unsigned char mn_revision;

	bool is_VBR() const;
	bool is_ABR() const;
	bool is_CBR() const;
	std::string get_VBR_info() const;
	bool is_simple_tag() const { return mb_simple_tag; };

private:
	//n_offset为lame tag的开始位置
	lame_inner_tag(boost::shared_ptr<io_base> sp_file, unsigned int n_offset);
	
	unsigned char mn_VBR_info;
	bool mb_simple_tag;
	static std::string ms_VBR_info[10];
private:
	float mf_version;	// format x.yz
};

}		//end namespace em_mp3_tag
