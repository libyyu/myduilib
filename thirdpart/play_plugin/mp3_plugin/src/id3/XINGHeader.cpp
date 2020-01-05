#include "xingheader.h"
#include "MPAframe.h"
#include "lametag.h"

namespace em_mp3_tag
{
// for XING VBR Header flags
#define FRAMES_FLAG		0x0001
#define BYTES_FLAG			0x0002
#define TOC_FLAG				0x0004
#define VBR_SCALE_FLAG	0x0008

// offset of XING header: after side information in Layer III
xing_header* xing_header::find_header(const CMPAFrame* p_frame)
{
	// where does VBR header begin (XING)
	unsigned int n_offset = p_frame->mn_offset + MPA_HEADER_SIZE + p_frame->m_mpeg_header.GetSideInfoSize();		// + (pFrame->m_pHeader->m_bCRC?2:0);

	// check for XING header first
	if (!check_ID(p_frame->msp_file_io, n_offset, 'X', 'i', 'n', 'g') && !check_ID(p_frame->msp_file_io, n_offset, 'I', 'n', 'f', 'o'))
		return NULL;
	
	return new xing_header(p_frame, n_offset);
}

xing_header::xing_header(const CMPAFrame* p_frame, unsigned int n_offset) : CVBRHeader(p_frame->msp_file_io, n_offset)
{
	/* XING VBR-Header

	 size	description
	 4		'Xing' or 'Info'
	 4		flags (indicates which fields are used)
	 4		frames (optional)
	 4		bytes (optional)
	 100	toc (optional)
	 4		a VBR quality indicator: 0=best 100=worst (optional)

	*/
	
	// XING ID already checked at this point
	n_offset += 4;
	unsigned int n_flags;

	// get flags (mandatory in XING header)
	n_flags = CMPAFrame::BE_2_native(msp_file_io, 4, n_offset); 
	n_offset += 4;

	// extract total number of frames in file
	if (n_flags & FRAMES_FLAG)
	{
		mn_frames = CMPAFrame::BE_2_native(msp_file_io, 4, n_offset);
		n_offset += 4;
	}

	// extract total number of bytes in file
	if (n_flags & BYTES_FLAG) 
	{
		mn_bytes = CMPAFrame::BE_2_native(msp_file_io, 4, n_offset);
		n_offset += 4;
	}

	int n_result = 0;
	// extract TOC (for more accurate seeking)
	if (n_flags & TOC_FLAG) 
	{
		mn_table_size = 100;
		mp_TOC = new int[mn_table_size];
		assert(mp_TOC != NULL);
		
		for (unsigned int i = 0; i < mn_table_size; ++i)
		{
			n_result = msp_file_io->seek(n_offset, FILE_BEGIN);
			assert(n_result == 0);
			unsigned int n_bytes_read = 0;
			n_result = msp_file_io->read(&mp_TOC[i], 1, &n_bytes_read);
			assert(n_result == 0 && n_bytes_read == 1);
			n_offset += 1;
		}
	}

	if (n_flags & VBR_SCALE_FLAG)
	{
		mn_quality = CMPAFrame::BE_2_native(msp_file_io, 4, n_offset);
		n_offset += 4;
	}
	mp_lame_tag = lame_inner_tag::find_tag(msp_file_io, mn_offset);
}

xing_header::~xing_header()
{
	if (mp_lame_tag != NULL)
		delete mp_lame_tag;
}

unsigned int xing_header::seek_position(float& f_percent) const
{
	// interpolate in TOC to get file seek point in bytes
	int a;
	float fa, fb, fx;

	a = (int)f_percent;
	fa = (float)mp_TOC[a];
	
	if (a < 99) 
	{
		fb = (float)mp_TOC[a+1];
	}
	else 
	{
		fb = 256.0f;
	}

	fx = fa + (fb - fa) * (f_percent - a);

	return (int)((1.0f / 256.0f) * fx * mn_bytes); 
}

}		//end namespace em_mp3_tag
