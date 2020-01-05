#include <stdlib.h>
#include "lametag.h"
#include <cassert>

using namespace std;

namespace em_mp3_tag
{

string lame_inner_tag::ms_VBR_info[10] = 
{
	("Unknown"),
	("CBR"),
	("ABR"),
	("VBR1"),
	("VBR2"),
	("VBR3"),
	("VBR4"),
	("Reserved"),
	("CBR2Pass"),
	("ABR2Pass")
};

lame_inner_tag* lame_inner_tag::find_tag(boost::shared_ptr<io_base> sp_file, unsigned int n_begin)
{
	// check for LAME Tag extension (always 120 bytes after XING ID)
	unsigned int n_offset = n_begin + 120;
	unsigned char s_buf[9];
	int n_result = sp_file->seek(n_offset, FILE_BEGIN);
	assert(n_result == 0);
	unsigned int n_bytes_read = 0;
	n_result = sp_file->read(s_buf, 9, &n_bytes_read);
	assert(n_result == 0 && n_bytes_read == 9);
	if (memcmp(s_buf, "LAME", 4) == 0)
		return new lame_inner_tag(sp_file, n_offset);
	return NULL;
}

lame_inner_tag::lame_inner_tag(boost::shared_ptr<io_base> sp_file, unsigned int n_offset)
{
	unsigned char s_buf[21];
	int n_result = sp_file->seek(n_offset, FILE_BEGIN);
	assert(n_result == 0);
	unsigned int n_bytes_read = 0;
	n_result = sp_file->read(s_buf, 21, &n_bytes_read);
	assert(n_result == 0 && n_bytes_read == 21);
	
	string s_version((char*)s_buf + 4, 4);
	mf_version = (float)atof(s_version.c_str());
	
	// LAME prior to 3.90 writes only a 20 byte encoder string
	if (mf_version < 3.90)
	{
		mb_simple_tag = true;
		ms_encoder = string((char*)s_buf, 20);
	}
	else
	{
		//9 byte is encoder string
		n_offset = 0;
		mb_simple_tag = false;
		ms_encoder = string((char*)s_buf, 9);
		n_offset += 9;
	
		// version information
		unsigned char n_info_and_VBR = s_buf[n_offset];
		++n_offset;							
		// revision info in 4 MSB
		mn_revision = n_info_and_VBR & 0xF0;
		// invalid value
		assert(mn_revision != 15);

		// VBR info in 4 LSB
		mn_VBR_info = n_info_and_VBR & 0x0F;
		
		// lowpass information
		mn_lowpass_filter_Hz = s_buf[n_offset] * 100;
		++n_offset;
		// skip replay gain values
		n_offset += 8;

		// skip encoding flags
		n_offset += 1;

		// average bitrate for ABR, bitrate for CBR and minimal bitrat for VBR [in kbps]
		// 255 means 255 kbps or more
		assert(n_offset == 20);
		mn_bitrate = s_buf[n_offset];
	}
}

lame_inner_tag::~lame_inner_tag()
{
}

bool lame_inner_tag::is_VBR() const
{
	if (mn_VBR_info >= 3 && mn_VBR_info <= 6)
		return true;
	return false;
}

bool lame_inner_tag::is_ABR() const
{
	if (mn_VBR_info == 2 || mn_VBR_info == 9)
		return true;
	return false;
}

bool lame_inner_tag::is_CBR() const
{
	if (mn_VBR_info == 1 || mn_VBR_info == 8)
		return true;
	return false;
}

string lame_inner_tag::get_VBR_info() const
{
	if (mn_VBR_info > 9)
		return ms_VBR_info[0];

	return ms_VBR_info[mn_VBR_info];
}

}		//end namespace em_mp3_tag
