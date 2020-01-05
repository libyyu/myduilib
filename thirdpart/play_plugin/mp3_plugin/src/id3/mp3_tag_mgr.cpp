#include "mp3_tag_mgr.h"
#include <cassert>
#include "FileIO.h"
#include <string>
#include <utility>
#include "VBRHeader.h"
#include <iostream>
#include "id3_tag_wrapper.h"
#include "ape_tag_wrapper.h"
#include "MPAFrame.h"
#include "Lyrics3Tag.h"
#include <boost/shared_ptr.hpp>

#include "VBRHeader.h"
#include "XINGHeader.h"
#include "VBRIHeader.h"
#include "LameTag.h"
#include "encrypt.h"
using namespace std;

namespace em_mp3_tag
{
	mp3_file::mp3_file(unsigned int n_check_frames)
	{
		register_default_factory();
		mn_audio_begin = 0;
		mn_audio_end = 0;
		mb_read_only = false;
		mb_encrypt_data=false;
		if (n_check_frames < CHECK_MINI_FRAMES)
			n_check_frames = CHECK_MINI_FRAMES;
		mn_check_frames = n_check_frames;
		mn_last_frame_end = 0;
	}

	mp3_file::~mp3_file()
	{
		unload();
	}

	bool mp3_file::load(const char* p_file_name, bool b_parse, bool b_read_only,bool b_parse_tag_only)
	{
		bool b_result = false;
		try
		{
			unload();
			if (p_file_name == NULL)
				return false;
			msp_file.reset(new file_io());
			mb_read_only = b_read_only;
			int n_result = msp_file->open(p_file_name, mb_read_only);
			if (n_result == 0)
			{
				mn_audio_begin = 0;
				mn_audio_end = msp_file->get_size();
				if (mn_audio_end > 0)
				{
					b_result = true;
					if (b_parse)
						b_result = parse(b_parse_tag_only);
					mb_encrypt_data = is_have_encrypt_data();
				}
			}
		}
		catch (...)
		{
		}
		return b_result;
	}

	bool mp3_file::load(const wchar_t* p_file_name, bool b_parse, bool b_read_only,bool b_parse_tag_only)
	{
		bool b_result = false;
		try
		{
			unload();
			if (p_file_name == NULL)
				return false;
			msp_file.reset(new file_io());
			mb_read_only = b_read_only;
			int n_result = msp_file->open(p_file_name, mb_read_only);
			if (n_result == 0)
			{
				mn_audio_begin = 0;
				mn_audio_end = msp_file->get_size();
				if (mn_audio_end > 0)
				{
					b_result = true;
					if (b_parse)
						b_result = parse(b_parse_tag_only);
					mb_encrypt_data=is_have_encrypt_data();
				}
			}
		}
		catch (...)
		{
		}
		return b_result;
	}

	void mp3_file::unload()
	{
		try
		{
			m_tag_list.clear();
			mn_audio_begin  = 0;
			mn_audio_end = 0;
			mb_read_only = false;
			msp_first_frame.reset();
			msp_file.reset();
		}
		catch (...)
		{
		}
		return;
	}

	bool mp3_file::parse(bool b_parse_tag_only)
	{
		bool b_result = false;
		assert(msp_file);
		m_tag_list.clear();

		mn_audio_begin = 0;
		mn_audio_end = msp_file->get_size();
		if (mn_audio_end == 0)
			return false;
		int n_result = 	msp_file->seek(0, FILE_BEGIN);
		assert(n_result == 0);
		unsigned int n_tag_len = 0;
BEGIN_CHECK:
		try
		{
			for (factory_iter i = m_tag_factories.begin(); i != m_tag_factories.end(); ++i)
			{
				tag_factory_ptr sp_factory = *i;
				mp3_tag_interface* p_tag = sp_factory->create_tag(msp_file.get(), mn_audio_begin, mn_audio_end);
				if (p_tag != NULL)		//????????§¹tag
				{
					std::string s_tag_name = p_tag->get_tag_name();
					//cout<<s_tag_name<<endl;
					unsigned int n_serial = p_tag->get_tag_serial();
					n_tag_len = p_tag->get_length();
					if (p_tag->is_append())				//???¦Â????tag
					{
						assert(p_tag->get_offset() + n_tag_len == mn_audio_end);
						mn_audio_end -= n_tag_len;
					}
					else											//????????tag
					{
						assert(mn_audio_begin == p_tag->get_offset());
						mn_audio_begin += n_tag_len;
					}
					m_tag_list.insert(make_pair(n_serial, tag_ptr_type(p_tag)));
					goto BEGIN_CHECK;
				}
			}
		}
		catch (...)
		{
		}
		if(mn_audio_end <= mn_audio_begin)
			return false;
		//assert(mn_audio_end > mn_audio_begin);
		try
		{
			if(!b_parse_tag_only)
				b_result = seek_first_frame();				//??????????
			else
				b_result = true;
		}
		catch (...)
		{
		}
		return b_result;
	}

	void mp3_file::register_default_factory()
	{
		m_tag_factories.clear();
		m_tag_factories.push_back(boost::shared_ptr<mp3_tag_factory>(new id3_v1_factory()));
		m_tag_factories.push_back(boost::shared_ptr<mp3_tag_factory>(new id3_v2_factory()));
		m_tag_factories.push_back(boost::shared_ptr<mp3_tag_factory>(new ape_tag_factory()));
		m_tag_factories.push_back(boost::shared_ptr<mp3_tag_factory>(new lyrics_tag_factory()));
	}

	unsigned int mp3_file::get_size()
	{
		assert(msp_file);
		unsigned int n_size = msp_file->get_size();
		return n_size;
	}

	unsigned int mp3_file::get_tag_count()
	{
		return m_tag_list.size();
	}

	mp3_file::tag_ptr_type mp3_file::get_tag(unsigned int n_index)
	{
		assert(n_index < m_tag_list.size());
		tag_iter pos = m_tag_list.begin();
		for (unsigned int i = 0; i < n_index; ++i)
			++pos;
		return pos->second;
	}

	mp3_file::tag_ptr_type mp3_file::get_tag_by_serial(unsigned int n_serial)
	{
		tag_iter pos = m_tag_list.find(n_serial);
		if (pos != m_tag_list.end())
			return pos->second;
		return tag_ptr_type((mp3_tag_interface*)NULL);
	}

	boost::shared_ptr<CMPAFrame> mp3_file::get_first_frame()
	{
		return msp_first_frame;
	}

	unsigned int mp3_file::get_audio_begin() const
	{
		return mn_audio_begin;
	}

	unsigned int mp3_file::get_audio_end() const
	{
		if(mb_encrypt_data)
			return mn_audio_end-626;//?????????§³
		else			//??§Þ???????
			return mn_audio_end;
	}

	unsigned int mp3_file::get_len_seconds(CMPAFrame* p_first_frame)
	{
		if (p_first_frame == NULL)
			return 0;
		unsigned int n_file_size = msp_file->get_size();
		unsigned int n_bytes_per_seconds = 0;
		n_bytes_per_seconds = p_first_frame->m_mpeg_header.GetBytesPerSecond();

		return n_bytes_per_seconds;

		CVBRHeader* p_VBR_header = p_first_frame->FindVBRHeader();
		// in case of a VBR header we know the bitrate (if at least the number of frames is known)
		if (p_VBR_header && p_VBR_header->mn_frames > 0)
		{
			unsigned int n_bytes = p_VBR_header->mn_bytes;
			// number of bytes can be guessed
			if (n_bytes == 0)
			{
				//p_VBR_header->mn_bytes = n_file_size;
				n_bytes = n_file_size;
			}
			n_bytes_per_seconds = p_first_frame->m_mpeg_header.GetBytesPerSecond(p_VBR_header->mn_frames, n_bytes);
			return n_bytes_per_seconds;
		}
		if (p_VBR_header)
		{
			delete p_VBR_header;
			p_VBR_header = NULL;
		}
		// otherwise we have to guess it

		// go through all frames that have a lower bitrate than 48kbit
		n_bytes_per_seconds = p_first_frame->m_mpeg_header.GetBytesPerSecond();
		bool b_result(false);
		CMPAFrame* p_frame = p_first_frame;
		unsigned int n_frames = 0;
		static const unsigned int MAX_EMPTY_FRAMES = 5;
		while (p_frame && p_frame->m_mpeg_header.m_dwBitrate <= 48000)
		{
			b_result = p_frame->next();
			assert(b_result);
			if (n_frames++ > MAX_EMPTY_FRAMES)	
				break;
		};
		n_bytes_per_seconds = p_frame->m_mpeg_header.GetBytesPerSecond();
		return n_bytes_per_seconds;
	}

	boost::shared_ptr<mpeg_info> mp3_file::get_mpeg_info()
	{
		mpeg_info* p_info = NULL;
		boost::shared_ptr<CMPAFrame> sp_frame =get_first_frame();
		if (sp_frame)
		{
			p_info = new mpeg_info();
			p_info->mpa_version = sp_frame->m_mpeg_header.m_Version;
			p_info->mpa_layer = sp_frame->m_mpeg_header.m_Layer;
			p_info->channel_mode = sp_frame->m_mpeg_header.m_ChannelMode;
			p_info->sample_rate = sp_frame->m_mpeg_header.m_dwSamplesPerSec;
			p_info->bit_rate =  sp_frame->m_mpeg_header.m_dwBitrate;
			p_info->e_bitrate_type = eb_CBR;
			p_info->bit_rate_in_vbr = 0;

			unsigned int n_bytes_per_seconds = get_len_seconds(sp_frame.get());
			unsigned int n_file_size = msp_file->get_size();
			if (n_file_size == 0)
				p_info->seconds_length = 0;
			else
				p_info->seconds_length = n_file_size / n_bytes_per_seconds;

			CVBRHeader* p_header = sp_frame->FindVBRHeader();
			if (p_header != NULL)
			{
				xing_header* p_XING_header = dynamic_cast<xing_header*>(p_header);
				if (p_XING_header != NULL)
				{
					if  (p_XING_header->mp_lame_tag != NULL)
					{
						if (p_XING_header->mp_lame_tag->is_VBR())
						{
							p_info->e_bitrate_type = eb_VBR;
							p_info->bit_rate_in_vbr = p_XING_header->mp_lame_tag->mn_bitrate;		//minimum bitrate kbps
						}
						else if (p_XING_header->mp_lame_tag->is_CBR())
						{
							p_info->e_bitrate_type = eb_CBR;
							p_info->bit_rate_in_vbr = p_XING_header->mp_lame_tag->mn_bitrate;
						}
						else if (p_XING_header->mp_lame_tag->is_ABR())
						{
							p_info->e_bitrate_type = eb_ABR;
							p_info->bit_rate_in_vbr = p_XING_header->mp_lame_tag->mn_bitrate;			//Average bitrate kbps
						}
						else				//????VBR
						{
							p_info->e_bitrate_type = eb_VBR;
							p_info->bit_rate_in_vbr = p_XING_header->mp_lame_tag->mn_bitrate;
						}
					}
					else
						p_info->e_bitrate_type = eb_VBR;
				}
				vbr_I_header* p_VBRI_header = dynamic_cast<vbr_I_header*>(p_header);
				if (p_VBRI_header != NULL)
				{
					p_info->e_bitrate_type = eb_VBR;
					p_info->bit_rate_in_vbr = 0;
				}
				delete p_header;
				p_header = NULL;
			}
		}
		return boost::shared_ptr<mpeg_info>(p_info);
	}
	bool mp3_file::has_id3v2_pic()
	{
		bool b_result = false;
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v2);
		if (sp_tag)
		{
			id3_v2_tag* p_tag_v2 = dynamic_cast<id3_v2_tag*>(sp_tag.get());
			assert(p_tag_v2 != NULL);
			b_result = p_tag_v2->has_picture();
		}
		return b_result;
	}
	bool mp3_file::rip_id3v2_pic(const char* p_dir)
	{
		bool b_result(false);
		assert(msp_file);
		int n_result = 0;
		char s_buf[512];
		n_result = msp_file->get_name(s_buf);
		assert(n_result == 0);
		char* p_pos = strrchr(s_buf, '.');
		assert(p_pos != NULL);
		*p_pos = 0x0;
		string s_raw_name;
		if (p_dir == NULL)
		{
			s_raw_name = s_buf;
		}
		else
		{
			s_raw_name = p_dir;
			assert(!s_raw_name.empty());
			std::replace(s_raw_name.begin(), s_raw_name.end(), '\\', '/');
			if (s_raw_name[s_raw_name.length() - 1] != '/')
				s_raw_name += '/';
			char* p_begin = strrchr(s_buf, '/');
			if (p_begin == NULL)
				p_begin = strrchr(s_buf, '\\');
			assert(p_begin != NULL);
			++p_begin;
			s_raw_name += p_begin;
		}
		//string s_raw_name = s_buf;
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v2);
		unsigned int n_rip_count = 0;
		if (sp_tag)
		{
			id3_v2_tag* p_tag_v2 = dynamic_cast<id3_v2_tag*>(sp_tag.get());
			assert(p_tag_v2 != NULL);
			n_rip_count = p_tag_v2->rip_picture(s_raw_name.c_str());
		}
		return n_rip_count > 0;
	}

	bool mp3_file::add_picture(const wchar_t *file_name)
	{
		bool b_result = false;
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v2);
		unsigned int n_rip_count = 0;
		if (sp_tag)
		{
			id3_v2_tag* p_tag_v2 = dynamic_cast<id3_v2_tag*>(sp_tag.get());
			assert(p_tag_v2 != NULL);
			tag_base_data_w *old_tag_data =p_tag_v2->get_base_tag_wstring();
			id3_v2_tag  new_tag;
			
			b_result = new_tag.add_picture(file_name);
			if (b_result)
			{
				b_result = new_tag.import_wstring(old_tag_data);
				new_tag.attach_inner_frame(p_tag_v2,true,false);			
				if (b_result)
				{
					long n_old_len=p_tag_v2->get_length();
					int n_offset = p_tag_v2->get_offset();

					assert(new_tag.get_offset() == 0);

					b_result = msp_file->cut_header(n_old_len);
					assert(b_result == 0);
					int n_new_len = new_tag.render(NULL);
					if (n_new_len <= 0 || n_new_len > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
						return false;
					new_tag.set_padding_size(n_new_len);

					assert(n_new_len > 0);

					static const unsigned int EMPTY_PADDING_SIZE = 0;
					unsigned char* p_buf = new unsigned char[n_new_len + EMPTY_PADDING_SIZE];
					assert(p_buf);
					memset(p_buf, 0x0, n_new_len + EMPTY_PADDING_SIZE);

					long n_tag_len = new_tag.render(p_buf);

					assert(n_tag_len == n_new_len);

					b_result = msp_file->attach_header(p_buf, n_new_len + EMPTY_PADDING_SIZE) == 0;
					assert(b_result);
					delete[] p_buf;
				}
			/*
			new_tag.import_wstring(old_tag_data);

			//??id3_v2 tag ???????
			bool n_result = new_tag.add_picture(file_name);

			//?????????????mp3?????
			if(n_result)
			{

				long n_old_len=p_tag_v2->get_length();
				int n_offset = p_tag_v2->get_offset();
		
				assert(new_tag.get_offset() == 0);

				n_result = msp_file->cut_header(n_old_len);
				assert(n_result == 0);
				int n_new_len = new_tag.render(NULL);
				new_tag.set_padding_size(n_new_len);

				assert(n_new_len > 0);
			
				static const unsigned int EMPTY_PADDING_SIZE = 10;
				unsigned char* p_buf = new unsigned char[n_new_len + EMPTY_PADDING_SIZE];
				assert(p_buf);
				memset(p_buf, 0x0, n_new_len + EMPTY_PADDING_SIZE);

				long n_tag_len = new_tag.render(p_buf);
				
				assert(n_tag_len == n_new_len);
							
				n_result = msp_file->attach_header(p_buf, n_new_len + EMPTY_PADDING_SIZE);

				assert(n_result == 0);
				delete[] p_buf;
				
				return true;
				*/
			}
			delete old_tag_data;
		}
		return true;
	}
	int mp3_file::del_tail_tags()
	{
		if (mb_read_only)
		{
			return 0;
		}
		int n_result = 0;
		tag_ptr_type sp_id3v1 = get_tag_by_serial(st_id3_v1);
		tag_ptr_type sp_ape = get_tag_by_serial(st_ape);
		int n_offset = msp_file->get_size();
		int n_len = 0;
		if (sp_id3v1)			//??id3v1 tag
		{
			int n_v1_offset = sp_id3v1->get_offset();
			int n_v1_len = sp_id3v1->get_length();
			assert(n_v1_offset > 0);
			assert(n_v1_offset < n_offset);
			assert(n_v1_len == TAG_ID3_V1_LEN);
			n_offset = n_v1_offset;
			n_result = 1;
		}
		if (sp_ape)				//??ape tag
		{
			if (!sp_id3v1)
				n_result = 2;
			else
				n_result = 3;
			int n_ape_offset = sp_ape->get_offset();
			int n_ape_len = sp_ape->get_length();
			assert(n_ape_offset > 0 && n_ape_len > 0);
			if (n_ape_offset < n_offset)						//ape tag??id3 v1 tag???
				n_offset = n_ape_offset;			
		}
		n_len = msp_file->get_size() - n_offset;
		if (n_len > 0)
		{
			int n_cut_result = msp_file->cut_tail(n_len);
			assert(n_cut_result == 0);
		}
		return n_result;
	}

	bool mp3_file::rip_id3v2_pic(const wchar_t* s_raw_name)
	{
		assert(s_raw_name != NULL);
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v2);
		unsigned int n_rip_count = 0;
		if (sp_tag)
		{
			id3_v2_tag* p_tag_v2 = dynamic_cast<id3_v2_tag*>(sp_tag.get());
			assert(p_tag_v2 != NULL);
			n_rip_count = p_tag_v2->rip_picture(s_raw_name);
		}
		return n_rip_count > 0;
	}

	bool mp3_file::write_id3v1_tag(const tag_base_data* p_data)
	{
		bool b_result(false);
		assert(msp_file && !mb_read_only);
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v1);
		if (p_data == NULL)				//???tag
		{
			if (sp_tag)							//????tag
			{
				string s_name = sp_tag->get_tag_name();
				int n_offset = sp_tag->get_offset();
				int n_len = sp_tag->get_length();
				assert(n_len > 0);
				if (n_offset + n_len == msp_file->get_size())					//???????¦Â??
				{
					int n_result = msp_file->cut_tail(n_len);
					assert(n_result == 0);
					b_result = true;
				}
			}
		}
		else								//????????tag
		{
			if (!sp_tag)					//?????tag
			{
				id3_v1_tag v1_tag;
				b_result = v1_tag.import(p_data);
				int n_result = v1_tag.render(NULL);
				if (n_result <= 0 || n_result > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
					return false;
				assert(n_result > 0);
				unsigned char* p_buf = new unsigned char[n_result];
				int n_len = v1_tag.render(p_buf);
				assert(n_len == n_result);
				n_result = msp_file->attach_header(p_buf, n_len);
				assert(n_result == 0);
				delete[] p_buf;
				p_buf = NULL;
			}
			else //?????tag,???????????,?????§Õ?????tag
			{
				//??????????????tag
				string s_name = sp_tag->get_tag_name();
				int n_offset = sp_tag->get_offset();
				int n_len = sp_tag->get_length();
				assert(n_len > 0);
				if (n_offset + n_len == msp_file->get_size())					//???????¦Â??
				{
					int n_result = msp_file->cut_tail(n_len);
					assert(n_result == 0);
					b_result = true;
				}
				//§Õ?????Tag
				id3_v1_tag v1_tag;
				b_result = v1_tag.import(p_data);
				int n_result = v1_tag.render(NULL);
				if (n_result <= 0 || n_result > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
					return false;
				assert(n_result > 0);
				unsigned char* p_buf = new unsigned char[n_result];
				n_len = v1_tag.render(p_buf);
				assert(n_len == n_result);
				n_result = msp_file->attach_header(p_buf, n_len);
				assert(n_result == 0);
				delete[] p_buf;
				p_buf = NULL;
			}
		}
		return true;
	}



	bool mp3_file::write_id3v1_tag_wstring(const tag_base_data_w* p_data)
	{
		bool b_result(false);
		assert(msp_file && !mb_read_only);
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v1);
		if (p_data == NULL)				//???tag
		{
			if (sp_tag)							//????tag
			{
				string s_name = sp_tag->get_tag_name();
				int n_offset = sp_tag->get_offset();
				int n_len = sp_tag->get_length();
				assert(n_len > 0);
				if (n_offset + n_len == msp_file->get_size())					//???????¦Â??
				{
					int n_result = msp_file->cut_tail(n_len);
					assert(n_result == 0);
					b_result = true;
				}
			}
		}
		else								//????????tag
		{
			if (!sp_tag)					//?????tag
			{
				id3_v1_tag v1_tag;
				b_result = v1_tag.import_wstring(p_data);
				int n_result = v1_tag.render(NULL);
				if (n_result <= 0 || n_result > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
					return false;
				assert(n_result > 0);
				unsigned char* p_buf = new unsigned char[n_result];
				int n_len = v1_tag.render(p_buf);
				assert(n_len == n_result);
				n_result = msp_file->attach_header(p_buf, n_len);
				assert(n_result == 0);
				delete[] p_buf;
				p_buf = NULL;
			}
			else //?????tag,???????????,?????§Õ?????tag
			{
				//??????????????tag
				string s_name = sp_tag->get_tag_name();
				int n_offset = sp_tag->get_offset();
				int n_len = sp_tag->get_length();
				assert(n_len > 0);
				if (n_offset + n_len == msp_file->get_size())					//???????¦Â??
				{
					int n_result = msp_file->cut_tail(n_len);
					assert(n_result == 0);
					b_result = true;
				}
				//§Õ?????Tag
				id3_v1_tag v1_tag;
				b_result = v1_tag.import_wstring(p_data);
				int n_result = v1_tag.render(NULL);
				if (n_result <= 0 || n_result > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
					return false;
				assert(n_result > 0);
				unsigned char* p_buf = new unsigned char[n_result];
				n_len = v1_tag.render(p_buf);
				assert(n_len == n_result);
				n_result = msp_file->attach_header(p_buf, n_len);
				assert(n_result == 0);
				delete[] p_buf;
				p_buf = NULL;
			}
		}
		return true;
	}

	bool mp3_file::update_id3v2_tag(const tag_base_data* p_data)
	{
		bool b_result(false);
		int n_result = 0;
		assert(p_data != NULL);
		assert(msp_file && !mb_read_only);
		tag_ptr_type sp_old_tag = get_tag_by_serial(st_id3_v2);
		if (!sp_old_tag)
			return false;
		string s_name = sp_old_tag->get_tag_name();
		int n_old_len = sp_old_tag->get_length();
		if (n_old_len <= 0 || n_old_len > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
			return false;
		assert(n_old_len > 0);
		id3_v2_tag v2_new_tag;
		b_result = v2_new_tag.import(p_data);
		int n_new_len = v2_new_tag.render(NULL);
		if (n_new_len <= 0 || n_new_len > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
			return false;
		assert(n_new_len > 0);
		//?????tag?§Ö???????
		b_result = v2_new_tag.attach_inner_frame(sp_old_tag.get());
		//
		n_new_len = v2_new_tag.render(NULL);
		assert(n_new_len > 0);
		int n_offset = sp_old_tag->get_offset();
		if (n_old_len >= n_new_len)				//???§á?s??????data
		{
			v2_new_tag.set_padding_size(n_old_len);
			unsigned char* p_buf = new unsigned char[n_old_len];
			n_result = v2_new_tag.render(p_buf);
			assert(n_result == n_old_len);
			n_result = msp_file->seek(n_offset, FILE_BEGIN);
			assert(n_result == 0);
			unsigned int n_bytes_written = 0;
			n_result = msp_file->write(p_buf, n_old_len, &n_bytes_written);
			assert(n_result == 0 && n_bytes_written == n_old_len);
			delete[] p_buf;
		}
		else						//???tag?????
		{
			assert(sp_old_tag->get_offset() == 0);
			unsigned char* p_buf = new unsigned char[n_new_len];
			n_result = v2_new_tag.render(p_buf);
			assert(n_result == n_new_len);
			n_result = msp_file->cut_header(n_old_len);
			assert(n_result == 0);
			n_result = msp_file->attach_header(p_buf, n_new_len);
			assert(n_result == 0);
			delete[] p_buf;
		}
		return true;
	}

	bool mp3_file::update_id3v2_tag_wstring(const tag_base_data_w* p_data)
	{
		bool b_result(false);
		int n_result = 0;
		assert(p_data != NULL);
		assert(msp_file && !mb_read_only);
		tag_ptr_type sp_old_tag = get_tag_by_serial(st_id3_v2);
		if (!sp_old_tag)
			return false;
		string s_name = sp_old_tag->get_tag_name();
		int n_old_len = sp_old_tag->get_length();
		if (n_old_len <= 0 || n_old_len > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
			return false;
		assert(n_old_len > 0);
		id3_v2_tag v2_new_tag;
		b_result = v2_new_tag.import_wstring(p_data);
		int n_new_len = v2_new_tag.render(NULL);
		assert(n_new_len > 0);
		//?????tag?§Ö???????
		b_result = v2_new_tag.attach_inner_frame(sp_old_tag.get());
		
		n_new_len = v2_new_tag.render(NULL);
		if (n_new_len <= 0 || n_new_len > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
			return false;
		assert(n_new_len > 0);
		int n_offset = sp_old_tag->get_offset();
		if (n_old_len >= n_new_len)				//???§á?s??????data
		{
			v2_new_tag.set_padding_size(n_old_len);
			unsigned char* p_buf = new unsigned char[n_old_len];
			n_result = v2_new_tag.render(p_buf);
			assert(n_result == n_old_len);
			n_result = msp_file->seek(n_offset, FILE_BEGIN);
			assert(n_result == 0);
			unsigned int n_bytes_written = 0;
			n_result = msp_file->write(p_buf, n_old_len, &n_bytes_written);
			assert(n_result == 0 && n_bytes_written == n_old_len);
			delete[] p_buf;
		}
		else						//???tag?????
		{
			assert(sp_old_tag->get_offset() == 0);
			unsigned char* p_buf = new unsigned char[n_new_len];
			n_result = v2_new_tag.render(p_buf);
			assert(n_result == n_new_len);
			n_result = msp_file->cut_header(n_old_len);
			assert(n_result == 0);
			n_result = msp_file->attach_header(p_buf, n_new_len);
			assert(n_result == 0);
			delete[] p_buf;
		}
		return true;
	}

	bool mp3_file::seek_first_frame()
	{
		bool b_find(false);
		assert(msp_file);
		CMPAFrame* p_frame = NULL;
		p_frame = CMPAFrame::seek_first_frame(msp_file, mn_audio_begin, mn_audio_end, mn_check_frames);
		if (p_frame != NULL)
		{
			assert(p_frame->mn_offset >= mn_audio_begin);
			mn_audio_begin = p_frame->mn_offset;
			b_find = true;
		}
		/*
		CMPAFrame* p_frame = new CMPAFrame();
		bool b_result = false;
		while (true)
		{
		b_result = p_frame->init(msp_file, mn_audio_begin, mn_audio_end, NULL);
		if (b_result)		//??¦Ë???
		{
		unsigned int n_offset = p_frame->mn_offset;
		assert(p_frame->mn_offset >= mn_audio_begin);
		assert(p_frame->mn_offset <= mn_audio_end);
		mn_audio_begin = p_frame->mn_offset;
		break;
		}
		else					//??¦Ë???
		{
		//assert(p_frame->mn_offset >= mn_audio_begin);
		assert(p_frame->mn_offset <= mn_audio_end);
		if (p_frame->mn_offset == mn_audio_begin || p_frame->mn_offset == 0)
		{
		if (mn_audio_begin == mn_audio_end)
		break;
		else
		++mn_audio_begin;
		}
		else
		//mn_audio_begin = p_frame->mn_offset;		//???????????
		++mn_audio_begin;
		}
		}
		if (!b_result)
		{
		delete p_frame;
		p_frame = NULL;
		}
		*/
		msp_first_frame.reset(p_frame);
		return b_find;
	}

	bool mp3_file::add_id3v2_tag(const tag_base_data* p_data)
	{
		bool b_result(false);
		assert(msp_file && !mb_read_only);
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v2);
		if (p_data == NULL && sp_tag)			//???id3v2_tag
		{
			string s_name = sp_tag->get_tag_name();
			int n_offset = sp_tag->get_offset();
			if (n_offset != 0)
				return false;
			int n_len = sp_tag->get_length();
			assert(n_len > 0);
			int n_result = msp_file->cut_header(n_len);
			assert(n_result == 0);
			b_result = true;
		}
		else if (p_data != NULL && !sp_tag)			//????id3v2_tag
		{
			assert(!sp_tag);
			id3_v2_tag v2_tag;
			b_result = v2_tag.import(p_data);
			int n_result = v2_tag.render(NULL);
			if (n_result <= 0 || n_result > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
				return false;
			assert(n_result > 0);
			unsigned char* p_buf = new unsigned char[n_result];
			int n_len = v2_tag.render(p_buf);
			assert(n_len == n_result);
			n_result = msp_file->attach_header(p_buf, n_len);
			assert(n_result == 0);
			delete[] p_buf;
			p_buf = NULL;
			b_result = true;
		}
		return b_result;		
	}
	bool mp3_file::add_id3v2_tag_wstring(const tag_base_data_w* p_data)
	{
		bool b_result(false);
		assert(msp_file && !mb_read_only);
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v2);
		if (p_data == NULL && sp_tag)			//???id3v2_tag
		{
			string s_name = sp_tag->get_tag_name();
			int n_offset = sp_tag->get_offset();
			if (n_offset != 0)
				return false;
			int n_len = sp_tag->get_length();
			assert(n_len > 0);
			int n_result = msp_file->cut_header(n_len);
			assert(n_result == 0);
			b_result = true;
		}
		else if (p_data != NULL && !sp_tag)			//????id3v2_tag
		{
			assert(!sp_tag);
			id3_v2_tag v2_tag;
			b_result = v2_tag.import_wstring(p_data);
			int n_result = v2_tag.render(NULL);
			if (n_result <= 0 || n_result > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
				return false;
			assert(n_result > 0);
			unsigned char* p_buf = new unsigned char[n_result];
			int n_len = v2_tag.render(p_buf);
			assert(n_len == n_result);
			n_result = msp_file->attach_header(p_buf, n_len);
			assert(n_result == 0);
			delete[] p_buf;
			p_buf = NULL;
			b_result = true;
		}
		return b_result;		
	}

	bool mp3_file::write_id3v2_tag(const tag_base_data* p_data)
	{
		assert(msp_file && !mb_read_only);
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v2);
		if ((p_data == NULL && sp_tag) || (p_data != NULL && !sp_tag))
		{
			return add_id3v2_tag(p_data);
		}
		else if (p_data != NULL && sp_tag)
		{
			return update_id3v2_tag(p_data);
		}
		return false;	
	}
	bool mp3_file::write_id3v2_tag_wstring(const tag_base_data_w* p_data)
	{
		assert(msp_file && !mb_read_only);
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v2);
		if ((p_data == NULL && sp_tag) || (p_data != NULL && !sp_tag))
		{
			return add_id3v2_tag_wstring(p_data);
		}
		else if (p_data != NULL && sp_tag)
		{
			return update_id3v2_tag_wstring(p_data);
		}
		return false;	
	}

	check_result mp3_file::check_content(bool b_check_CRC, bool b_adjust)
	{
		check_result result;
		result.e_result = ce_success;
		result.e_bitrate_type = eb_CBR;
		result.n_err_frame = 0;
		result.n_frames = 0;
		result.n_unpadding_frames = 0;
		result.n_padding_frames = 0;
		result.n_bitrate = 0;
		result.n_seconds_length = 0;
		result.max_bitrate = result.min_bitrate = 0;
		result.n_adjust_result = 0;
		result.average_bitrate = 0;
		unsigned int n_index = 0;

		try
		{
			boost::shared_ptr<CMPAFrame> sp_frame = get_first_frame();
			if (!sp_frame)
			{
				result.e_result = ce_seek_failed;
				return result;
			}

			unsigned int n_aver_bitrate = sp_frame->m_mpeg_header.m_dwBitrate;			

			bool b_VBR = false;
			CMPAHeader base_header = sp_frame->m_mpeg_header;
			unsigned int n_next_offset = 0;
			do
			{
				++n_index;

				//??????CRC§µ??
				if (b_check_CRC && sp_frame->m_mpeg_header.m_bCRC)
				{
					if (!sp_frame->CheckCRC())
					{
						result.e_result = ce_CRC_error;
						result.n_err_frame = n_index;
						break;
					};
				}

				//??????????(VBR or ABR)
				if (sp_frame->m_mpeg_header.m_dwBitrate != n_aver_bitrate)
				{
					// aver. bitrate += (new bitrate - aver. bitrate / #frames)
					// convert to int (some values can be negative)
					n_aver_bitrate += (int) ((int) sp_frame->m_mpeg_header.m_dwBitrate - (int) n_aver_bitrate) / (int) n_index;
					b_VBR = true;
				}

				if (result.min_bitrate == 0 || sp_frame->m_mpeg_header.m_dwBitrate < result.min_bitrate)
				{
					result.min_bitrate = sp_frame->m_mpeg_header.m_dwBitrate;
				}
				if (sp_frame->m_mpeg_header.m_dwBitrate > result.max_bitrate)
				{
					result.max_bitrate = sp_frame->m_mpeg_header.m_dwBitrate;
				}

				//??????????????????VBR???????????????
				if (sp_frame->m_mpeg_header.m_dwSamplesPerSec != base_header.m_dwSamplesPerSec)		//????????
				{
					result.e_result = ce_sample_change;
					result.n_err_frame = n_index;
					break;
				}
				if (sp_frame->m_mpeg_header.m_Layer != base_header.m_Layer)			//layer
				{
					result.e_result = ce_layer_change;
					result.n_err_frame = n_index;
					break;
				}
				if (sp_frame->m_mpeg_header.m_Version != base_header.m_Version)		//mpeg version
				{
					result.e_result = ce_version_change;
					result.n_err_frame = n_index;
					break;
				}

				//???¦Ë????
				if (n_index > 1 && sp_frame->mn_offset != n_next_offset)			
				{
					result.e_result = ce_frame_offset;
					result.n_err_frame = n_index;
					break;
				}

				if (sp_frame->m_mpeg_header.m_dwPaddingSize > 0)
				{
					++result.n_padding_frames;
				}
				else
				{
					++result.n_unpadding_frames;
				}
				n_next_offset = sp_frame->next_frame_offset();
			} while (sp_frame->next());

			result.average_bitrate = n_aver_bitrate;

			n_next_offset = sp_frame->next_frame_offset();
			if (n_next_offset /*sp_frame->next_frame_offset()*/ > mn_audio_end)
			{
				result.e_result =ce_last_truncated;
				result.n_extend = mn_audio_end - sp_frame->mn_offset;
			}
			else if (n_next_offset /* sp_frame->next_frame_offset()*/ < mn_audio_end)
			{
				result.e_result = ce_free_space;
				result.n_extend = mn_audio_end - n_next_offset;
			}
			result.n_seconds_length = sp_frame->m_mpeg_header.GetLengthSecond(n_index);

			if (b_VBR)
			{
				result.e_bitrate_type = eb_VBR;
				result.n_bitrate = n_aver_bitrate / 1000;
			}
			else
			{
				result.e_bitrate_type = eb_CBR;
				result.n_bitrate = n_aver_bitrate / 1000;
			}
			result.n_frames = n_index;
			if (b_adjust)
			{
				if (mn_audio_end == msp_file->get_size())
				{
					if (result.e_result == ce_free_space)
					{
						assert(result.n_extend > 0);

						int n_result = msp_file->cut_tail(result.n_extend);
						if (n_result == 0)
						{
							result.n_adjust_result = 1;
						}	
						else
						{
							result.n_adjust_result = -1;
						}
					}
					else if (result.e_result == ce_last_truncated)
					{
						assert(result.n_extend > 0);

						int n_result = msp_file->cut_tail(result.n_extend);
						if (n_result == 0)
						{
							result.n_adjust_result = 1;
						}	
						else
						{
							result.n_adjust_result = -1;
						}
					}
				}
				else
					result.n_adjust_result = -1;
			}
			//if (result.e_result == ce_last_truncated)
			//{
			//	result.n_extend = n_next_offset - mn_audio_end;						//?????§³
			//}
		}
		catch (...)
		{

		}
		return result;
	}


	bool  mp3_file::is_have_tag_encrypt_data()
	{
		int n =0;
		return read_tag_encrypt_data(NULL,n);
	}
	bool mp3_file::write_tag_encrypt_data(void *buf,int buflen)
	{
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v2);
		unsigned int n_rip_count = 0;
		if (sp_tag)
		{
			id3_v2_tag* p_tag_v2 = dynamic_cast<id3_v2_tag*>(sp_tag.get());
			tag_base_data_w *old_tag_data =p_tag_v2->get_base_tag_wstring();
			id3_v2_tag  new_tag;
			new_tag.import_wstring(old_tag_data);
			new_tag.attach_inner_frame(p_tag_v2,false,true);
			assert(p_tag_v2 != NULL);
			//??id3_v2 tag ????????????
			bool n_result = new_tag.add_encrypt_frame(buflen,(char*)buf);

			
			if(n_result)
			{

				long n_old_len=p_tag_v2->get_length();
				int n_offset = p_tag_v2->get_offset();

				assert(new_tag.get_offset() == 0);

				n_result = msp_file->cut_header(n_old_len);
				assert(n_result == 0);
				int n_new_len = new_tag.render(NULL);
				new_tag.set_padding_size(n_new_len);
				if (n_new_len <= 0 || n_new_len > mp3_tag_interface::MAX_TAG_ITEM_SIZE)
					return false;
				assert(n_new_len > 0);

				unsigned char* p_buf = new unsigned char[n_new_len];
				memset(p_buf,0,n_new_len);
				assert(p_buf);


				long n_tag_len = new_tag.render(p_buf);

				assert(n_tag_len == n_new_len);

				n_result = msp_file->attach_header(p_buf, n_new_len);

				assert(n_result == 0);
				delete[] p_buf;
				parse();
				return true;

			}
		}
		return false;
	}

	bool mp3_file::read_tag_encrypt_data(void *buf,int &buflen)
	{
		tag_ptr_type sp_tag = get_tag_by_serial(st_id3_v2);
		unsigned int n_rip_count = 0;
		if (sp_tag)
		{
			id3_v2_tag* p_tag_v2 = dynamic_cast<id3_v2_tag*>(sp_tag.get());
			assert(p_tag_v2 != NULL);
			return  p_tag_v2->get_encrypt_frame_data(buflen,(char *)buf);
		}
		return false;
	}

	bool  mp3_file::is_have_encrypt_data() 
	{
		return find_encrypt_data("MYXM") == 0 ? false :true ;
	}

	//???????§Õ????????,???????????????????????,??????????
	//??????????????????,??????????false,??????? true
	bool mp3_file::write_encrypt_data(void *buf,int buflen)
	{
		if (mb_read_only)
		{
			return false;
		}
		int endtag_start_pos=find_endtag_start_pos();

		char *tag_info =NULL;
		int   tag_info_size =  msp_file->get_size() -endtag_start_pos;
		//???????¦Â??tag,?????tag??????????????
		if(tag_info_size > 0)
		{
			int bytes_need = msp_file->get_size() - endtag_start_pos;
			if (bytes_need <= 0 || bytes_need > 1024 * 1024)
				return false;
			tag_info = new char[bytes_need];

			msp_file->seek(endtag_start_pos,FILE_BEGIN);

			unsigned int reads_now=0;

			while(reads_now < bytes_need)
			{
				unsigned int reads=0;
				msp_file->read(tag_info+reads_now,bytes_need -reads_now,&reads);

				reads_now+=reads;
			}

		}

		int encrypt_data_pos= find_encrypt_data("MYXM");
		if (encrypt_data_pos == 0)
		{
			encrypt_data_pos = endtag_start_pos;

		}
		else
		{
			//		cout<< "find_encrypt_data(MYXM) =="<<encrypt_data_pos<<	endl;
		}
		msp_file->seek(encrypt_data_pos,FILE_BEGIN);

		msp_file->set_EOF();

		unsigned int bytes_write=0;
		if(buf != NULL  && buflen > 0)//???buf==NULL ???????§³?????0,????????????????
		{
			unsigned int FrameHeader=0x4CB0FBFF;
			msp_file->write(&FrameHeader,4,&bytes_write);
			msp_file->write("MYXM",4,&bytes_write);

			if(buflen >618)
				buflen = 618;
			//§Õ???????
			//msp_file->write(&buflen,4,&bytes_write);
			long m[32]={1127029477u,2787430716u,1428749768u,2965828749u,274565019u,1178412241u,596746388u,1089869494u,2588017383u,4291593401u,3053843839u,1725947188u,4214236667u,1275985824u,1104304020u,2668483176u,336974020u,2291569464u,1386003983u,1116206178u,2974800363u,401112141u,834689850u,1521252788u,2281738211u,3562763741u,129043013u,758258210u,2094448516u,100847113u,1282080350u,753479536u};
			encrypt_mod::PK pk;
			for(int i=0;i<32;i++)
			{
				pk.m[i]=m[i];
			}
			char buffer[618];
			memset(buffer,0,618);
			memcpy(buffer,buf,buflen);
			encrypt_mod::public_key p_key;
			p_key.PK_to_vlong(pk);

			p_key.encrypt((DWORD*)buffer);

			//§Õ?????????
			msp_file->write(buffer,618,&bytes_write);

		}

		if(tag_info_size > 0)
		{
			msp_file->write(tag_info,tag_info_size,&bytes_write);
			delete [] tag_info;
		}	

		parse();
		mb_encrypt_data=true;
		return true;
	}

	bool mp3_file::read_encrypt_data(void *buf,int &buflen)
	{
		if(buflen <= 0)
			return false;

		int encrypt_data_pos= find_encrypt_data("MYXM");

		if (encrypt_data_pos == 0)//??§Þ???????
		{
			return false;
		}

		msp_file->seek(encrypt_data_pos,FILE_BEGIN);

		unsigned int bytes_read;
		char buffer[256];
		unsigned int FrameHeader=0;
		msp_file->read(&FrameHeader,4,&bytes_read);
		if(FrameHeader != 0x4CB0FBFF)
			return false;
		msp_file->seek(encrypt_data_pos+4,FILE_BEGIN);
		msp_file->read(buffer,4,&bytes_read);
		if(buffer[0] == 'M' && buffer[1] == 'Y' && buffer[2] == 'X' && buffer[3] == 'M')
		{
			int data_size=0;
			msp_file->seek(encrypt_data_pos+8,FILE_BEGIN);
			msp_file->read(&data_size,4,&bytes_read);
			msp_file->seek(encrypt_data_pos+12,FILE_BEGIN);
			if (data_size > buflen)
			{
				data_size = buflen;
			}
			msp_file->read(buf,data_size,&bytes_read);

			if(data_size != bytes_read)
				return false;
			
			char buffer[618];
			memset(buffer,0,618);
			msp_file->seek(encrypt_data_pos+8,FILE_BEGIN);
			msp_file->read(&buffer,618,&bytes_read);
			DWORD p[16]={1424447429u,1182577621u,1990031760u,2745724781u,2409869496u,2344528199u,2933738723u,1448035291u,
				1584419171u,1302693796u,1363591130u,3850618049u,2474083185u,3313075129u,2244470666u,1313306772u};
			DWORD q[16]={1635371169u,3667755719u,2364457553u,3482232042u,3244075909u,1575840861u,1304332192u,3686127224u,
				2123614928u,1504159856u,2106935109u,3812781421u,1974036336u,1454728075u,3098472009u,2464138641u};

			encrypt_mod::SK sk;
			for (int i=0;i<16;i++)
			{
				sk.p[i]=p[i];
				sk.q[i]=q[i];
			}
			encrypt_mod::private_key pk;
			pk.SK_to_vlong(sk);
			pk.decrypt((DWORD*) buffer);

			memcpy(buf,buffer,buflen);
			return true;
		}

		return false;

	}

	int mp3_file::find_endtag_start_pos() 
	{
		tag_ptr_type sp_id3v1 = get_tag_by_serial(st_id3_v1);
		tag_ptr_type sp_ape = get_tag_by_serial(st_ape);
		int n_offset = msp_file->get_size();

		if (sp_id3v1)			//??id3v1 tag
		{
			int n_v1_offset = sp_id3v1->get_offset();
			int n_v1_len = sp_id3v1->get_length();
			assert(n_v1_offset > 0 && n_v1_offset < n_offset);
			assert(n_v1_len == TAG_ID3_V1_LEN);
			n_offset = n_v1_offset;

		}
		if (sp_ape)				//??ape tag
		{
			int n_ape_offset = sp_ape->get_offset();
			int n_ape_len = sp_ape->get_length();
			assert(n_ape_offset > 0 && n_ape_len > 0);
			if (n_ape_offset < n_offset)						//ape tag??id3 v1 tag???
				n_offset = n_ape_offset;			
		}

		return n_offset;

	}
	//?????????????????§Ö?¦Ë??
	int mp3_file::find_encrypt_data(const char *tag_name) 
	{
		//??????????????TAG?????????¦Ë??
		if(strlen(tag_name) != 4)
			return 0;

		int n_offset = find_endtag_start_pos();


		//?????????(????????4),??tag?????????????1024?????
		int start_pos = n_offset -1026;
		if(start_pos < 0 )
			return 0;

		msp_file->seek(start_pos,FILE_BEGIN);
		char buf[1024];
		unsigned int read_bytes = 0;
		msp_file->read(buf,1024,&read_bytes);

		unsigned int FrameHeader=0;
		for(int i =0; i<read_bytes-4;i++)
		{
			memcpy(&FrameHeader,&buf[i],4);

			if(FrameHeader != 0x4CB0FBFF )
				continue;

			if( buf[i+4]	 == tag_name[0] &&
				buf[i+5] == tag_name[1] &&
				buf[i+6] == tag_name[2] &&
				buf[i+7] == tag_name[3] )
			{
				return start_pos+i; 
			}
		}

		return 0;


	}

	//////////////////////////////////////////////////////////////////////////
	tag_manager::tag_manager()
	{
	}

	tag_manager::~tag_manager()
	{
	}

	unsigned int tag_manager::rip_picture(const char* p_file_name, const char* p_dir)
	{
		unsigned int n_rip_count = 0;
		boost::shared_ptr<mp3_file> sp_mp3_file = tag_manager::load_file(p_file_name);
		if (sp_mp3_file)
		{
			n_rip_count = sp_mp3_file->rip_id3v2_pic(p_dir);
		}
		return n_rip_count;
	}

	void tag_manager::test_mp3_tag(const char* p_file_name)
	{
		boost::shared_ptr<mp3_file> sp_mp3_file = tag_manager::load_file(p_file_name);
		if (sp_mp3_file)
		{
			unsigned int n_tag_count = sp_mp3_file->get_tag_count();
			cout << "?????? " << n_tag_count << " ???????tag??" << endl;
			for (unsigned int i = 0 ; i < n_tag_count; ++i)
			{
				mp3_file::tag_ptr_type sp_tag = sp_mp3_file->get_tag(i);
				string s_tag_name = sp_tag->get_tag_name();
				cout << "?? " << i << " ??tag???????" << s_tag_name << endl;
				tag_base_data* p_base_info = sp_tag->get_base_tag();
				assert(p_base_info != NULL);
				cout << "?????" << p_base_info->s_artist << ", ?????" << p_base_info->s_album << "????????" << p_base_info->s_title << "??" << endl;
				cout << "?????????" << p_base_info->s_date << "??????" << p_base_info->n_track << "???????" << p_base_info->s_genre 
					<< "??????????" << p_base_info->n_genre_index << "??????????" << p_base_info->s_album_artist << "??" << endl;
				cout << "cd index : " << p_base_info->n_disk_serial << "??cd??????" << p_base_info->n_disk_count << ", gmt_publish : " << p_base_info->n_gmt_publish << "??" << endl;
				cout << "????????" << p_base_info->s_comment << "??" << endl;
				cout << "------------------------------------" << endl;
				delete p_base_info;
				p_base_info = NULL;
			}
			cout << "-------------???mpeg???--------------" << endl;
			boost::shared_ptr<mpeg_info> sp_fix_info = sp_mp3_file->get_mpeg_info();
			if (sp_fix_info)
			{
				cout << "?·Ú??" << const_enum_map::ma_mpeg_versions[sp_fix_info->mpa_version] 
				<< "??layer??" << const_enum_map::ma_mpeg_layers[sp_fix_info->mpa_layer] 
				<< "??????????" << const_enum_map::ma_channel_modes[sp_fix_info->channel_mode]
				<< "?????????" << sp_fix_info->bit_rate << "?????????" << sp_fix_info->sample_rate << "??" << endl;
			}
			else
				cout << "???mpeg???????" << endl;

			cout << "------------??????????????-----------------" << endl;
			check_result check_info = sp_mp3_file->check_content(false, false);
			cout << "???????" << const_enum_map::ma_check_results[check_info.e_result] << "?????????" << check_info.n_err_frame << "?????????" << check_info.n_frames
				<< "??¦Ä??????" << check_info.n_unpadding_frames << "????????" << check_info.n_padding_frames << endl;
			cout << "?????????" << check_info.e_bitrate_type << "?????????" << check_info.n_bitrate << "???????" << check_info.n_seconds_length << "??" << endl;
			cout << "------------?????????---------------------" << endl;

		}
		else
			cout << "???? " << p_file_name << " ????????????????????????????mp3?????" << endl;
		return;
	}


	void tag_manager::test_mp3_tag(const wchar_t* p_file_name)
	{
		boost::shared_ptr<mp3_file> sp_mp3_file = tag_manager::load_file(p_file_name);
		if (sp_mp3_file)
		{
			unsigned int n_tag_count = sp_mp3_file->get_tag_count();
			cout << "?????? " << n_tag_count << " ???????tag??" << endl;
			for (unsigned int i = 0 ; i < n_tag_count; ++i)
			{
				mp3_file::tag_ptr_type sp_tag = sp_mp3_file->get_tag(i);
				string s_tag_name = sp_tag->get_tag_name();
				cout << "?? " << i << " ??tag???????" << s_tag_name << endl;
				tag_base_data* p_base_info = sp_tag->get_base_tag();
				assert(p_base_info != NULL);
				cout << "?????" << p_base_info->s_artist << ", ?????" << p_base_info->s_album << "????????" << p_base_info->s_title << "??" << endl;
				cout << "?????????" << p_base_info->s_date << "??????" << p_base_info->n_track << "???????" << p_base_info->s_genre 
					<< "??????????" << p_base_info->n_genre_index << "??????????" << p_base_info->s_album_artist << "??" << endl;
				cout << "cd index : " << p_base_info->n_disk_serial << "??cd??????" << p_base_info->n_disk_count << ", gmt_publish : " << p_base_info->n_gmt_publish << "??" << endl;
				cout << "????????" << p_base_info->s_comment << "??" << endl;
				cout << "------------------------------------" << endl;
				delete p_base_info;
				p_base_info = NULL;
			}
			cout << "-------------???mpeg???--------------" << endl;
			boost::shared_ptr<mpeg_info> sp_fix_info = sp_mp3_file->get_mpeg_info();
			if (sp_fix_info)
			{
				cout << "?·Ú??" << const_enum_map::ma_mpeg_versions[sp_fix_info->mpa_version] 
				<< "??layer??" << const_enum_map::ma_mpeg_layers[sp_fix_info->mpa_layer] 
				<< "??????????" << const_enum_map::ma_channel_modes[sp_fix_info->channel_mode]
				<< "?????????" << sp_fix_info->bit_rate << "?????????" << sp_fix_info->sample_rate << "??" << endl;
			}
			else
				cout << "???mpeg???????" << endl;

			cout << "------------??????????????-----------------" << endl;
			check_result check_info = sp_mp3_file->check_content(false, false);
			cout << "???????" << const_enum_map::ma_check_results[check_info.e_result] << "?????????" << check_info.n_err_frame << "?????????" << check_info.n_frames
				<< "??¦Ä??????" << check_info.n_unpadding_frames << "????????" << check_info.n_padding_frames << endl;
			cout << "?????????" << check_info.e_bitrate_type << "?????????" << check_info.n_bitrate << "???????" << check_info.n_seconds_length << "??" << endl;
			cout << "------------?????????---------------------" << endl;

		}
		else
			cout << "???? " << p_file_name << " ????????????????????????????mp3?????" << endl;
		return;
	}

	bool tag_manager::attach_id3v1_tag(const char* p_file_name, const tag_base_data* p_data)
	{
		bool b_result(false);
		assert(p_file_name != NULL && p_data != NULL);

		mp3_file* p_mp3_file = new mp3_file();
		b_result = p_mp3_file->load(p_file_name, true, false);
		if (b_result)
		{
			b_result = p_mp3_file->write_id3v1_tag(p_data);
		}
		delete p_mp3_file;
		p_mp3_file = NULL;
		return b_result;
	}
	bool tag_manager::attach_id3v1_tag_wstring(const wchar_t* p_file_name, const tag_base_data_w* p_data)
	{
		bool b_result(false);
		assert(p_file_name != NULL && p_data != NULL);

		mp3_file* p_mp3_file = new mp3_file();
		b_result = p_mp3_file->load(p_file_name, true, false);
		if (b_result)
		{
			b_result = p_mp3_file->write_id3v1_tag_wstring(p_data);
		}
		delete p_mp3_file;
		p_mp3_file = NULL;
		return b_result;
	}

	bool tag_manager::attach_id3v2_tag(const char* p_file_name, const tag_base_data* p_data)
	{
		bool b_result(false);
		assert(p_file_name != NULL && p_data != NULL);

		mp3_file* p_mp3_file = new mp3_file();
		b_result = p_mp3_file->load(p_file_name, true, false);
		if (b_result)
		{
			b_result = p_mp3_file->write_id3v2_tag(p_data);
		}
		delete p_mp3_file;
		p_mp3_file = NULL;
		return b_result;
	}

	bool tag_manager::attach_id3v2_tag_wstring(const wchar_t* p_file_name, const tag_base_data_w* p_data)
	{
		bool b_result(false);
		assert(p_file_name != NULL && p_data != NULL);

		mp3_file* p_mp3_file = new mp3_file();
		b_result = p_mp3_file->load(p_file_name, true, false);
		if (b_result)
		{
			b_result = p_mp3_file->write_id3v2_tag_wstring(p_data);
		}
		delete p_mp3_file;
		p_mp3_file = NULL;
		return b_result;
	}

	boost::shared_ptr<mp3_file> tag_manager::load_file(const char* p_file_name, unsigned int n_check_frames, bool b_read_only)
	{
		bool b_result(false);
		assert(p_file_name != NULL);

		mp3_file* p_mp3_file = new mp3_file(n_check_frames);
		b_result = p_mp3_file->load(p_file_name, true, b_read_only);
		if (!b_result)
		{
			delete p_mp3_file;
			p_mp3_file = NULL;
		}
		return boost::shared_ptr<mp3_file>(p_mp3_file);
	}
	boost::shared_ptr<mp3_file> tag_manager::load_file(const wchar_t* p_file_name, unsigned int n_check_frames, bool b_read_only)
	{
		bool b_result(false);
		assert(p_file_name != NULL);

		mp3_file* p_mp3_file = new mp3_file(n_check_frames);
		b_result = p_mp3_file->load(p_file_name, true, b_read_only);
		if (!b_result)
		{
			delete p_mp3_file;
			p_mp3_file = NULL;
		}
		return boost::shared_ptr<mp3_file>(p_mp3_file);
	}

}	//end namespace em_mp3_tag
