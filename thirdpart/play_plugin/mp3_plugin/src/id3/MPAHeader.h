#pragma once
#include "tag_base.h"
#include "IO.h"

namespace em_mp3_tag
{

#define MPA_HEADER_SIZE 4	// MPEG-Audio Header Size 32bit
#define MAXTIMESREAD 5

//mpeg audio header
class CMPAHeader
{
public:
	CMPAHeader();
	~CMPAHeader();
	
	bool operator==(const CMPAHeader& DestHeader) const;
	
	//返回真正找到的offset,正常状态下,返回 = n_offset
	int Init(io_base* p_file_io, unsigned int n_offset, const CMPAHeader* p_compare_header);
	unsigned int CalcFrameSize() const 
	{	
		return int(((m_dwCoefficients[m_bLSF][m_Layer] * m_dwBitrate / m_dwSamplesPerSec) + m_dwPaddingSize)) * m_dwSlotSizes[m_Layer]; 
	};
	// bitrate is in bit per second, to calculate in bytes => (/ 8)
	unsigned int GetBytesPerSecond() const 
	{ 
		return m_dwBitrate / 8; 
	};
	// calc number of seconds from number of frames
	unsigned int GetLengthSecond(unsigned int dwNumFrames) const 
	{ 
		return dwNumFrames * m_dwSamplesPerFrame / m_dwSamplesPerSec; 
	};
	unsigned int GetBytesPerSecond(unsigned int dwNumFrames, unsigned int dwNumBytes) const 
	{ 
		return dwNumBytes / GetLengthSecond( dwNumFrames ); 
	};
	bool IsMono() const 
	{ 
		return (m_ChannelMode == cm_single_channel) ? true : false; 
	};
	// true if MPEG2/2.5 otherwise false
	bool IsLSF() const 
	{ 
		return m_bLSF;	
	};
	unsigned int GetSideInfoSize() const 
	{ 
		return m_dwSideInfoSizes[IsLSF()][IsMono()]; 
	};
	unsigned int GetNumChannels() const 
	{ 
		return IsMono() ? 1 : 2; 
	};
private:
	static const unsigned int m_dwMaxRange;
	static const unsigned int m_dwTolerance;

	static const unsigned int m_dwSamplingRates[4][3];
	static const unsigned int m_dwPaddingSizes[3];
	static const unsigned int m_dwBitrates[2][3][15];
	static const bool m_bAllowedModes[15][2];
	static const unsigned int m_dwSamplesPerFrames[2][3];
	static const unsigned int m_dwCoefficients[2][3];
	static const unsigned int m_dwSlotSizes[3];
	static const unsigned int m_dwSideInfoSizes[2][2];
	//true表示低的采样频率
	bool m_bLSF;		// true means lower sampling frequencies (=MPEG2/MPEG2.5)

	bool _init(unsigned char* pHeader, io_base* p_file_io, unsigned int n_offset, const CMPAHeader* p_compare_header);
public:
	mpeg_version_e m_Version;
	mpeg_layer_e m_Layer;
	emphasis_mode_e m_Emphasis;
	channel_mode_e m_ChannelMode;
	
	unsigned int m_dwSamplesPerSec;
	unsigned int m_dwSamplesPerFrame;
	unsigned int m_dwBitrate;	// in bit per second (1 kb = 1000 bit, not 1024)
	unsigned int m_dwPaddingSize;
	unsigned short m_wBound;		// only valid for intensity stereo
	unsigned short m_wAllocationTableIndex;	// only valid for MPEG 1 Layer II (0=table a, 1=table b,...)
	
	// flags
	bool m_bCopyright, m_bPrivate, m_bOriginal;
	bool m_bCRC; 
	unsigned char m_ModeExt;
};

}		//end namespace em_mp3_tag
