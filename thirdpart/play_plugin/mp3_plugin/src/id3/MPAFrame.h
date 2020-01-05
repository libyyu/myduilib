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

	
	//��ʼ��
	bool init(boost::shared_ptr<io_base> sp_file_io, unsigned int n_begin, unsigned int n_end, const CMPAHeader* p_compare_header);
	//�ߵ���һ֡, nextʱ���ٽ���mpege header����֡�Ƚ�
	bool next();


	CVBRHeader* FindVBRHeader() const;
	//ȡ����һ��mpeg headerλ��ƫ��
	unsigned int next_frame_offset() const { return mn_offset + mn_length; };
	bool CheckCRC() const;
	bool IsLast() const { return mb_is_last; };
public:
	CMPAHeader m_mpeg_header;
	boost::shared_ptr<io_base> msp_file_io;

	unsigned int mn_offset;			//��֡����ʼƫ��
	unsigned int mn_length;			//֡���ݳ���
public:
	static unsigned int BE_2_native(unsigned char* p_buf, unsigned int n_size);
	static unsigned int BE_2_native(boost::shared_ptr<io_base> sp_file_io, unsigned int n_size, unsigned int n_offset);
private:
	static const unsigned int mn_protected_bits_layer2[5][2];
	static unsigned short CalcCRC16(unsigned char* p_buffer, unsigned int n_bit_size);
	bool mb_is_last;						//�Ƿ����һ֡

	unsigned int mn_end;				//��Ƶ֡�Ľ���λ��
	CMPAHeader* mp_first_header;	//��һ֡��header
	bool mount(unsigned int n_begin, const CMPAHeader* p_compare_header);
};

}		//end namespace em_mp3_tag
