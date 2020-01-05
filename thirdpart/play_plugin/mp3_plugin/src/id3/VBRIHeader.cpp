#include "vbriheader.h"
#include "mpaframe.h"

namespace em_mp3_tag
{
vbr_I_header* vbr_I_header::find_header(const CMPAFrame* p_frame)
{
	// VBRI header always at fixed offset
	unsigned int n_offset = p_frame->mn_offset + MPA_HEADER_SIZE + 32;
	
	// VBRI ID found?
	if (!check_ID(p_frame->msp_file_io, n_offset, 'V', 'B', 'R', 'I'))
		return NULL;
	
	return new vbr_I_header(p_frame, n_offset);
}

vbr_I_header::vbr_I_header(const CMPAFrame* p_frame, unsigned int n_offset) :
	CVBRHeader(p_frame->msp_file_io, n_offset)
{
	/* FhG VBRI Header

	size	description
	4		'VBRI' (ID)
	2		version
	2		delay
	2		quality
	4		# bytes
	4		# frames
	2		table size (for TOC)
	2		table scale (for TOC)
	2		size of table entry (max. size = 4 byte (must be stored in an integer))
	2		frames per table entry
	
	??		dynamic table consisting out of frames with size 1-4
			whole length in table size! (for TOC)

	*/

    // ID is already checked at this point
	n_offset += 4;

	// extract all fields from header (all mandatory)
	mn_version = CMPAFrame::BE_2_native(msp_file_io, 2, n_offset);
	n_offset += 2;
	mf_delay = (float)CMPAFrame::BE_2_native(msp_file_io, 2, n_offset);
	n_offset += 2;
	mn_quality = CMPAFrame::BE_2_native(msp_file_io, 2, n_offset);
	n_offset += 2;
	mn_bytes = CMPAFrame::BE_2_native(msp_file_io, 4, n_offset);
	n_offset += 4;
	mn_frames = CMPAFrame::BE_2_native(msp_file_io, 4, n_offset);
	n_offset += 4;
	mn_table_size = CMPAFrame::BE_2_native(msp_file_io, 2, n_offset) + 1;	//!!!
	n_offset += 2;
	mn_table_scale = CMPAFrame::BE_2_native(msp_file_io, 2, n_offset);
	n_offset += 2;
	mn_bytes_per_entry = CMPAFrame::BE_2_native(msp_file_io, 2, n_offset);
	n_offset += 2;
	mn_frames_per_entry = CMPAFrame::BE_2_native(msp_file_io, 2, n_offset);
	n_offset += 2;

	// extract TOC  (for more accurate seeking)
	mp_TOC = new int[mn_table_size];
	assert(mp_TOC != NULL);
	for (unsigned int i = 0 ; i < mn_table_size ; ++i)
	{
		mp_TOC[i] = CMPAFrame::BE_2_native(msp_file_io, mn_bytes_per_entry, n_offset);
		n_offset += mn_bytes_per_entry;
	}

	// get length of file (needed for seeking)
	mn_length_sec = p_frame->m_mpeg_header.GetLengthSecond(mn_frames);
}

vbr_I_header::~vbr_I_header()
{
	if (mp_TOC != NULL)
	{
		delete[] mp_TOC;
		mp_TOC = NULL;
	}
}

unsigned int vbr_I_header::seek_position(float& f_percent) const
{
	return seek_position_by_time((f_percent / 100.0f) * mn_length_sec * 1000.0f);
}

unsigned int vbr_I_header::seek_position_by_time(float f_entry_time_MS) const
{
	unsigned int i=0,  fraction = 0;
	unsigned int dwSeekPoint = 0;

	float fLengthMS;
	float fLengthMSPerTOCEntry;
	float fAccumulatedTimeMS = 0.0f ;
	 
	fLengthMS = (float)mn_length_sec * 1000.0f ;
	fLengthMSPerTOCEntry = fLengthMS / (float)mn_table_size;
	 
	if (f_entry_time_MS > fLengthMS) 
		f_entry_time_MS = fLengthMS; 
	 
	while (fAccumulatedTimeMS <= f_entry_time_MS)
	{
		dwSeekPoint += mp_TOC[i++];
		fAccumulatedTimeMS += fLengthMSPerTOCEntry;
	}
	  
	// Searched too far; correct result
	fraction = ( (int)((((fAccumulatedTimeMS - f_entry_time_MS) / fLengthMSPerTOCEntry) 
				+ (1.0f/(2.0f*(float)mn_frames_per_entry))) * (float)mn_frames_per_entry));

	dwSeekPoint -= (unsigned int)((float)mp_TOC[i-1] * (float)(fraction) 
					/ (float)mn_frames_per_entry);

	return dwSeekPoint;
}

}		//end namespace em_mp3_tag
