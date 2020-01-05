#include <math.h>	// for ceil
#include "mpaframe.h"
#include <boost/scoped_ptr.hpp>
#include "tag_base.h"

namespace em_mp3_tag
{

// number of bits that are used for CRC check in MPEG 1 Layer 2
// (this table is invalid for Joint Stereo/Intensity Stereo)
// 1. index = allocation table index, 2. index = mono
const unsigned int CMPAFrame::mn_protected_bits_layer2[5][2] =
{
	{284,142},	// table a
	{308,154},	// table b
	{42,84},		// table c
	{62,114},	// table d
	{270,135}	// table for MPEG 2/2.5
};

CMPAFrame* CMPAFrame::seek_first_frame(boost::shared_ptr<io_base> sp_file_io,
	unsigned int n_begin, unsigned int n_end, unsigned int n_min_frames)
{
	static const unsigned int INVALID_OFFSET = 0xFFFFFFFF;
	assert(n_end > n_begin);
	assert(n_end <= sp_file_io->get_size());
	assert(n_min_frames >= 1);
	unsigned int n_seek_begin = n_begin;
	CMPAFrame* p_frame = new CMPAFrame();
	bool b_result = false;
	unsigned int n_first_begin = INVALID_OFFSET;
	while (n_seek_begin < n_end)
	{
		n_first_begin = INVALID_OFFSET;
		b_result = p_frame->init(sp_file_io, n_seek_begin, n_end, NULL);	
		if (b_result)		//定位成功，找到第一帧(同时说明第二帧是匹配的)
		{
			n_first_begin = p_frame->mn_offset;
			assert(p_frame->mn_offset >= n_seek_begin);
			assert(p_frame->mn_offset <= n_end);
			for (unsigned int i = 0; i < n_min_frames - 1; ++i)
			{
				b_result = p_frame->next();
				if (!b_result)
					break;
			}
		}
		if (b_result)			//连续走指定的帧数都正常
			break;		
		if (n_seek_begin == n_end)
			break;
		if (n_first_begin == INVALID_OFFSET || n_first_begin == 0 || n_first_begin == n_seek_begin)
			++n_seek_begin;
		else
			n_seek_begin = n_first_begin;		//继续查找第一帧
		assert(n_seek_begin <= n_end);
		if (n_seek_begin == n_end)
			break;
 		if (!b_result)				//直接无法定位到第一帧, 直接退出, 失败(不接受这样的音频文件)
 		{
 			break;
 		}
	}

	delete p_frame;
	p_frame = NULL;
	if (b_result)
	{
		assert(n_first_begin != INVALID_OFFSET);
		p_frame = new CMPAFrame();
		b_result = p_frame->init(sp_file_io, n_first_begin, n_end, NULL);
		assert(b_result);
		assert(p_frame->mn_offset == n_first_begin);
	}
	else
	{
		//assert(n_first_begin == INVALID_OFFSET);
		//assert(n_seek_begin == n_end);
	}
	return p_frame;
}

bool CMPAFrame::next()
{
	bool b_result(false);
	assert(msp_file_io);
	if (mb_is_last)
		return false;
	unsigned int n_next_offset = next_frame_offset();
	if (n_next_offset > mn_end)			//越界
	{
		assert(false);
	}
	else if (n_next_offset == mn_end)			//当前帧已经是最后一帧
	{
		assert(false);				//在之前mb_is_last已经返回
	}
	else
	{
		assert(mp_first_header != NULL);
		CMPAHeader backup = m_mpeg_header;
		b_result = init(msp_file_io, n_next_offset, mn_end, mp_first_header);
		if (!b_result)
			m_mpeg_header = backup;
	}
	return b_result;
}

bool CMPAFrame::init(boost::shared_ptr<io_base> sp_file_io, unsigned int n_begin, unsigned int n_end, const CMPAHeader* p_compare_header)
{
	assert(n_end > n_begin);
	assert(n_end <= sp_file_io->get_size());

	//初始化mpeg header
	int n_result = m_mpeg_header.Init(sp_file_io.get(), n_begin, p_compare_header);
	if (n_result < 0)
		return false;
	assert(n_end > n_result);
	n_begin = n_result;
	msp_file_io = sp_file_io;
	mn_offset = n_begin;
	mn_length = m_mpeg_header.CalcFrameSize();
	mn_end = n_end;
	mb_is_last = false;
	bool b_result = false;

	unsigned int n_next_offset = next_frame_offset();

	if (n_next_offset > n_end)					//越界
	{
		//assert(false);
		return false;
	}
	else if (n_next_offset == n_end)			//明确的最后一帧
	{
		mb_is_last = true;
		b_result = true;
	}
	else													//在允许的范围内
	{
		b_result = true;
		if (p_compare_header == NULL)		//需要做下一帧header匹配
		{
			CMPAFrame* p_next_frame = new CMPAFrame();
			b_result = p_next_frame->init(sp_file_io, n_next_offset, n_end, &m_mpeg_header);
			delete p_next_frame;
			p_next_frame = NULL;
			if (mp_first_header == NULL)
				mp_first_header = new CMPAHeader();
			*mp_first_header = m_mpeg_header;
		}
	}
	return b_result;
}

CMPAFrame::CMPAFrame()
{
	mn_offset = 0;
	mn_length = 0;
	mb_is_last = false;
	mn_end = 0;
	mp_first_header = NULL;
}

CMPAFrame::~CMPAFrame()
{
	if (mp_first_header != NULL)
		delete mp_first_header;
}

CVBRHeader* CMPAFrame::FindVBRHeader() const
{
	// read out VBR header (if available), must be freed with delete
	return CVBRHeader::find_header(this);
}

// returns true if CRC is correct otherwise false
// do only call this function, if header contains a CRC checksum
bool CMPAFrame::CheckCRC() const
{
	if (!m_mpeg_header.m_bCRC)
		return false;

	unsigned int n_protected_bits;
	
	// which bits should be considered for CRC calculation
	switch(m_mpeg_header.m_Layer)
	{
		case ml_layer_1:
			n_protected_bits = 4 * (m_mpeg_header.GetNumChannels() * m_mpeg_header.m_wBound + (32 - m_mpeg_header.m_wBound));
			break;
		case ml_layer_2:
			// no check for Layer II files (not easy to compute number protected bits, need to get allocation bits first)
			return true;
			break;
		// for Layer III the protected bits are the side information
		case ml_layer_3:
			n_protected_bits = m_mpeg_header.GetSideInfoSize() * 8;
		default:
			return true;
	}

	// CRC is also calculated from the last 2 bytes of the header
	n_protected_bits += MPA_HEADER_SIZE * 8 +16;				// +16bit for CRC itself

	unsigned int n_byte_size = (unsigned int)ceil(n_protected_bits / 8.0); 
	if (n_byte_size > 1024 * 1024)
		return false;
	// the first 2 bytes and the CRC itself are automatically skipped
	unsigned int n_offset = mn_offset;

	boost::scoped_ptr<unsigned char> sp_buf(new unsigned char[n_byte_size]);
	unsigned int n_bytes_2_read = 0;
	int n_result = 0;
	n_result = msp_file_io->seek(n_offset, FILE_BEGIN);
	assert(n_result == 0);
	n_result = msp_file_io->read(sp_buf.get(), n_byte_size, &n_bytes_2_read);
	assert(n_result == 0 && n_bytes_2_read == n_byte_size);
	unsigned short n_CRC16 = CalcCRC16(sp_buf.get(), n_protected_bits);

	// read out crc from frame (it follows frame header)
	n_offset += MPA_HEADER_SIZE;
	unsigned char s_CRC[2];
	n_result = msp_file_io->seek(n_offset, FILE_BEGIN);
	assert(n_result == 0);
	n_bytes_2_read = 0;
	n_result = msp_file_io->read(sp_buf.get(), 2, &n_bytes_2_read);
	assert(n_result == 0 && n_bytes_2_read == 2);
	if (n_CRC16 == BE_2_native(s_CRC, 2))
		return true;
	return false;
}

unsigned int CMPAFrame::BE_2_native(unsigned char* p_buf, unsigned int n_size)
{
	assert(n_size > 0 && n_size <= 4);
	unsigned int n_result = 0;

	// big endian extract (most significant byte first) (will work on little and big-endian computers)
	unsigned int n_bytes_shift = n_size - 1;
	for (unsigned int i = 0; i < n_size; ++i)
	{
		n_result |= p_buf[i] << 8 * n_bytes_shift--;			// the bit shift will do the correct byte order for you                                                           
	}
	return n_result;
}

unsigned int CMPAFrame::BE_2_native(boost::shared_ptr<io_base> sp_file_io, unsigned int n_size, unsigned int n_offset)
{
	assert(n_size > 0 && n_size <= 4);
	unsigned char* p_buf = new unsigned char[n_size];
	int n_result = 0;
	n_result = sp_file_io->seek(n_offset, FILE_BEGIN);
	assert(n_result == 0);
	unsigned int n_bytes_2_read = 0;
	n_result = sp_file_io->read(p_buf, n_size, &n_bytes_2_read);
	assert(n_result == 0 && n_bytes_2_read == n_size);
	unsigned int n_value = BE_2_native(p_buf, n_size);
	delete[] p_buf;
	return n_value;
}

// CRC16 check
unsigned short CMPAFrame::CalcCRC16(unsigned char* p_buffer, unsigned int n_bit_size)
{
	unsigned int n;
	unsigned short tmpchar, crcmask, tmpi;
	crcmask = tmpchar = 0;
	unsigned short crc = 0xffff;			// start with inverted value of 0

	// start with byte 2 of header
	for (n = 16;  n < n_bit_size;  n++)
	{
		if (n < 32 || n >= 48) // skip the 2 bytes of the crc itself
		{
			if (n%8 == 0)
			{
				crcmask = 1 << 8;
				tmpchar = p_buffer[n/8];
			}
			crcmask >>= 1;
			tmpi = crc & 0x8000;
			crc <<= 1;
			
			if (!tmpi ^ !(tmpchar & crcmask))
				crc ^= 0x8005;
		}
	}
	crc &= 0xffff;	// invert the result
	return crc;
}

}		//end namespace em_mp3_tag
