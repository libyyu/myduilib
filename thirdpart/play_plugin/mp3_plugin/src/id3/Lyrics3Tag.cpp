#include "lyrics3tag.h"
#include <cassert>
#ifdef _WIN32
#include <atlstr.h>
#endif
#include <boost/lexical_cast.hpp>
#include "IO.h"

using namespace std;

namespace em_mp3_tag
{
lyrics3_tag::lyrics3_tag()
{
	mn_begin = 0;
	mn_length = 0;
	mf_version = 0.00f;
}

lyrics3_tag::~lyrics3_tag()
{
}

bool lyrics3_tag::init(io_base* p_file, unsigned int n_begin, unsigned int n_footer_begin, bool b_ver_2)
{
	assert(p_file != NULL);
	bool b_result(false);
	int n_result = 0;
	unsigned int n_bytes_read = 0;
	unsigned int n_pos = n_footer_begin;
	if (b_ver_2)
	{
		mf_version = 2.00f;

		// look for size of tag (stands before dwOffset)
		n_pos -= V2_SIZE_BUF;
		if (n_pos <= n_begin)
			return false;
		n_result = p_file->seek(n_pos, FILE_BEGIN);
		if (n_result != 0)
			return false;
		unsigned char s_size_buf[V2_SIZE_BUF + 1];
		n_result = p_file->read(s_size_buf, V2_SIZE_BUF, &n_bytes_read);
		if (n_result != 0 || n_bytes_read != V2_SIZE_BUF)
			return false;
		s_size_buf[V2_SIZE_BUF] = '\0';
		try
		{
			mn_length = boost::lexical_cast<unsigned int>(s_size_buf);
			mn_length += V2_SIZE_BUF;
			mn_begin = n_footer_begin - mn_length;
			if (mn_begin <= n_begin)
				return false;
			mn_length += FOOTER_SIZE;					//加上FOOTER的长度
			//todo : copy data to buf
			b_result = true;
		}
		catch (...)
		{
		}
	}
	else
	{
		mf_version = 1.00f;

		// seek back 5100 bytes and look for LYRICSBEGIN
		mn_begin= n_footer_begin - V1_BEFORE_FOOTER;
		if (mn_begin <= n_begin)
			return false;
		n_result = p_file->seek(mn_begin, FILE_BEGIN);
		if (n_result != 0)
			return false;
		unsigned char s_header_buf[V1_BEFORE_FOOTER];
		n_result = p_file->read(s_header_buf, V1_BEFORE_FOOTER, &n_bytes_read);
		if (n_result != 0 || n_bytes_read != V1_BEFORE_FOOTER)
			return false;
		unsigned char* p_pos = s_header_buf;
		b_result = true;
		while (memcmp("LYRICSBEGIN", p_pos, V1_HEADER_SIZE) != 0)
		{
			++p_pos;
			++mn_begin;
			if (mn_begin + V1_HEADER_SIZE > n_footer_begin)
			{
				b_result = false;
				break;
			}
		}
		if (b_result)				//找到tag header
		{
			mn_length = n_footer_begin - mn_begin + FOOTER_SIZE;
			//todo : copy data to buf
		}
		else
		{
		}
	}
	return b_result;
}

int lyrics3_tag::get_length() const
{
	return mn_length;
}

int lyrics3_tag::get_offset() const
{
	return mn_begin;
}

bool lyrics3_tag::is_append() const
{
	return true;
}

tag_base_data* lyrics3_tag::get_base_tag()
{
	return NULL;
}
tag_base_data_w* lyrics3_tag::get_base_tag_wstring()
{
	return NULL;
}
string lyrics3_tag::get_tag_name()
{
	return const_enum_map::ma_tags[st_lyric3];
}

wstring lyrics3_tag::get_tag_name_wstring()
{
	string str =const_enum_map::ma_tags[st_lyric3];
	return wstring(CA2W(str.c_str()).m_psz);
}

unsigned int lyrics3_tag::get_tag_serial()
{
	return st_lyric3;
}

//////////////////////////////////////////////////////////////////////////
lyrics_tag_factory::lyrics_tag_factory()
{
}

lyrics_tag_factory::~lyrics_tag_factory()
{
}

mp3_tag_interface* lyrics_tag_factory::create_tag(io_base *p_file, unsigned int n_begin, unsigned int n_end)
{
	int n_result = 0;
	assert(p_file != NULL);
	if (n_end - n_begin < lyrics3_tag::FOOTER_SIZE)
		return NULL;
	n_result = p_file->seek(n_end - lyrics3_tag::FOOTER_SIZE, FILE_BEGIN);
	if (n_result != 0)
		return NULL;

	unsigned char s_min_buf[lyrics3_tag::FOOTER_SIZE];
	unsigned int n_bytes_read = 0;
	n_result = p_file->read(s_min_buf, lyrics3_tag::FOOTER_SIZE, &n_bytes_read);
	if (n_result != 0 || n_bytes_read != lyrics3_tag::FOOTER_SIZE)
		return NULL;

	bool b_result(false);
	lyrics3_tag* p_tag = NULL;
	// is it Lyrics 2 Tag
	if (memcmp("LYRICS200", s_min_buf, lyrics3_tag::FOOTER_SIZE) == 0)
	{
		p_tag = new lyrics3_tag();
		b_result = p_tag->init(p_file, n_begin, n_end - lyrics3_tag::FOOTER_SIZE, true);
		if (!b_result)
		{
			delete p_tag;
			p_tag = NULL;
		}
	}
	else if (memcmp("LYRICSEND", s_min_buf, lyrics3_tag::FOOTER_SIZE) == 0)
	{
		p_tag = new lyrics3_tag();
		b_result = p_tag->init(p_file, n_begin, n_end - lyrics3_tag::FOOTER_SIZE, false);
		if (!b_result)
		{
			delete p_tag;
			p_tag = NULL;
		}
	}
	return p_tag;	
}

}		//end namespace em_mp3_tag
