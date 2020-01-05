#pragma once
//#include "mpastream.h"
#include "mpaheader.h"
#include "vbrheader.h"

#include "IO.h"
#include <boost/shared_ptr.hpp>

namespace em_mp3_tag
{

class CMPAFrame
{
public:
	static CMPAFrame* seek_first_frame(boost::shared_ptr<io_base> sp_file_io, 
		unsigned int n_begin, unsigned int n_end, unsigned int n_min_frames);
public:
	CMPAFrame();
	~CMPAFrame();

	
	//初始化
	bool init(boost::shared_ptr<io_base> sp_file_io, unsigned int n_begin, unsigned int n_end, const CMPAHeader* p_compare_header);
	//走到下一帧, next时不再进行mpege header的下帧比较
	bool next();


	CVBRHeader* FindVBRHeader() const;
	//取得下一个mpeg header位置偏移
	unsigned int next_frame_offset() const { return mn_offset + mn_length; };
	bool CheckCRC() const;
	bool IsLast() const { return mb_is_last; };
public:
	CMPAHeader m_mpeg_header;
	boost::shared_ptr<io_base> msp_file_io;

	unsigned int mn_offset;			//该帧的起始偏移
	unsigned int mn_length;			//帧数据长度
public:
	static unsigned int BE_2_native(unsigned char* p_buf, unsigned int n_size);
	static unsigned int BE_2_native(boost::shared_ptr<io_base> sp_file_io, unsigned int n_size, unsigned int n_offset);
private:
	static const unsigned int mn_protected_bits_layer2[5][2];
	static unsigned short CalcCRC16(unsigned char* p_buffer, unsigned int n_bit_size);
	bool mb_is_last;						//是否最后一帧

	unsigned int mn_end;				//音频帧的结束位置
	CMPAHeader* mp_first_header;	//第一帧的header
	bool mount(unsigned int n_begin, const CMPAHeader* p_compare_header);
};

}		//end namespace em_mp3_tag
