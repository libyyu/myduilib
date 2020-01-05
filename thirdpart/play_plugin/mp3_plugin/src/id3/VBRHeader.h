#pragma once

#include "IO.h"
#include <boost/shared_ptr.hpp>
#include "tag_base.h"

namespace em_mp3_tag
{
// class CMPAFrame must be included first and must be known here
class CMPAFrame;
class CMPAHeader;

class CVBRHeader
{
public:
	static CVBRHeader* find_header(const CMPAFrame* p_frame);
	virtual ~CVBRHeader();
	bool seek_position(float& f_percent, unsigned int& n_seek_point) const;

	unsigned int mn_bytes;		// total number of bytes
	unsigned int mn_frames;		// total number of frames

	static const unsigned char ID_SIZE = 4;

protected:	
	CVBRHeader(boost::shared_ptr<io_base> sp_file_io, unsigned int n_offset);

	static bool check_ID(boost::shared_ptr<io_base> sp_file_io, unsigned int n_offset, char ch0, char ch1, char ch2, char ch3);
	virtual unsigned int seek_position(float& f_percent) const = 0;
	boost::shared_ptr<io_base> msp_file_io;
public:	
	unsigned int mn_offset;
	unsigned int mn_quality;		// quality (0..100)
	int* mp_TOC;			// TOC points for seeking (must be freed)
	unsigned int mn_table_size;	// size of table (number of entries)	
};

}		//end namespace em_mp3_tag
