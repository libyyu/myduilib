#include "mpaframe.h"	// also includes vbrheader.h
#include "xingheader.h"
#include "vbriheader.h"
#include "MPAHeader.h"
#include <cassert>

namespace em_mp3_tag
{
// first test with this static method, if it does exist
CVBRHeader* CVBRHeader::find_header(const CMPAFrame* p_frame)
{
	assert(p_frame != NULL);
	CVBRHeader* p_vbr_header = NULL;
	p_vbr_header = xing_header::find_header(p_frame);
	if (!p_vbr_header)
		p_vbr_header = vbr_I_header::find_header(p_frame);
	return p_vbr_header;
}

CVBRHeader::CVBRHeader(boost::shared_ptr<io_base> sp_file_io, unsigned int n_offset)
{
	msp_file_io = sp_file_io;
	mp_TOC = NULL;
	mn_offset = n_offset;
	mn_frames = 0;
	mn_bytes = 0;
	mn_quality = 0;
	mn_table_size = 0;
}

bool CVBRHeader::check_ID(boost::shared_ptr<io_base> sp_file_io, unsigned int n_offset, char ch0, char ch1, char ch2, char ch3)
{
	unsigned char s_ID_buf[ID_SIZE];
	int n_result = sp_file_io->seek(n_offset, FILE_BEGIN);
	assert(n_result == 0);
	unsigned int n_bytes_read = 0;
	n_result = sp_file_io->read(s_ID_buf, ID_SIZE, &n_bytes_read);
	assert(n_result == 0 && n_bytes_read == ID_SIZE);
	if (s_ID_buf[0] == ch0 && s_ID_buf[1] == ch1 && s_ID_buf[2] == ch2 && s_ID_buf[3] == ch3)
		return true;
	return false;
}

CVBRHeader::~CVBRHeader()
{
	if (mp_TOC)
		delete[] mp_TOC;
}

// get byte position for percentage value (fPercent) of file
bool CVBRHeader::seek_position(float& f_percent, unsigned int& n_seek_point) const
{
	if (mp_TOC == NULL || mn_bytes == 0)
		return false;

	// check range of f_percent
	if (f_percent < 0.0f)   
		f_percent = 0.0f;
	if (f_percent > 99.0f) 
		f_percent = 99.0f;

	n_seek_point = seek_position(f_percent);				//由子类提供实现
	return true;
}

}		//end namespace em_mp3_tag
