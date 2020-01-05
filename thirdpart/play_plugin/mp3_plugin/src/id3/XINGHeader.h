#pragma once

#include "vbrheader.h"

namespace em_mp3_tag
{
class lame_inner_tag;
class CMPAFrame;

class xing_header : public CVBRHeader
{
public:
	static xing_header* find_header(const CMPAFrame* p_frame);
	xing_header(const CMPAFrame* p_frame, unsigned int n_offset);
	virtual ~xing_header();
	virtual unsigned int seek_position(float& f_percent) const;
	lame_inner_tag* mp_lame_tag;
};

}		//end namespace em_mp3_tag
