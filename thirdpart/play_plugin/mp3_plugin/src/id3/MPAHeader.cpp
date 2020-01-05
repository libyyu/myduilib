#include "mpaheader.h"
#include <cassert>
#include <string.h>
#include "IO.h"

namespace em_mp3_tag
{
// static variables

//以hertz为单位的采样率（频率）
// sampling rates in hertz: 1. index = MPEG Version ID, 2. index = sampling rate index
const unsigned int CMPAHeader::m_dwSamplingRates[4][3] = 
{ 
	{11025, 12000, 8000,  },	// MPEG 2.5
	{0,     0,     0,     },	// reserved
	{22050, 24000, 16000, },	// MPEG 2
	{44100, 48000, 32000  }		// MPEG 1
};

//编码率(振幅)
// bitrates: 1. index = LSF, 2. index = Layer, 3. index = bitrate index
const unsigned int CMPAHeader::m_dwBitrates[2][3][15] =
{
	{	// MPEG 1
		{0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},	// Layer1
		{0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},	// Layer2
		{0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,}	// Layer3
	},
	{	// MPEG 2, 2.5		
		{0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},		// Layer1
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},			// Layer2
		{0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,}			// Layer3
	}
};

// allowed combination of bitrate (1.index) and mono (2.index)
const bool CMPAHeader::m_bAllowedModes[15][2] =
{
	// {stereo, intensity stereo, dual channel allowed,single channel allowed}
	{true,true},		// free mode
	{false,true},		// 32
	{false,true},		// 48
	{false,true},		// 56
	{true,true},		// 64
	{false,true},		// 80
	{true,true},		// 96
	{true,true},		// 112
	{true,true},		// 128
	{true,true},		// 160
	{true,true},		// 192
	{true,false},		// 224
	{true,false},		// 256
	{true,false},		// 320
	{true,false}		// 384
};

// Samples per Frame: 1. index = LSF, 2. index = Layer
const unsigned int CMPAHeader::m_dwSamplesPerFrames[2][3] =
{
	{	// MPEG 1
		384,	// Layer1
		1152,	// Layer2	
		1152	// Layer3
	},
	{	// MPEG 2, 2.5
		384,	// Layer1
		1152,	// Layer2
		576		// Layer3
	}	
};

// Samples per Frame / 8
const unsigned int CMPAHeader::m_dwCoefficients[2][3] =
{
	{	// MPEG 1
		12,		// Layer1	(must be multiplied with 4, because of slot size)
		144,	// Layer2
		144		// Layer3
	},
	{	// MPEG 2, 2.5
		12,		// Layer1	(must be multiplied with 4, because of slot size)
		144,	// Layer2
		72		// Layer3
	}	
};

// slot size per layer
const unsigned int CMPAHeader::m_dwSlotSizes[3] =
{
	4,			// Layer1
	1,			// Layer2
	1			// Layer3
};

// size of side information (only for Layer III)
// 1. index = LSF, 2. index = mono
const unsigned int CMPAHeader::m_dwSideInfoSizes[2][2] =
{
	// MPEG 1
	{32,17},
	// MPEG 2/2.5
	{17,9}
};

// tolerance range, look at expected offset +/- m_dwTolerance/2 for beginning of a frame
const unsigned int CMPAHeader::m_dwTolerance = 6;	// +/-3 bytes

// max. range where to look for frame sync
const unsigned int CMPAHeader::m_dwMaxRange = 16384;

CMPAHeader::CMPAHeader() : m_wAllocationTableIndex(0), m_wBound(32)
{
}

int CMPAHeader::Init(io_base* p_file_io, unsigned int n_offset, const CMPAHeader* p_compare_header)
{
	assert(p_file_io != NULL);
	unsigned char s_buf[MPA_HEADER_SIZE];
	memset(s_buf, 0x0, MPA_HEADER_SIZE * sizeof(unsigned char));
	// look for synchronization
	unsigned int n_step = 1;

	// is new offset within valid range?
	bool b_found = false;

	unsigned int n_bytes_read = 0;
	int n_result = 0;
	while (true)
	{
		p_file_io->seek(n_offset, FILE_BEGIN);
		n_result = p_file_io->read(s_buf, MPA_HEADER_SIZE, &n_bytes_read);
		if (n_result != 0 || n_bytes_read != MPA_HEADER_SIZE )
			break;
		// sync bytes found?
		// for performance reasons check already that it is not data within an empty frame (all bits set)
		// therefore check whether the bits for bitrate are all set -> means that this is no header!
		if ((s_buf[0] == 0xFF) && ((s_buf[1] & 0xE0) == 0xE0) && ((s_buf[2] & 0xF0) != 0xF0) ) // first 11 bits should be 1
		{
			b_found = _init(s_buf, p_file_io, n_offset, p_compare_header);	
			if (b_found)
				break;
		}

		// just go forward or backward to find sync
		n_offset += 1;

		if (n_step > m_dwMaxRange)
			break;
		n_step++;
	}
	return b_found ? (int)n_offset : -1;
}

// the bit information refers to bit 0 as the most significant bit (MSB) of Byte 0
// decodes the header in pHeader
bool CMPAHeader::_init(unsigned char* pHeader, io_base* p_file_io, unsigned int n_offset, const CMPAHeader* p_compare_header) 
{
	// get MPEG version [bit 11,12]
	m_Version = (mpeg_version_e)((pHeader[1] >> 3) & 0x03);	// mask only the rightmost 2 bits
	if (m_Version == mv_mpeg_reserved)
		return false;

	if (m_Version == mv_mpeg_1)
		m_bLSF = false;
	else
		m_bLSF = true;

	// get layer (0 = layer1, 2 = layer2, ...)  [bit 13,14]
	m_Layer = (mpeg_layer_e)(3 - ((pHeader[1] >> 1) & 0x03));
	if (m_Layer == ml_layer_reserved)
		return false;
	/*
	if (m_Version != mv_mpeg_1 || m_Layer != ml_layer_3)
		return false;
	*/
	// protection bit (inverted) [bit 15]
	m_bCRC = !((pHeader[1]) & 0x01);

	// bitrate [bit 16..19]
	unsigned char bBitrateIndex = (unsigned char)((pHeader[2] >> 4) & 0x0F);
	if (bBitrateIndex == 0x0F)		// all bits set is reserved
		return false;
	m_dwBitrate = m_dwBitrates[m_bLSF][m_Layer][bBitrateIndex] * 1000; // convert from kbit to bit

	if (m_dwBitrate == 0)	// means free bitrate (is unsupported yet)
		return false;

	// sampling rate [bit 20,21]
	unsigned char bIndex = (unsigned char)((pHeader[2] >> 2) & 0x03);
	if (bIndex == 0x03)		// all bits set is reserved
		return false;
	m_dwSamplesPerSec = m_dwSamplingRates[m_Version][bIndex];

	// padding bit [bit 22]
	m_dwPaddingSize = 1 * ((pHeader[2] >> 1) & 0x01);	// in Slots (always 1)
	
	m_dwSamplesPerFrame = m_dwSamplesPerFrames[m_bLSF][m_Layer];

	// private bit [bit 23]
	m_bPrivate = (pHeader[2]) & 0x01;

	// channel mode [bit 24,25]
	m_ChannelMode = (channel_mode_e)((pHeader[3] >> 6) & 0x03);

	// mode extension [bit 26,27]
	m_ModeExt = (unsigned char)((pHeader[3] >> 4) & 0x03);

	// determine the bound for intensity stereo
	if (m_ChannelMode == cm_joint_stereo)
		m_wBound = 4 + m_ModeExt * 4;

	// copyright bit [bit 28]
	m_bCopyright = (pHeader[3] >> 3) & 0x01;
	
	// original bit [bit 29]
	m_bOriginal = (pHeader[3] >> 2) & 0x01;

	// emphasis [bit 30,31]
	m_Emphasis = (emphasis_mode_e)((pHeader[3]) & 0x03);
	if (m_Emphasis == em_reserved)
		return false;

	// extended check for Layer II
	if (m_Layer == ml_layer_2)
	{
		// MPEG 1
		if (m_Version == mv_mpeg_1)
		{
			if (!m_bAllowedModes[bBitrateIndex][IsMono()])
				return false;

			// which allocation table is used
			switch (m_dwBitrate/1000/(IsMono()?1:2))
			{
				case 32:
				case 48:
					if (m_dwSamplesPerSec == 32000)
						m_wAllocationTableIndex = 3;	// table d
					else
						m_wAllocationTableIndex = 2;	// table c
					break;
				case 56:
				case 64:
				case 80:
					if (m_dwSamplesPerSec != 48000)
					{
						m_wAllocationTableIndex = 0;	// table a
						break;
					}
				case 96:
				case 112:
				case 128:
				case 160:
				case 192:
					if (m_dwSamplesPerSec != 48000)
					{
						m_wAllocationTableIndex = 1;	// table b
						break;
					}
					else
						m_wAllocationTableIndex = 0;	// table a
					break;
			}
		}
		else	// MPEG 2/2.5
			m_wAllocationTableIndex = 4;
	}
	bool b_result(false);
	if (p_compare_header != NULL)
	{
		if (*this == *p_compare_header)
			b_result = true;
		else
			b_result = false;
	}
	else
		b_result = true;
	return b_result;
}

// destructor
CMPAHeader::~CMPAHeader()
{
}


// compare headers
// return true if identical or related
// return false if no similarities

bool CMPAHeader::operator==(const CMPAHeader& DestHeader) const
{
	// version change never possible
	if (DestHeader.m_Version != m_Version)
		return false;

	// layer change never possible
	if (DestHeader.m_Layer != m_Layer)
		return false;

	// sampling rate change never possible
	if (DestHeader.m_dwSamplesPerSec != m_dwSamplesPerSec)
		return false;

	// from mono to stereo never possible
	if (DestHeader.IsMono() != IsMono())
		return false;

	if (DestHeader.m_Emphasis != m_Emphasis)
		return false;

	return true;
}

}		//end namespace end
