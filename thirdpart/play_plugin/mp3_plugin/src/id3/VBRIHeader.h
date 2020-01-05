#pragma once
#include "vbrheader.h"

namespace em_mp3_tag
{

class CMPAFrame;

class vbr_I_header : public CVBRHeader
{
public:
	static vbr_I_header* find_header(const CMPAFrame* p_frame);

	vbr_I_header(const CMPAFrame* p_frame, unsigned int n_offset);
	virtual ~vbr_I_header();

	virtual unsigned int seek_position(float& f_percent) const;
	unsigned int seek_position_by_time(float f_entry_time_MS) const;

	// these values exist only in VBRI headers
	float mf_delay;	
	unsigned int mn_table_scale;	// for seeking
	unsigned int mn_bytes_per_entry;
	unsigned int mn_frames_per_entry;
	unsigned int mn_version;
private:
	unsigned int mn_length_sec;
};

}		//end namespace em_mp3_tag
