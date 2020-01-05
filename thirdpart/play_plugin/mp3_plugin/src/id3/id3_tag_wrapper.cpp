#include "id3_tag_wrapper.h"
#include "All.h"
#include "IO.h"
#include <cassert>
#include <iostream>
#include <atlbase.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "misc.h"
#include <boost/numeric/conversion/cast.hpp>
#include "CharacterHelper.h"
#include "FileIO.h"
#include "encrypt.h"
using namespace std;

namespace em_mp3_tag
{

//////////////////////////////////////////////////////////////////////////

bool id3_tag_base::parse_base_data(id3_tag* p_input, bool b_id3_v1, tag_base_data* p_output)
{
	assert(p_input != NULL);
	assert(p_output != NULL);
	id3_frame* p_frame = NULL;
	string s_value;
	p_frame = id3_tag_findframe(p_input, ID3_FRAME_TITLE, 0);
	if (p_frame != NULL)
		p_output->s_title = get_field_string(p_frame);

	p_frame = id3_tag_findframe(p_input, ID3_FRAME_ARTIST, 0);
	if (p_frame != NULL)
		p_output->s_artist = get_field_string(p_frame);

	p_frame = id3_tag_findframe(p_input, ID3_FRAME_ALBUM, 0);
	if (p_frame != NULL)
		p_output->s_album = get_field_string(p_frame);

	p_frame = id3_tag_findframe(p_input, ID3V2_FRAME_COMMENT, 0);
	if (p_frame != NULL)
		p_output->s_comment = get_field_string(p_frame);
	
	p_frame = id3_tag_findframe(p_input, ID3_FRAME_TRACK, 0);
	if (p_frame != NULL)
	{
		s_value = get_field_string(p_frame);
		
		vector<string> items = misc::string_helper::split_string(s_value, "/");
		for (misc::string_helper::split_iter i = items.begin(); i != items.end(); ++i)
		{
			s_value = *i;
			//boost::algorithm::trim(s_value);
			if (i - items.begin() == 0)
			{
				try
				{
					p_output->n_track = boost::lexical_cast<unsigned short>(s_value);
				}
				catch (boost::bad_lexical_cast&)
				{
					p_output->n_track = 0;	
				}
			}
			else if (i - items.begin() == 1)
			{
				try
				{
					p_output->n_song_count = boost::lexical_cast<unsigned short>(s_value);
				}
				catch (boost::bad_lexical_cast&)
				{
					p_output->n_song_count = 0;
				}
			}
		}
	}

	p_frame = id3_tag_findframe(p_input, ID3_FRAME_YEAR, 0);
	if (p_frame != NULL)
	{
		p_output->s_date = get_field_string(p_frame);
		p_output->generate_gmt_publish();
	}

	p_frame = id3_tag_findframe(p_input, ID3_FRAME_GENRE, 0);
	if (p_frame != NULL)
	{
		pair<int, string> genre_result = get_genre_field(p_frame, b_id3_v1);
		p_output->n_genre_index = genre_result.first;
		p_output->s_genre = genre_result.second;
	}

	if (!b_id3_v1)			//id3 v2
	{
		p_frame = id3_tag_findframe(p_input, ID3V2_FRAME_ORCHESTRA, 0);
		if (p_frame != NULL)
			p_output->s_album_artist = get_field_string(p_frame);

		p_frame = id3_tag_findframe(p_input, ID3V2_FRAME_SET_PART, 0);
		if (p_frame != NULL)
		{
			s_value = get_field_string(p_frame);

			vector<string> items = misc::string_helper::split_string(s_value, "/");
			for (misc::string_helper::split_iter i = items.begin(); i != items.end(); ++i)
			{
				s_value = *i;
				//boost::algorithm::trim(s_value);
				if (i - items.begin() == 0)
				{
					try
					{
						p_output->n_disk_serial = boost::lexical_cast<unsigned int>(s_value);
					}
					catch (boost::bad_lexical_cast&)
					{
						p_output->n_disk_serial = 0;	
					}
				}
				else if (i - items.begin() == 1)
				{
					try
					{
						p_output->n_disk_count = boost::lexical_cast<unsigned int>(s_value);
					}
					catch (boost::bad_lexical_cast&)
					{
						p_output->n_disk_count = 0;
					}
				}
			}
		}
		
	}
	return true;
}
bool id3_tag_base::parse_base_data_wstring(id3_tag* p_input, bool b_id3_v1, tag_base_data_w* p_output)
{
	assert(p_input != NULL);
	assert(p_output != NULL);
	id3_frame* p_frame = NULL;
	string s_value;
	p_frame = id3_tag_findframe(p_input, ID3_FRAME_TITLE, 0);
	if (p_frame != NULL)
		p_output->s_title = get_field_wstring(p_frame);

	p_frame = id3_tag_findframe(p_input, ID3_FRAME_ARTIST, 0);
	if (p_frame != NULL)
		p_output->s_artist = get_field_wstring(p_frame);

	p_frame = id3_tag_findframe(p_input, ID3_FRAME_ALBUM, 0);
	if (p_frame != NULL)
		p_output->s_album = get_field_wstring(p_frame);

	p_frame = id3_tag_findframe(p_input, ID3V2_FRAME_COMMENT, 0);
	if (p_frame != NULL)
		p_output->s_comment = get_field_wstring(p_frame);

	p_frame = id3_tag_findframe(p_input, ID3_FRAME_TRACK, 0);
	if (p_frame != NULL)
	{
		s_value = get_field_string(p_frame);

		vector<string> items = misc::string_helper::split_string(s_value, "/");
		for (misc::string_helper::split_iter i = items.begin(); i != items.end(); ++i)
		{
			s_value = *i;
			boost::algorithm::trim(s_value);
			if (i - items.begin() == 0)
			{
				try
				{
					p_output->n_track = boost::lexical_cast<unsigned short>(s_value);
				}
				catch (boost::bad_lexical_cast&)
				{
					p_output->n_track = 0;	
				}
			}
			else if (i - items.begin() == 1)
			{
				try
				{
					p_output->n_song_count = boost::lexical_cast<unsigned int>(s_value);
				}
				catch (boost::bad_lexical_cast&)
				{
					p_output->n_song_count = 0;
				}
			}
		}
	}

	p_frame = id3_tag_findframe(p_input, ID3_FRAME_YEAR, 0);
	if (p_frame != NULL)
	{
		p_output->s_date = get_field_wstring(p_frame);
		p_output->generate_gmt_publish();
	}

	p_frame = id3_tag_findframe(p_input, ID3_FRAME_GENRE, 0);
	if (p_frame != NULL)
	{
		pair<int, wstring> genre_result = get_genre_field_wstring(p_frame, b_id3_v1);
		p_output->n_genre_index = genre_result.first;
		p_output->s_genre = genre_result.second;
	}

	if (!b_id3_v1)			//id3 v2
	{
		p_frame = id3_tag_findframe(p_input, ID3V2_FRAME_ORCHESTRA, 0);
		if (p_frame != NULL)
			p_output->s_album_artist = get_field_wstring(p_frame);

		p_frame = id3_tag_findframe(p_input, ID3V2_FRAME_SET_PART, 0);
		if (p_frame != NULL)
		{
			s_value = get_field_string(p_frame);

			vector<string> items = misc::string_helper::split_string(s_value, "/");
			for (misc::string_helper::split_iter i = items.begin(); i != items.end(); ++i)
			{
				s_value = *i;
				boost::algorithm::trim(s_value);
				if (i - items.begin() == 0)
				{
					try
					{
						p_output->n_disk_serial = boost::lexical_cast<unsigned int>(s_value);
					}
					catch (boost::bad_lexical_cast&)
					{
						p_output->n_disk_serial = 0;	
					}
				}
				else if (i - items.begin() == 1)
				{
					try
					{
						p_output->n_disk_count = boost::lexical_cast<unsigned int>(s_value);
					}
					catch (boost::bad_lexical_cast&)
					{
						p_output->n_disk_count = 0;
					}
				}
			}
		}

	}
	return true;
}

bool id3_tag_base::add_comment_frame(id3_tag* p_tag, const char* p_frame_name, const std::string& s_value, bool b_id3_v1)
{
	assert(p_tag != NULL);
	assert(p_frame_name != NULL);
	id3_frame* p_frame = id3_frame_new(p_frame_name);
	if (p_frame == NULL)
		return false;

	int n_result = 0;
	id3_field* p_field = p_frame->fields;
	for (size_t i = 0; i < p_frame->nfields; ++i, ++p_field)
	{
		assert(p_field != NULL);
		switch (p_field->type)
		{
		case ID3_FIELD_TYPE_TEXTENCODING :
			if (b_id3_v1)
				n_result = id3_field_settextencoding(p_field, ID3_FIELD_TEXTENCODING_ISO_8859_1);
			else
				n_result = id3_field_settextencoding(p_field, ID3_FIELD_TEXTENCODING_UTF_16);
			assert(n_result == 0);
			break;
		case ID3_FIELD_TYPE_LANGUAGE :
			{
				n_result = id3_field_setlanguage(p_field, "XXX");
				assert(n_result == 0);
			}
			break;
		case ID3_FIELD_TYPE_STRING :
			{
				id3_latin1_t* p_latin = (id3_latin1_t*)"";
				if (b_id3_v1)
				{
					id3_ucs4_t* p_ucs = id3_latin1_ucs4duplicate(p_latin);
					assert(p_ucs != NULL);
					n_result = id3_field_setstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
				}
				else
				{
					str_utf16* p_value_w = GetUTF16FromANSI("");
					assert(p_value_w != NULL);

					ATL::CA2W value_w2(s_value.c_str());
					str_utf16* p_value_w2 =LPWSTR(value_w2);

					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)p_value_w);
					assert(p_ucs != NULL);
					n_result = id3_field_setstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
					delete[] p_value_w;
					p_value_w = NULL;
				}
			}
			break;
		case ID3_FIELD_TYPE_STRINGFULL : 
			{
				id3_latin1_t* p_latin = (id3_latin1_t*)(s_value.c_str());
				if (b_id3_v1)
				{
					id3_ucs4_t* p_ucs = id3_latin1_ucs4duplicate(p_latin);
					assert(p_ucs != NULL);
					n_result = id3_field_setfullstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
				}
				else
				{
					str_utf16* p_value_w = GetUTF16FromANSI(s_value.c_str());
					assert(p_value_w != NULL);

					ATL::CA2W value_w2(s_value.c_str());
					str_utf16* p_value_w2 =LPWSTR(value_w2);

					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)p_value_w);
					assert(p_ucs != NULL);
					n_result = id3_field_setfullstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
					delete[] p_value_w;
					p_value_w = NULL;
				}
			}
			break;
		default :
			assert(false);
			break;
		}
	}

	n_result = id3_tag_attachframe(p_tag, p_frame);
	assert(n_result == 0);
	return true;
}
bool id3_tag_base::add_comment_frame_wstring(id3_tag* p_tag, const char* p_frame_name, const std::wstring& s_value, bool b_id3_v1)
{
	
	assert(p_tag != NULL);
	assert(p_frame_name != NULL);
	id3_frame* p_frame = id3_frame_new(p_frame_name);
	if (p_frame == NULL)
		return false;

	int n_result = 0;
	id3_field* p_field = p_frame->fields;
	for (size_t i = 0; i < p_frame->nfields; ++i, ++p_field)
	{
		assert(p_field != NULL);
		switch (p_field->type)
		{
		case ID3_FIELD_TYPE_TEXTENCODING :
			if (b_id3_v1)
				n_result = id3_field_settextencoding(p_field, ID3_FIELD_TEXTENCODING_ISO_8859_1);
			else
				n_result = id3_field_settextencoding(p_field, ID3_FIELD_TEXTENCODING_UTF_16);
			assert(n_result == 0);
			break;
		case ID3_FIELD_TYPE_LANGUAGE :
			{
				n_result = id3_field_setlanguage(p_field, "XXX");
				assert(n_result == 0);
			}
			break;
		case ID3_FIELD_TYPE_STRING :
			{
				id3_latin1_t* p_latin = (id3_latin1_t*)"";
				if (b_id3_v1)
				{
					id3_ucs4_t* p_ucs = id3_latin1_ucs4duplicate(p_latin);
					assert(p_ucs != NULL);
					n_result = id3_field_setstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
				}
				else
				{
					/*str_utf16* p_value_w = GetUTF16FromANSI("");
					assert(p_value_w != NULL);

					ATL::CA2W value_w2(s_value.c_str());
					str_utf16* p_value_w2 =LPWSTR(value_w2);
					*/
					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)(s_value.c_str()));
					assert(p_ucs != NULL);
					n_result = id3_field_setstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
					//delete[] p_value_w;
					//p_value_w = NULL;
				}
			}
			break;
		case ID3_FIELD_TYPE_STRINGFULL : 
			{
				//id3_latin1_t* p_latin = (id3_latin1_t*)(s_value.c_str());
				if (b_id3_v1)
				{
					id3_ucs4_t* p_ucs =id3_utf16_ucs4duplicate((id3_utf16_t*)(s_value.c_str()));
					assert(p_ucs != NULL);
					n_result = id3_field_setfullstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
				}
				else
				{
					/*str_utf16* p_value_w =(str_utf16*)s_value.c_str();
					assert(p_value_w != NULL);

					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)p_value_w);
					assert(p_ucs != NULL);
					n_result = id3_field_setfullstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
					delete[] p_value_w;
					p_value_w = NULL;
					*/
					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)(s_value.c_str()));
					assert(p_ucs != NULL);
					n_result = id3_field_setfullstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
				}
			}
			break;
		default :
			assert(false);
			break;
		}
	}

	n_result = id3_tag_attachframe(p_tag, p_frame);
	assert(n_result == 0);
	return true;
}
bool id3_tag_base::add_frame_string(id3_tag* p_tag, const char* p_frame_name, const std::string& s_value, bool b_id3_v1)
{
	assert(p_tag != NULL);
	assert(p_frame_name != NULL);
	id3_frame* p_frame = id3_frame_new(p_frame_name);
	if (p_frame == NULL)
		return false;
	bool b_result = set_field_string(p_frame, s_value, b_id3_v1);
	assert(b_result);
	int n_result = id3_tag_attachframe(p_tag, p_frame);
	assert(n_result == 0);
	return true;
}
bool id3_tag_base::add_frame_wstring(id3_tag* p_tag, const char* p_frame_name, const std::wstring& s_value, bool b_id3_v1)
{
	assert(p_tag != NULL);
	assert(p_frame_name != NULL);
	id3_frame* p_frame = id3_frame_new(p_frame_name);
	if (p_frame == NULL)
		return false;
	bool b_result = set_field_wstring(p_frame, s_value, b_id3_v1);
	assert(b_result);
	int n_result = id3_tag_attachframe(p_tag, p_frame);
	assert(n_result == 0);
	return true;
}

bool id3_tag_base::set_field_string(struct id3_frame* p_frame, const std::string& s_value, bool b_id3_v1)
{
	int n_result = 0;
	assert(p_frame != NULL);
	id3_field* p_field = p_frame->fields;
	for (size_t i = 0; i < p_frame->nfields; ++i, ++p_field)
	{
		assert(p_field != NULL);
		switch (p_field->type)
		{
		case ID3_FIELD_TYPE_TEXTENCODING :
			if (b_id3_v1)
				n_result = id3_field_settextencoding(p_field, ID3_FIELD_TEXTENCODING_ISO_8859_1);
			else
				n_result = id3_field_settextencoding(p_field, ID3_FIELD_TEXTENCODING_UTF_16);
			assert(n_result == 0);
			break;
		case ID3_FIELD_TYPE_STRINGLIST :
			{
				id3_latin1_t* p_latin = (id3_latin1_t*)(s_value.c_str());
				if (b_id3_v1)
				{
					id3_ucs4_t* p_ucs = id3_latin1_ucs4duplicate(p_latin);
					assert(p_ucs != NULL);
					n_result = id3_field_setstrings(p_field, 1, &p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
				}
				else
				{
					str_utf16* p_value_w = GetUTF16FromANSI(s_value.c_str());
					assert(p_value_w != NULL);
					
					ATL::CA2W value_w2(s_value.c_str());
					str_utf16* p_value_w2 =LPWSTR(value_w2);

					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)p_value_w);
					assert(p_ucs != NULL);
					n_result = id3_field_setstrings(p_field, 1, &p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
					delete[] p_value_w;
					p_value_w = NULL;
				}
			}
			break;
		case ID3_FIELD_TYPE_LANGUAGE :
			{
				n_result = id3_field_setlanguage(p_field, "XXX");
			}
			break;
		case ID3_FIELD_TYPE_STRING :
			{
				id3_latin1_t* p_latin = (id3_latin1_t*)(s_value.c_str());
				if (b_id3_v1)
				{
					id3_ucs4_t* p_ucs = id3_latin1_ucs4duplicate(p_latin);
					assert(p_ucs != NULL);
					n_result = id3_field_setstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
				}
				else
				{
					str_utf16* p_value_w = GetUTF16FromANSI(s_value.c_str());
					assert(p_value_w != NULL);

					ATL::CA2W value_w2(s_value.c_str());
					str_utf16* p_value_w2 =LPWSTR(value_w2);

					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)p_value_w);
					assert(p_ucs != NULL);
					n_result = id3_field_setstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
					delete[] p_value_w;
					p_value_w = NULL;
				}
			}
			break;
		case ID3_FIELD_TYPE_STRINGFULL : 
			{
				id3_latin1_t* p_latin = (id3_latin1_t*)(s_value.c_str());
				if (b_id3_v1)
				{
					id3_ucs4_t* p_ucs = id3_latin1_ucs4duplicate(p_latin);
					assert(p_ucs != NULL);
					n_result = id3_field_setfullstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
				}
				else
				{
					str_utf16* p_value_w = GetUTF16FromANSI(s_value.c_str());
					assert(p_value_w != NULL);

					ATL::CA2W value_w2(s_value.c_str());
					str_utf16* p_value_w2 =LPWSTR(value_w2);

					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)p_value_w);
					assert(p_ucs != NULL);
					n_result = id3_field_setfullstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
					delete[] p_value_w;
					p_value_w = NULL;
				}
			}
			break;
		default :
			assert(false);
			break;
		}
	}

	string s_temp = get_field_string(p_frame);

	return true;
}
bool id3_tag_base::set_field_wstring(struct id3_frame* p_frame, const std::wstring& s_value, bool b_id3_v1)
{
	int n_result = 0;
	assert(p_frame != NULL);
	id3_field* p_field = p_frame->fields;
	for (size_t i = 0; i < p_frame->nfields; ++i, ++p_field)
	{
		assert(p_field != NULL);
		switch (p_field->type)
		{
		case ID3_FIELD_TYPE_TEXTENCODING :
			if (b_id3_v1)
				n_result = id3_field_settextencoding(p_field, ID3_FIELD_TEXTENCODING_ISO_8859_1);
			else
				n_result = id3_field_settextencoding(p_field, ID3_FIELD_TEXTENCODING_UTF_16);
			assert(n_result == 0);
			break;
		case ID3_FIELD_TYPE_STRINGLIST :
			{
				id3_utf16_t* p_utf16 = (id3_utf16_t*)(s_value.c_str());
				if (b_id3_v1)
				{
					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate(p_utf16);
					assert(p_ucs != NULL);
					n_result = id3_field_setstrings(p_field, 1, &p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
				}
				else
				{
					/*str_utf16* p_value_w = GetUTF16FromANSI(s_value.c_str());
					assert(p_value_w != NULL);

					ATL::CA2W value_w2(s_value.c_str());
					str_utf16* p_value_w2 =LPWSTR(value_w2);
					*/
					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)s_value.c_str());
					assert(p_ucs != NULL);
					n_result = id3_field_setstrings(p_field, 1, &p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
					//delete[] p_value_w;
					//p_value_w = NULL;
				}
			}
			break;
		case ID3_FIELD_TYPE_LANGUAGE :
			{
				n_result = id3_field_setlanguage(p_field, "XXX");
			}
			break;
		case ID3_FIELD_TYPE_STRING :
			{
				id3_utf16_t* p_utf16 = (id3_utf16_t*)(s_value.c_str());
				if (b_id3_v1)
				{
					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate(p_utf16);
					assert(p_ucs != NULL);
					n_result = id3_field_setstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
				}
				else
				{
					/*str_utf16* p_value_w = GetUTF16FromANSI(s_value.c_str());
					assert(p_value_w != NULL);

					ATL::CA2W value_w2(s_value.c_str());
					str_utf16* p_value_w2 =LPWSTR(value_w2);
					*/
					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)s_value.c_str());
					assert(p_ucs != NULL);
					n_result = id3_field_setstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
					//delete[] p_value_w;
					//p_value_w = NULL;
				}
			}
			break;
		case ID3_FIELD_TYPE_STRINGFULL : 
			{
				id3_utf16_t* p_utf16 = (id3_utf16_t*)(s_value.c_str());
				if (b_id3_v1)
				{
					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate(p_utf16);
					assert(p_ucs != NULL);
					n_result = id3_field_setfullstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
				}
				else
				{
					/*str_utf16* p_value_w = GetUTF16FromANSI(s_value.c_str());
					assert(p_value_w != NULL);

					ATL::CA2W value_w2(s_value.c_str());
					str_utf16* p_value_w2 =LPWSTR(value_w2);
					*/
					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)s_value.c_str());
					assert(p_ucs != NULL);
					n_result = id3_field_setfullstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
					//delete[] p_value_w;
					//p_value_w = NULL;
				}
			}
			break;
		default :
			assert(false);
			break;
		}
	}

	wstring s_temp = get_field_wstring(p_frame);

	return true;
}
string id3_tag_base::get_field_string(struct id3_frame* p_frame)
{
	assert(p_frame != NULL);
	//assert(p_frame->nfields == 2);
	id3_field_textencoding encoding_type = ID3_FIELD_TEXTENCODING_ISO_8859_1;
	bool b_get_encoding(false);
	id3_field* p_field = NULL;
	p_field = p_frame->fields;
	string s_value;
	for (size_t i = 0; i < p_frame->nfields; ++i)
	{
		assert(p_field != NULL);
		switch (p_field->type)
		{
		case ID3_FIELD_TYPE_TEXTENCODING : 
			encoding_type = id3_field_gettextencoding(p_field);
			b_get_encoding = true;
			break;
		case ID3_FIELD_TYPE_LATIN1 :
			{
				id3_latin1_t const * p_text = id3_field_getlatin1(p_field);
				assert(p_text != NULL);
				s_value = (char*)p_text;
			}
			break;
		case ID3_FIELD_TYPE_LATIN1FULL :
			{
				id3_latin1_t const* p_text = id3_field_getfulllatin1(p_field);
				assert(p_text != NULL);
				s_value = (char*)p_text;
			}
			break;
		case ID3_FIELD_TYPE_LATIN1LIST :
			{
				assert(p_field->latin1list.nstrings == 1);
				id3_latin1_t* p_text = p_field->latin1list.strings[0];
				assert(p_text != NULL);
				s_value = (char*)p_text;
			}
			break;
		case ID3_FIELD_TYPE_STRING :
			{
				id3_ucs4_t const* p_text_u = id3_field_getstring(p_field);
				assert(p_text_u != NULL);
				assert(b_get_encoding);
				if (encoding_type == ID3_FIELD_TEXTENCODING_ISO_8859_1)
				{
					id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
					assert(p_text != NULL);
					if (s_value.empty())
						s_value = (char*)p_text;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_16 || encoding_type == ID3_FIELD_TEXTENCODING_UTF_16BE)
				{
					id3_utf16_t* p_text = id3_ucs4_utf16duplicate(p_text_u);
					assert(p_text != NULL);
					ATL::CW2A ansi_text((LPCWSTR)p_text);
					if (s_value.empty())
						s_value = ansi_text;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_8)
				{
					id3_utf8_t* p_text = id3_ucs4_utf8duplicate(p_text_u);
					assert(p_text != NULL);
					str_ansi* p_ansi_text = GetANSIFromUTF8((str_utf8*)p_text);
					if (s_value.empty())
						s_value = p_ansi_text;
					free(p_text);
					delete[] p_ansi_text;
				}
				else
					assert(false);
			}
			break;
		case ID3_FIELD_TYPE_STRINGFULL : 
			{
				id3_ucs4_t const* p_text_u = id3_field_getfullstring(p_field);
				assert(p_text_u != NULL);
				assert(b_get_encoding);

				if (encoding_type == ID3_FIELD_TEXTENCODING_ISO_8859_1)
				{
					id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
					assert(p_text != NULL);
					if (s_value.empty())
						s_value = (char*)p_text;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_16 || encoding_type == ID3_FIELD_TEXTENCODING_UTF_16BE)
				{
					id3_utf16_t* p_text = id3_ucs4_utf16duplicate(p_text_u);
					assert(p_text != NULL);
					ATL::CW2A ansi_text((LPCWSTR)p_text);
					if (s_value.empty())
						s_value = ansi_text;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_8)
				{
					id3_utf8_t* p_text = id3_ucs4_utf8duplicate(p_text_u);
					assert(p_text != NULL);
					str_ansi* p_ansi_text = GetANSIFromUTF8((str_utf8*)p_text);
					if (s_value.empty())
						s_value = p_ansi_text;
					free(p_text);
					delete[] p_ansi_text;
				}
				else
					assert(false);
			}
			break;
		case ID3_FIELD_TYPE_STRINGLIST :
			{
				unsigned int n_item_count = id3_field_getnstrings(p_field);
				//assert(id3_field_getnstrings(p_field) == 1);
				id3_ucs4_t const* p_text_u = id3_field_getstrings(p_field, 0);
				if (p_text_u == NULL)
					break;
				assert(p_text_u != NULL);
				assert(b_get_encoding);
				if (encoding_type == ID3_FIELD_TEXTENCODING_ISO_8859_1)
				{
					id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
					if (p_text == NULL)
						break;
					assert(p_text != NULL);
					if (s_value.empty())
						s_value = (char*)p_text;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_16 || encoding_type == ID3_FIELD_TEXTENCODING_UTF_16BE)
				{
					id3_utf16_t* p_text = id3_ucs4_utf16duplicate(p_text_u);
					if (p_text == NULL)
						break;
					assert(p_text != NULL);
					ATL::CW2A ansi_text((LPCWSTR)p_text);
					if (s_value.empty())
						s_value = ansi_text;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_8)
				{
					id3_utf8_t* p_text = id3_ucs4_utf8duplicate(p_text_u);
					if (p_text == NULL)
						break;
					assert(p_text != NULL);
					str_ansi* p_ansi_text = GetANSIFromUTF8((str_utf8*)p_text);
					if (s_value.empty())
						s_value = p_ansi_text;
					free(p_text);
					delete[] p_ansi_text;
				}
				else
					assert(false);
			}
			break;
		case ID3_FIELD_TYPE_LANGUAGE :
			{
				string s_language = p_field->immediate.value;
			}
			break;
		default :
			assert(false);
			break;
		}
		++p_field;
	}
	return s_value;
}

wstring id3_tag_base::get_field_wstring(struct id3_frame* p_frame)
{
	assert(p_frame != NULL);
	//assert(p_frame->nfields == 2);
	id3_field_textencoding encoding_type = ID3_FIELD_TEXTENCODING_ISO_8859_1;
	bool b_get_encoding(false);
	id3_field* p_field = NULL;
	p_field = p_frame->fields;
	wstring s_value;
	for (size_t i = 0; i < p_frame->nfields; ++i)
	{
		assert(p_field != NULL);
		switch (p_field->type)
		{
		case ID3_FIELD_TYPE_TEXTENCODING : 
			encoding_type = id3_field_gettextencoding(p_field);
			b_get_encoding = true;
			break;
		case ID3_FIELD_TYPE_LATIN1 :
			{
				id3_latin1_t const * p_text = id3_field_getlatin1(p_field);
				assert(p_text != NULL);
				s_value = CA2W((char*)p_text).m_psz;
			}
			break;
		case ID3_FIELD_TYPE_LATIN1FULL :
			{
				id3_latin1_t const* p_text = id3_field_getfulllatin1(p_field);
				assert(p_text != NULL);
				s_value = CA2W((char*)p_text).m_psz;
			}
			break;
		case ID3_FIELD_TYPE_LATIN1LIST :
			{
				assert(p_field->latin1list.nstrings == 1);
				id3_latin1_t* p_text = p_field->latin1list.strings[0];
				assert(p_text != NULL);
				s_value = CA2W((char*)p_text).m_psz;
			}
			break;
		case ID3_FIELD_TYPE_STRING :
			{
				id3_ucs4_t const* p_text_u = id3_field_getstring(p_field);
				assert(p_text_u != NULL);
				assert(b_get_encoding);
				if (encoding_type == ID3_FIELD_TEXTENCODING_ISO_8859_1)
				{
					id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
					assert(p_text != NULL);
					if (s_value.empty())
						s_value = CA2W((char*)p_text).m_psz;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_16 || encoding_type == ID3_FIELD_TEXTENCODING_UTF_16BE)
				{
					id3_utf16_t* p_text = id3_ucs4_utf16duplicate(p_text_u);
					assert(p_text != NULL);
					
					if (s_value.empty())
						s_value =((LPCWSTR)p_text);;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_8)
				{
					id3_utf8_t* p_text = id3_ucs4_utf8duplicate(p_text_u);
					assert(p_text != NULL);
					str_utf16* p_utf16_text = GetUTF16FromUTF8((str_utf8*)p_text);
					if (s_value.empty())
						s_value = p_utf16_text;
					free(p_text);
					delete[] p_utf16_text;
				}
				else
					assert(false);
			}
			break;
		case ID3_FIELD_TYPE_STRINGFULL : 
			{
				id3_ucs4_t const* p_text_u = id3_field_getfullstring(p_field);
				assert(p_text_u != NULL);
				assert(b_get_encoding);

				if (encoding_type == ID3_FIELD_TEXTENCODING_ISO_8859_1)
				{
					id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
					assert(p_text != NULL);
					if (s_value.empty())
						s_value = CA2W((char*)p_text).m_psz;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_16 || encoding_type == ID3_FIELD_TEXTENCODING_UTF_16BE)
				{
					id3_utf16_t* p_text = id3_ucs4_utf16duplicate(p_text_u);
					assert(p_text != NULL);
					
					if (s_value.empty())
						s_value = (LPCWSTR)p_text;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_8)
				{
					id3_utf8_t* p_text = id3_ucs4_utf8duplicate(p_text_u);
					assert(p_text != NULL);
					str_utf16* p_utf16_text = GetUTF16FromUTF8((str_utf8*)p_text);
					if (s_value.empty())
						s_value = p_utf16_text;
					free(p_text);
					delete[] p_utf16_text;
				}
				else
					assert(false);
			}
			break;
		case ID3_FIELD_TYPE_STRINGLIST :
			{
				unsigned int n_item_count = id3_field_getnstrings(p_field);
				//assert(id3_field_getnstrings(p_field) == 1);
				id3_ucs4_t const* p_text_u = id3_field_getstrings(p_field, 0);
				if (p_text_u == NULL)
					break;
				assert(p_text_u != NULL);
				assert(b_get_encoding);
				if (encoding_type == ID3_FIELD_TEXTENCODING_ISO_8859_1)
				{
					id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
					if (p_text == NULL)
						break;
					assert(p_text != NULL);
					if (s_value.empty())
						s_value = CA2W((char*)p_text).m_psz;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_16 || encoding_type == ID3_FIELD_TEXTENCODING_UTF_16BE)
				{
					id3_utf16_t* p_text = id3_ucs4_utf16duplicate(p_text_u);
					if (p_text == NULL)
						break;
					assert(p_text != NULL);
					
					if (s_value.empty())
						s_value = (LPCWSTR)p_text;
					free(p_text);
				}
				else if (encoding_type == ID3_FIELD_TEXTENCODING_UTF_8)
				{
					id3_utf8_t* p_text = id3_ucs4_utf8duplicate(p_text_u);
					if (p_text == NULL)
						break;
					assert(p_text != NULL);
					str_utf16* p_utf16_text = GetUTF16FromUTF8((str_utf8*)p_text);
					if (s_value.empty())
						s_value = p_utf16_text;
					free(p_text);
					delete[] p_utf16_text;
				}
				else
					assert(false);
			}
			break;
		case ID3_FIELD_TYPE_LANGUAGE :
			{
				string s_language = p_field->immediate.value;
			}
			break;
		default :
			assert(false);
			break;
		}
		++p_field;
	}
	return s_value;
}
int id3_tag_base::get_field_int(struct id3_frame* p_frame)
{
	assert(p_frame != NULL);
	assert(p_frame->nfields == 1);
	id3_field* p_field = NULL;
	p_field = p_frame->fields;
	int n_value = 0;
	for (size_t i = 0; i < p_frame->nfields; ++i)
	{
		assert(p_field != NULL);
		switch (p_field->type)
		{
		case ID3_FIELD_TYPE_INT8 :
			n_value = static_cast<int>(id3_field_getint(p_field));
			break;
		case ID3_FIELD_TYPE_INT16 :
			n_value = static_cast<int>(id3_field_getint(p_field));
			break;
		case ID3_FIELD_TYPE_INT24 :
			n_value = static_cast<int>(id3_field_getint(p_field));
			break;
		case ID3_FIELD_TYPE_INT32 :
			n_value = static_cast<int>(id3_field_getint(p_field));
			break;
		case ID3_FIELD_TYPE_INT32PLUS : 
			n_value = static_cast<int>(p_field->binary.length);
			break;
		default :
			assert(false);
			break;
		}
		++p_field;
	}
	return n_value;
}

pair<int, string> id3_tag_base::get_genre_field(struct id3_frame* p_frame, bool b_id3_v1)
{
	int n_index = 0;
	string s_name;
	assert(p_frame != NULL);
	if(p_frame->nfields != 2)
	{
		n_index = INVALID_INDEX;
		s_name="";
		return make_pair(n_index, s_name);
	}
	id3_field* p_field = NULL;
	p_field = p_frame->fields;
	for (size_t i = 0; i < p_frame->nfields; ++i)
	{
		assert(p_field != NULL);
		switch (p_field->type)
		{
		case ID3_FIELD_TYPE_TEXTENCODING : 
			break;
		case ID3_FIELD_TYPE_STRINGLIST :
			{
				unsigned int n_item_count = id3_field_getnstrings(p_field);
				assert(n_item_count == 1 || n_item_count == 2);
				/*
				if (b_id3_v1)
					assert(n_item_count == 1);
				else
					assert(n_item_count == 2);
				*/
				id3_ucs4_t const* p_text_u = id3_field_getstrings(p_field, 0);
				assert(p_text_u != NULL);
				id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
				assert(p_text != NULL);
				try
				{
					n_index = boost::lexical_cast<int>(p_text);
				}
				catch (boost::bad_lexical_cast&)
				{
					n_index = INVALID_INDEX;
				}
				free(p_text);
				p_text = NULL;
				id3_ucs4_t const* p_name_u = id3_genre_name(p_text_u);
				assert(p_name_u != NULL);
				id3_latin1_t* p_name = id3_ucs4_latin1duplicate(p_name_u);
				assert(p_name != NULL);
				s_name = (char*)p_name;
				free(p_name);
				p_name = NULL;
				
				if (n_item_count == 2)
				{
					p_text_u = id3_field_getstrings(p_field, 1);
					assert(p_text_u != NULL);
					id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
					assert(p_text != NULL);
					s_name = (char*)p_text;
					free(p_text);
					p_text = NULL;
				}

			}
			break;
		default :
			assert(false);
			break;
		}
		++p_field;
	}
	return make_pair(n_index, s_name);
}

pair<int, wstring> id3_tag_base::get_genre_field_wstring(struct id3_frame* p_frame, bool b_id3_v1)
{
	int n_index = 0;
	string s_name;
	assert(p_frame != NULL);
	if(p_frame->nfields != 2)
	{
		n_index = INVALID_INDEX;
		wstring sw_name=L"";
		return make_pair(n_index, sw_name);
	}
	id3_field* p_field = NULL;
	p_field = p_frame->fields;
	for (size_t i = 0; i < p_frame->nfields; ++i)
	{
		assert(p_field != NULL);
		switch (p_field->type)
		{
		case ID3_FIELD_TYPE_TEXTENCODING : 
			break;
		case ID3_FIELD_TYPE_STRINGLIST :
			{
				unsigned int n_item_count = id3_field_getnstrings(p_field);
				if(n_item_count != 1 && n_item_count != 2)
				{
					n_index = INVALID_INDEX;
					s_name = "";
					break;
				}
				
				/*
				if (b_id3_v1)
				assert(n_item_count == 1);
				else
				assert(n_item_count == 2);
				*/
				id3_ucs4_t const* p_text_u = id3_field_getstrings(p_field, 0);
				assert(p_text_u != NULL);
				id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
				assert(p_text != NULL);
				try
				{
					n_index = boost::lexical_cast<int>(p_text);
				}
				catch (boost::bad_lexical_cast&)
				{
					n_index = INVALID_INDEX;
				}
				free(p_text);
				p_text = NULL;
				id3_ucs4_t const* p_name_u = id3_genre_name(p_text_u);
				assert(p_name_u != NULL);
				id3_latin1_t* p_name = id3_ucs4_latin1duplicate(p_name_u);
				assert(p_name != NULL);
				s_name = (char*)p_name;
				free(p_name);
				p_name = NULL;

				if (n_item_count == 2)
				{
					p_text_u = id3_field_getstrings(p_field, 1);
					assert(p_text_u != NULL);
					id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
					assert(p_text != NULL);
					s_name = (char*)p_text;
					free(p_text);
					p_text = NULL;
				}

			}
			break;
		default :
			assert(false);
			break;
		}
		++p_field;
	}

	wstring sw_name;
	sw_name = (CA2W(s_name.c_str()).m_psz);
	return make_pair(n_index, sw_name);
}


void id3_tag_base::print_frame(struct id3_frame* p_frame)
{
	assert(p_frame != NULL);
	cout << "---print frame info---" << endl;
	cout << "id : " << p_frame->id << ", refcount : " << p_frame->refcount << ", flags : " << p_frame->flags << endl;
	cout << "description : " << p_frame->description << endl;
	cout << "group id : " << p_frame->group_id << ", field count : " << p_frame->nfields << endl;
	cout << "encryption mode : " << p_frame->encryption_method << ", encoded length : " << p_frame->encoded_length << ", decoded length : " << p_frame->decoded_length << endl;
	if (p_frame->encoded_length > 0)
		cout << "encoded : " << p_frame->encoded << endl;
	id3_field_textencoding field_encoding = ID3_FIELD_TEXTENCODING_ISO_8859_1;
	bool b_get_encoding(false);
	id3_field* p_field = NULL;
	p_field = p_frame->fields;
	for (size_t i = 0; i < p_frame->nfields; ++i)
	{
		assert(p_field != NULL);
		switch (p_field->type)
		{
		case ID3_FIELD_TYPE_TEXTENCODING : 
			{
				id3_field_textencoding encoding = id3_field_gettextencoding(p_field);
				field_encoding = encoding;
				b_get_encoding = true;
				switch (encoding)
				{
				case	ID3_FIELD_TEXTENCODING_ISO_8859_1 :
					cout << "text coding is : latin1" << endl;
					break;
				case	ID3_FIELD_TEXTENCODING_UTF_16 :
					cout << "text coding is : utf-16" << endl;
					break;
				case	ID3_FIELD_TEXTENCODING_UTF_16BE :
					cout << "text coding is : utf-16be" << endl;
					break;
				case	ID3_FIELD_TEXTENCODING_UTF_8 :
					cout << "text coding is : utf-8" << endl;
					break;
				}
			}
			break;
		case ID3_FIELD_TYPE_LATIN1 :
			{
				id3_latin1_t const * p_text = id3_field_getlatin1(p_field);
				assert(p_text != NULL);
				cout << "latin1 value : " << p_text << endl;
			}
			break;
		case ID3_FIELD_TYPE_LATIN1FULL :
			{
				id3_latin1_t const* p_text = id3_field_getfulllatin1(p_field);
				assert(p_text != NULL);
				cout << "latin1full value : " << p_text << endl;
			}
			break;
		case ID3_FIELD_TYPE_LATIN1LIST :
			{
				cout << "latin1list item count : " << p_field->latin1list.nstrings << endl;
				for (unsigned int j = 0; j < p_field->latin1list.nstrings; ++j)
				{
					id3_latin1_t* p_text = p_field->latin1list.strings[j];
					assert(p_text != NULL);
					cout << "item " << j << " 's value : " << p_text << endl;
				}
			}
			break;
		case ID3_FIELD_TYPE_STRING :
			{
				id3_ucs4_t const* p_text_u = id3_field_getstring(p_field);
				assert(p_text_u != NULL);
				id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
				assert(p_text != NULL);
				cout << "string value : " << p_text << endl;
				free(p_text);
				p_text = NULL;
			}
			break;
		case ID3_FIELD_TYPE_STRINGFULL : 
			{
				id3_ucs4_t const* p_text_u = id3_field_getfullstring(p_field);
				assert(p_text_u != NULL);
				id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
				assert(p_text != NULL);
				cout << "stringfull value : " << p_text << endl;
				free(p_text);
				p_text = NULL;
			}
			break;
		case ID3_FIELD_TYPE_STRINGLIST :
			{
				unsigned int n_value = id3_field_getnstrings(p_field);
				cout << "stringlist item count : " << n_value << endl;
				for (unsigned int j = 0; j < n_value; ++j)
				{
					id3_ucs4_t const* p_text_u = id3_field_getstrings(p_field, j);
					assert(p_text_u != NULL);
					assert(b_get_encoding);
					if (field_encoding == ID3_FIELD_TEXTENCODING_ISO_8859_1)
					{
						id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
						assert(p_text != NULL);
						cout << "item " << j << " 's value : " << p_text << endl;
						free(p_text);
						p_text = NULL;
					}
					else if (field_encoding == ID3_FIELD_TEXTENCODING_UTF_16 || field_encoding == ID3_FIELD_TEXTENCODING_UTF_16BE)
					{
						id3_utf16_t* p_text = id3_ucs4_utf16duplicate(p_text_u);
						assert(p_text != NULL);
						ATL::CW2A ansi_text((LPCWSTR)p_text);
						cout << "item " << j << " 's value : " << ansi_text << endl;
						free(p_text);
						p_text = NULL;
					}
					else if (field_encoding == ID3_FIELD_TEXTENCODING_UTF_8)
					{
						id3_utf8_t* p_text = id3_ucs4_utf8duplicate(p_text_u);
						assert(p_text != NULL);
						ATL::CW2A ansi_text((LPCWSTR)p_text);
						cout << "item " << j << " 's value : " << ansi_text << endl;
						free(p_text);
						p_text = NULL;
					}
				}
			}
			break;
		case ID3_FIELD_TYPE_LANGUAGE :
			{
				cout << "language value : " <<  p_field->immediate.value << endl;
			}
			break;
		case ID3_FIELD_TYPE_FRAMEID :
			{
				char const* p_value = id3_field_getframeid(p_field);
				assert(p_value != NULL);
				cout << "frameid value : " << p_value << endl;
			}
			break;
		case ID3_FIELD_TYPE_DATE :
			{
				cout << "date value : " <<  p_field->immediate.value << endl;
			}
			break;
		case ID3_FIELD_TYPE_INT8 :
			{
				signed long n_value = id3_field_getint(p_field);
				cout << "int8 value : " << n_value << endl;
			}
			break;
		case ID3_FIELD_TYPE_INT16 :
			{
				signed long n_value = id3_field_getint(p_field);
				cout << "int16 value : " << n_value << endl;
			}
			break;
		case ID3_FIELD_TYPE_INT24 :
			{
				signed long n_value = id3_field_getint(p_field);
				cout << "int24 value : " << n_value << endl;
			}
			break;
		case ID3_FIELD_TYPE_INT32 :
			{
				signed long n_value = id3_field_getint(p_field);
				cout << "int32 value : " << n_value << endl;
			}
			break;
		case ID3_FIELD_TYPE_INT32PLUS : 
			{
				id3_length_t n_len = p_field->binary.length;
				cout << "int32plus length : " << n_len << endl;
			}
			break;
		case ID3_FIELD_TYPE_BINARYDATA :
			{
				id3_length_t n_len = 0;
				id3_byte_t const* p_binary = id3_field_getbinarydata(p_field, &n_len);
				cout << "binary length : " << n_len <<", data not print..." << endl;
			}
			break;
		}
		++p_field;
	}
	return;
}

//////////////////////////////////////////////////////////////////////////
id3_v1_tag::id3_v1_tag()
{
	memset(mp_buf, 0x0, sizeof(unsigned char) * TAG_ID3_V1_LEN);
	mn_begin = 0;
	mp_tag = NULL;
}

id3_v1_tag::~id3_v1_tag()
{
	if (mp_tag != NULL)
		id3_tag_delete(mp_tag);
}

bool id3_v1_tag::init(unsigned char* p_data, unsigned int n_begin, unsigned int n_length)
{
	assert(p_data != NULL);
	assert(n_length == TAG_ID3_V1_LEN);
	memcpy(mp_buf, p_data, TAG_ID3_V1_LEN);
	mn_begin = n_begin;
	assert(mp_tag == NULL);
	mp_tag = id3_tag_parse(mp_buf, TAG_ID3_V1_LEN);
	assert(mp_tag != NULL);
	return true;
}

int id3_v1_tag::get_length() const
{
	return TAG_ID3_V1_LEN;
}

int id3_v1_tag::get_offset() const
{
	return mn_begin;
}

bool id3_v1_tag::is_append() const
{
	return true;
}

tag_base_data* id3_v1_tag::get_base_tag()
{
	assert(mp_tag != NULL);
	tag_base_data* p_base_data = new tag_base_data;
	p_base_data->clear();
	
	bool b_result = parse_base_data(mp_tag, true, p_base_data);
	assert(b_result);

	return p_base_data;
}
tag_base_data_w* id3_v1_tag::get_base_tag_wstring()
{
	assert(mp_tag != NULL);
	tag_base_data_w* p_base_data = new tag_base_data_w;
	p_base_data->clear();

	bool b_result = parse_base_data_wstring(mp_tag, true, p_base_data);
	assert(b_result);

	return p_base_data;
}
string id3_v1_tag::get_tag_name()
{
	return const_enum_map::ma_tags[st_id3_v1];
}

wstring id3_v1_tag::get_tag_name_wstring()
{
	string str =const_enum_map::ma_tags[st_id3_v1];
	return wstring(CA2W(str.c_str()).m_psz);
}
unsigned int id3_v1_tag::get_tag_serial()
{
	return st_id3_v1;
}

int id3_v1_tag::render(unsigned char* p_buf)
{
	id3_length_t n_tag_len = 0;
	n_tag_len = id3_tag_render(mp_tag, p_buf);
	return boost::numeric_cast<int>(n_tag_len);
}

bool id3_v1_tag::import(const tag_base_data* p_data)
{
	bool b_result(false);
	assert(mp_tag == NULL);
	int n_result = 0;
	if (p_data != NULL)
	{
		mp_tag = id3_tag_new();
		assert(mp_tag != NULL);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_CRC, 1);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_ID3V1, 1);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_CRC, 0);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_COMPRESSION, 0);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_UNSYNCHRONISATION, 0);

		b_result = add_frame_string(mp_tag, ID3V2_FRAME_TITLE_DESC, p_data->s_title, true);
		assert(b_result);
		b_result = add_frame_string(mp_tag, ID3V2_FRAME_SOLOIST, p_data->s_artist, true);
		assert(b_result);
		b_result = add_frame_string(mp_tag, ID3V2_FRAME_AMS_TITLE, p_data->s_album, true);
		assert(b_result);
		b_result = add_frame_string(mp_tag, ID3V2_FRAME_RECORDING_TIME, p_data->s_date, true);
		assert(b_result);
		b_result = add_frame_string(mp_tag, ID3V2_FRAME_CONTENT_TYPE, p_data->s_genre, true);
		assert(b_result);
		if (p_data->n_track > 0)
		{
			assert(p_data->n_track <= tag_base_data::MAX_SONG_COUNT);
			char s_buf[32];
			sprintf(s_buf, "%u", p_data->n_track);
			b_result = add_frame_string(mp_tag, ID3V2_FRAME_SET_TRACK, string(s_buf), false);
			assert(b_result);
		}
	}
	return true;
}
bool id3_v1_tag::import_wstring(const tag_base_data_w* p_data)
{
	bool b_result(false);
	assert(mp_tag == NULL);
	int n_result = 0;
	if (p_data != NULL)
	{
		mp_tag = id3_tag_new();
		assert(mp_tag != NULL);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_CRC, 1);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_ID3V1, 1);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_CRC, 0);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_COMPRESSION, 0);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_UNSYNCHRONISATION, 0);

		b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_TITLE_DESC, p_data->s_title, true);
		assert(b_result);
		b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_SOLOIST, p_data->s_artist, true);
		assert(b_result);
		b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_AMS_TITLE, p_data->s_album, true);
		assert(b_result);
		b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_RECORDING_TIME, p_data->s_date, true);
		assert(b_result);
		b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_CONTENT_TYPE, p_data->s_genre, true);
		assert(b_result);
		if (p_data->n_track > 0)
		{
			assert(p_data->n_track <= tag_base_data::MAX_SONG_COUNT);
			wchar_t s_buf[32];
			wsprintfW(s_buf, (L"%u"), p_data->n_track);
			b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_SET_TRACK, wstring(s_buf), false);
			assert(b_result);
		}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
id3_v2_tag::id3_v2_tag()
{
	mn_begin = 0;
	mn_length = 0;
	mp_tag = NULL;
}

id3_v2_tag::~id3_v2_tag()
{
	if (mp_tag != NULL)
		id3_tag_delete(mp_tag);
}

bool id3_v2_tag::init(boost::shared_array<unsigned char> sp_buf, unsigned int n_begin, unsigned int n_length)
{
	assert(mp_tag == NULL);
	assert(sp_buf);
	msp_buf = sp_buf;
	mn_begin = n_begin;
	mn_length = n_length;

	mp_tag = id3_tag_parse(msp_buf.get(), mn_length);
//	assert(mp_tag != NULL);

	return true;
}

int id3_v2_tag::get_length() const
{
	return mn_length;
}

int id3_v2_tag::get_offset() const
{
	return mn_begin;
}

bool id3_v2_tag::is_append() const
{
	return false;
}

tag_base_data* id3_v2_tag::get_base_tag()
{
	tag_base_data* p_base_data = new tag_base_data;
	p_base_data->clear();

	bool b_result = parse_base_data(mp_tag, false, p_base_data);
	assert(b_result);

	return p_base_data;
}

tag_base_data_w* id3_v2_tag::get_base_tag_wstring()
{
	tag_base_data_w* p_base_data = new tag_base_data_w;
	p_base_data->clear();

	bool b_result = parse_base_data_wstring(mp_tag, false, p_base_data);
	assert(b_result);

	return p_base_data;
}


string id3_v2_tag::get_tag_name()
{
	return const_enum_map::ma_tags[st_id3_v2];
	
}
wstring id3_v2_tag::get_tag_name_wstring()
{
	string str =const_enum_map::ma_tags[st_id3_v2];
	return wstring(CA2W(str.c_str()).m_psz);
}

unsigned int id3_v2_tag::get_tag_serial()
{
	return st_id3_v2;
}

int id3_v2_tag::render(unsigned char* p_buf)
{
	int n_result = 0;
	assert(mp_tag != NULL);
	
	n_result = id3_tag_render(mp_tag, p_buf);
	return n_result;
}

void id3_v2_tag::set_padding_size(unsigned int n_bytes)
{
	assert(mp_tag != NULL);
	id3_tag_setlength(mp_tag, n_bytes);			//设置最小长度
}

bool id3_v2_tag::has_picture()
{
	bool b_result = false;
	assert(mp_tag != NULL);
	struct id3_frame* p_frame = NULL;
	unsigned int n_index = 0;
	b_result = id3_tag_findframe(mp_tag, ID3V2_FRAME_PICTURE, n_index) != NULL;
	return b_result;
}

unsigned int id3_v2_tag::rip_picture(const char* p_raw_name)
{
	assert(p_raw_name != NULL);
	bool b_result(false);
	assert(mp_tag != NULL);
	struct id3_frame* p_frame = NULL;
	unsigned int n_index = 0;
	int n_result = 0;
	string s_suffix;
	string s_file_name = p_raw_name;
	unsigned int n_rip_count = 0;
	while ((p_frame = id3_tag_findframe(mp_tag, ID3V2_FRAME_PICTURE, n_index)) != NULL)
	{ 
		id3_field* p_field = NULL;
		p_field = p_frame->fields;
		id3_length_t n_pic_len = 0;
		id3_byte_t const* p_binary = NULL;
		unsigned int n_check_type = 0;
		for (size_t i = 0; i < p_frame->nfields; ++i)
		{
			assert(p_field != NULL);
			switch (p_field->type)
			{
			case ID3_FIELD_TYPE_TEXTENCODING : 
				{
					id3_field_textencoding encoding = id3_field_gettextencoding(p_field);
					assert(encoding == ID3_FIELD_TEXTENCODING_ISO_8859_1);
				}
				break;
			case ID3_FIELD_TYPE_LATIN1 :
				{
					id3_latin1_t const * p_text = id3_field_getlatin1(p_field);
					assert(p_text != NULL);   // "image/jpeg" "image/png" or "JPG" "PNG"
					if (strcmp((const char*)p_text, "image/jpeg") == 0 || strcmp((const char*)p_text, "JPG") == 0)
					{
						s_suffix = ".jpg";
					}
					else if (strcmp((const char*)p_text, "image/png") == 0 || strcmp((const char*)p_text, "PNG") == 0)
					{
						s_suffix = ".png";
					}
					else
						assert(false);
				}
				break;
			case ID3_FIELD_TYPE_STRING :
				{
					id3_ucs4_t const* p_text_u = id3_field_getstring(p_field);
					assert(p_text_u != NULL);
					id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
					assert(p_text != NULL);
					free(p_text);
					p_text = NULL;
				}
				break;
			case ID3_FIELD_TYPE_INT8 :
				{
					signed long n_value = id3_field_getint(p_field);		//3 is cover(front) and 0 is other
					switch(n_value)
					{
					case 3 :
						s_file_name += "_cover";
						break;
					case 4 :
						s_file_name += "_back";
						break;
					default :
						s_file_name += "_";
						s_file_name += boost::lexical_cast<string>(n_value);
						break;
					}
				}
				break;
			case ID3_FIELD_TYPE_BINARYDATA :
				{
					n_pic_len = 0;
					p_binary = id3_field_getbinarydata(p_field, &n_pic_len);
					n_check_type = get_pic_type(p_binary, n_pic_len);
					//assert(n_check_type == 1 || n_check_type == 2);
				}
				break;
			}
			++p_field;
		}
		if (p_binary != NULL && n_pic_len > 0)
		{
			s_file_name += s_suffix;
			if(s_file_name.length()<=0)            
			{
                ++n_rip_count;
			}
            else
			{
				file_io pic_file;
				n_result = pic_file.create(s_file_name.c_str());
				if (n_result == 0)
				{
					unsigned int n_bytes_written = 0;
					n_result = pic_file.write(p_binary, n_pic_len, &n_bytes_written);
					assert(n_result == 0 && n_pic_len == n_bytes_written);
					n_result = pic_file.close();
					assert(n_result == 0);
					++n_rip_count;
				}
			}

		}
		++n_index;
	}
	return n_rip_count;
}

unsigned int id3_v2_tag::rip_picture(const wchar_t* p_raw_name)
{
	assert(p_raw_name != NULL);
	bool b_result(false);
	assert(mp_tag != NULL);
	struct id3_frame* p_frame = NULL;
	unsigned int n_index = 0;
	int n_result = 0;
	wstring s_suffix;
	wstring s_file_name = p_raw_name;
	unsigned int n_rip_count = 0;
	while ((p_frame = id3_tag_findframe(mp_tag, ID3V2_FRAME_PICTURE, n_index)) != NULL)
	{ 
		id3_field* p_field = NULL;
		p_field = p_frame->fields;
		id3_length_t n_pic_len = 0;
		id3_byte_t const* p_binary = NULL;
		unsigned int n_check_type = 0;
		for (size_t i = 0; i < p_frame->nfields; ++i)
		{
			assert(p_field != NULL);
			switch (p_field->type)
			{
			case ID3_FIELD_TYPE_TEXTENCODING : 
				{
					id3_field_textencoding encoding = id3_field_gettextencoding(p_field);
					assert(encoding == ID3_FIELD_TEXTENCODING_ISO_8859_1);
				}
				break;
			case ID3_FIELD_TYPE_LATIN1 :
				{
					//id3_latin1_t const * p_text = id3_field_getlatin1(p_field);
					//assert(p_text != NULL);   // "image/jpeg" "image/png" or "JPG" "PNG"
					//if (strcmp((const char*)p_text, "image/jpeg") == 0 || strcmp((const char*)p_text, "JPG") == 0)
					//{
					//	s_suffix = L".jpg";
					//}
					//else if (strcmp((const char*)p_text, "image/png") == 0 || strcmp((const char*)p_text, "PNG") == 0)
					//{
					//	s_suffix = L".png";
					//}
					//else
					//	assert(false);
				}
				break;
			case ID3_FIELD_TYPE_STRING :
				{
					id3_ucs4_t const* p_text_u = id3_field_getstring(p_field);
					assert(p_text_u != NULL);
					id3_latin1_t* p_text = id3_ucs4_latin1duplicate(p_text_u);
					assert(p_text != NULL);
					free(p_text);
					p_text = NULL;
				}
				break;
			case ID3_FIELD_TYPE_INT8 :
				{
					//signed long n_value = id3_field_getint(p_field);		//3 is cover(front) and 0 is other
					//switch(n_value)
					//{
					//case 3 :
					//	s_file_name += L"_cover";
					//	break;
					//case 4 :
					//	s_file_name += L"_back";
					//	break;
					//default :
					//	s_file_name += L"_";
					//	s_file_name += boost::lexical_cast<wstring>(n_value);
					//	break;
					//}
				}
				break;
			case ID3_FIELD_TYPE_BINARYDATA :
				{
					n_pic_len = 0;
					p_binary = id3_field_getbinarydata(p_field, &n_pic_len);
					n_check_type = get_pic_type(p_binary, n_pic_len);
					//assert(n_check_type == 1 || n_check_type == 2);
				}
				break;
			}
			++p_field;
		}
		if (p_binary != NULL && n_pic_len > 0)
		{
			//s_file_name += s_suffix;
			if (s_file_name.length()<=0)
			{

				++n_rip_count;
			}
			else
			{
				file_io pic_file;
				n_result = pic_file.create(s_file_name.c_str());
				if (n_result == 0)
				{
					unsigned int n_bytes_written = 0;
					n_result = pic_file.write(p_binary, n_pic_len, &n_bytes_written);
					assert(n_result == 0 && n_pic_len == n_bytes_written);
					n_result = pic_file.close();
					assert(n_result == 0);
					++n_rip_count;
				}
			}
		}
		++n_index;
	}
	return n_rip_count;
}

bool id3_v2_tag::add_picture(const wchar_t * sz_filename)
{
	if (mp_tag == NULL)
	{
		mp_tag = id3_tag_new();
		assert(mp_tag != NULL);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_CRC, 0);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_COMPRESSION, 0);
		id3_tag_options(mp_tag, ID3_TAG_OPTION_UNSYNCHRONISATION, 0);

		id3_tag_setlength(mp_tag, TAG_ID3_V2_LEN);			//设置最小长度
	}
	
	id3_frame* p_frame = id3_frame_new("APIC");
	if (p_frame == NULL)
		return false;
	id3_field* p_field = NULL;
		p_field = p_frame->fields;
		id3_length_t n_pic_len = 0;
		id3_byte_t const* p_binary = NULL;
		unsigned int n_check_type = 0;
		for (size_t i = 0; i < p_frame->nfields; ++i)
		{
			assert(p_field != NULL);
			switch (p_field->type)
			{
			case ID3_FIELD_TYPE_TEXTENCODING : 
				{
					id3_field_settextencoding(p_field,ID3_FIELD_TEXTENCODING_ISO_8859_1);
								
				}			
				break;
			case ID3_FIELD_TYPE_LATIN1 :
				{
					id3_field_setlatin1(p_field,(const id3_latin1_t *)"image/jpeg");
				}
				break;
			case ID3_FIELD_TYPE_STRING :
				{
					
					//id3_field_setlatin1(p_field,(const id3_latin1_t *)"Attached picture");
					id3_latin1_t* p_latin = (id3_latin1_t*)"";
					
					str_utf16* p_value_w = GetUTF16FromANSI("");
					assert(p_value_w != NULL);
					ATL::CA2W value_w2("My Picture");
					str_utf16* p_value_w2 =LPWSTR(value_w2);
					id3_ucs4_t* p_ucs = id3_utf16_ucs4duplicate((id3_utf16_t*)p_value_w);
					assert(p_ucs != NULL);
					long n_result = id3_field_setstring(p_field, p_ucs);
					assert(n_result == 0);
					free(p_ucs);
					p_ucs = NULL;
					delete[] p_value_w;
					p_value_w = NULL;
				}
					
				break;
			case ID3_FIELD_TYPE_INT8 :
				{
					id3_field_setint(p_field,3);		//3 is cover(front) and 0 is other
				}
				break;
			case ID3_FIELD_TYPE_BINARYDATA :
				{
					file_io pic_file;
 					long n_result = pic_file.open(sz_filename,true);
					if (n_result == 0)
					{
 						long file_size=pic_file.get_size();
						pic_file.seek(0,FILE_BEGIN);
						char *p_buffer=new char [file_size];
						if(NULL == p_buffer)
							return false;
						unsigned int n_bytes_read = 0;

						n_result = pic_file.read(p_buffer, file_size, &n_bytes_read);
						assert(n_result == 0 && file_size == n_bytes_read);
						n_result = pic_file.close();
        				assert(n_result == 0);
						int len =id3_field_setbinarydata(p_field,(const id3_byte_t *)p_buffer,file_size);
						assert(len == 0);
						delete p_buffer;
					}
				}
				break;
			}	

			++p_field;
		}

	int n_result = id3_tag_attachframe(mp_tag, p_frame);
	assert(n_result == 0);

	return true;
}

bool id3_v2_tag::add_encrypt_frame(int n_size,char *p_buffer)
{
	assert(mp_tag != NULL);

	id3_frame* p_frame = id3_frame_new("PRIV");
	if (p_frame == NULL)
		return false;
	id3_field* p_field = NULL;
	p_field = p_frame->fields;
	id3_length_t n_pic_len = 0;
	id3_byte_t const* p_binary = NULL;
	unsigned int n_check_type = 0;
	for (size_t i = 0; i < p_frame->nfields; ++i)
	{
		assert(p_field != NULL);
		switch (p_field->type)
		{
		case ID3_FIELD_TYPE_LATIN1 : 
			{
				id3_field_setlatin1(p_field,(const id3_latin1_t *)"lbsd");
			}			
			break;
	
		case ID3_FIELD_TYPE_BINARYDATA :
			{
				DWORD m[32]={1127029477u,2787430716u,1428749768u,2965828749u,274565019u,1178412241u,596746388u,1089869494u,2588017383u,4291593401u,3053843839u,1725947188u,4214236667u,1275985824u,1104304020u,2668483176u,336974020u,2291569464u,1386003983u,1116206178u,2974800363u,401112141u,834689850u,1521252788u,2281738211u,3562763741u,129043013u,758258210u,2094448516u,100847113u,1282080350u,753479536u};
				encrypt_mod::PK pk;
				for(int i=0;i<32;i++)
				{
					pk.m[i]=m[i];
				}
				
				encrypt_mod::public_key p_key;
				p_key.PK_to_vlong(pk);
				int n_out_len=0;
				char * p_out_buf = p_key.encrypt(p_buffer,n_size,n_out_len);

				int len =id3_field_setbinarydata(p_field,(const id3_byte_t *)p_out_buf,n_out_len);
				p_key.delete_buf(p_out_buf);
				assert(len == 0);
			}
			break;
		}	

		++p_field;
	}

	int n_result = id3_tag_attachframe(mp_tag, p_frame);
	assert(n_result == 0);

	return true;
}

bool id3_v2_tag::get_encrypt_frame_data(int &n_size,char *p_buffer)
{
	
	bool b_result(false);
	assert(mp_tag != NULL);
	struct id3_frame* p_frame = NULL;
	unsigned int n_index = 0;
	int n_result = 0;
	unsigned int n_rip_count = 0;

	while ((p_frame = id3_tag_findframe(mp_tag, ID3V2_FRAME_PRIVATE, n_index)) != NULL)
	{ 
		id3_field* p_field = NULL;
		p_field = p_frame->fields;
		id3_length_t n_data_size = 0;
		id3_byte_t const* p_binary = NULL;
		
		for (size_t i = 0; i < p_frame->nfields; ++i)
		{
			assert(p_field != NULL);
			switch (p_field->type)
			{
			
			case ID3_FIELD_TYPE_LATIN1 :
				{
					id3_latin1_t const * p_text = id3_field_getlatin1(p_field);
					if (strcmp((const char*)p_text, "xiami") != 0 && strcmp((const char*)p_text, "lbsd") != 0 )
					{
						return false;
					}
				}
				break;
			
			case ID3_FIELD_TYPE_BINARYDATA :
				{
					
					p_binary = id3_field_getbinarydata(p_field, &n_data_size);
					
				}
				break;
			}
			++p_field;
		}
		if (p_binary != NULL && n_data_size > 0)
		{
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
			int n_out_len = 0;
			char *p_out_buf = pk.decrypt( (char*)p_binary,n_data_size,n_out_len);
			if(n_size > 0 && p_buffer != NULL)
			{
				int n_len = n_out_len <n_size ? n_out_len:n_size;
				memcpy(p_buffer,p_out_buf,n_len);
				n_size = n_out_len;
			}
			pk.delete_buf(p_out_buf);	
			return true;
		}
		++n_index;
	}
	return n_rip_count;
	return true;
}
unsigned int id3_v2_tag::get_pic_type(const unsigned char* p_buf, unsigned int n_len)
{
	unsigned int n_type = 0;
	static const unsigned char JPG_BEGIN[2] = { 0xFF, 0xD8 };
	static const unsigned char JPG_END[2] = { 0xFF, 0xD9 };
	static const unsigned char PNG_BEGIN[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0D, 0x1A, 0x0A };
	static const unsigned char PNG_END[12] = { 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82 };
	bool b_right(false);
	if (n_len > 4)
	{
		b_right = true;
		for (int i = 0; i < 2; ++i)
		{
			if (*(p_buf + i) != JPG_BEGIN[i])
			{
				b_right = false;
				break;
			}
		}
		if (b_right)
		{
			for (int i = 0; i < 2; ++i)
			{
				if (*(p_buf + n_len - 2 + i) != JPG_END[i])
				{
					b_right = false;
					break;
				}
			}
		}
		if (b_right)
			return 1;			//jpg
	}
	if (n_len > 20)
	{
		b_right = true;
		for (int i = 0; i < 8; ++i)
		{
			if (*(p_buf + i) != JPG_BEGIN[i])
			{
				b_right = false;
				break;
			}
		}
		if (b_right)
		{
			for (int i = 0; i < 12; ++i)
			{
				if (*(p_buf + n_len - 12 + i) != JPG_END[i])
				{
					b_right = false;
					break;
				}
			}
		}
		if (b_right)
			return 2;			//png
	}
	return n_type;
}

bool id3_v2_tag::attach_inner_frame(mp3_tag_interface* p_source_tag,bool b_encrypt_frame,bool b_pic_frame)
{
	id3_v2_tag* p_source_v2 = dynamic_cast<id3_v2_tag*>(p_source_tag);
	if (p_source_v2 == NULL && p_source_v2->mp_tag == NULL)
		return false;
	assert(mp_tag != NULL);
	struct id3_frame* p_frame = NULL;
	unsigned int n_index = 0;
	int n_result = 0;

	n_result = 0;
	while ((p_frame = id3_tag_findframe(p_source_v2->mp_tag, ID3V2_FRAME_UNSYNC_LYRIC, n_index)) != NULL)
	{
		n_result = id3_tag_attachframe(mp_tag, p_frame);
		assert(n_result == 0);
		++n_index;
	}


	if(b_pic_frame)
	{
		n_result = 0;
		n_index = 0;
		while ((p_frame = id3_tag_findframe(p_source_v2->mp_tag, ID3V2_FRAME_PICTURE, n_index)) != NULL)
		{
			n_result = id3_tag_attachframe(mp_tag, p_frame);
			assert(n_result == 0);
			++n_index;
		}
	}

	if(b_encrypt_frame)
	{
		n_result = 0;
		n_index = 0;
		while ((p_frame = id3_tag_findframe(p_source_v2->mp_tag, ID3V2_FRAME_PRIVATE, n_index)) != NULL)
		{
			n_result = id3_tag_attachframe(mp_tag, p_frame);
			assert(n_result == 0);
			++n_index;
		}
	}
	
	
	return true;
}

bool id3_v2_tag::import(const tag_base_data* p_data)
{
	bool b_result(false);
	int n_result = 0;
	if (p_data != NULL)
	{
		if (mp_tag == NULL)
		{
			mp_tag = id3_tag_new();
			assert(mp_tag != NULL);
			id3_tag_options(mp_tag, ID3_TAG_OPTION_CRC, 0);
			id3_tag_options(mp_tag, ID3_TAG_OPTION_COMPRESSION, 0);
			id3_tag_options(mp_tag, ID3_TAG_OPTION_UNSYNCHRONISATION, 0);

			id3_tag_setlength(mp_tag, TAG_ID3_V2_LEN);			//设置最小长度
		}

		if (!p_data->s_title.empty())
		{
			b_result = add_frame_string(mp_tag, ID3V2_FRAME_TITLE_DESC, p_data->s_title, false);
			assert(b_result);
		}
		if (!p_data->s_album_artist.empty())
		{
			b_result = add_frame_string(mp_tag, ID3V2_FRAME_ORCHESTRA, p_data->s_album_artist, false);
			assert(b_result);
		}
		if (!p_data->s_artist.empty())
		{
			b_result = add_frame_string(mp_tag, ID3V2_FRAME_SOLOIST, p_data->s_artist, false);
			assert(b_result);
		}
		if (!p_data->s_album.empty())
		{
			b_result = add_frame_string(mp_tag, ID3V2_FRAME_AMS_TITLE, p_data->s_album, false);
			assert(b_result);
		}
		if (!p_data->s_date.empty())
		{
			//b_result = add_frame_string(mp_tag, ID3V2_FRAME_RECORDING_TIME, p_data->s_date, false);
			b_result = add_frame_string(mp_tag, ID3V2_FRAME_PUBLISH_YEAR, p_data->s_date, false);
			assert(b_result);
		}
		if (!p_data->s_genre.empty())
		{
			b_result = add_frame_string(mp_tag, ID3V2_FRAME_CONTENT_TYPE, p_data->s_genre, false);
			assert(b_result);
		}
		if (!p_data->s_comment.empty())
		{
			//b_result = add_frame_string(mp_tag, ID3V2_FRAME_COMMENT, p_data->s_comment, false);
			b_result = add_comment_frame(mp_tag, ID3V2_FRAME_COMMENT, p_data->s_comment, false);
			assert(b_result);
		}
		if (p_data->n_track > 0)
		{
			assert(p_data->n_track <= tag_base_data::MAX_SONG_COUNT);
			assert(p_data->n_song_count <= tag_base_data::MAX_SONG_COUNT);
			char s_buf[32];
			if (p_data->n_song_count > 0)
			{
				assert(p_data->n_song_count >= p_data->n_track);
				sprintf(s_buf, "%u/%u", p_data->n_track, p_data->n_song_count);
			}
			else
				sprintf(s_buf, "%u", p_data->n_track);
			b_result = add_frame_string(mp_tag, ID3V2_FRAME_SET_TRACK, string(s_buf), false);
			assert(b_result);
		}
		if (p_data->n_disk_serial > 0)
		{
			assert(p_data->n_disk_serial <= tag_base_data::MAX_DISK_COUNT);
			assert(p_data->n_disk_count <= tag_base_data::MAX_DISK_COUNT);
			char s_buf[32];
			if (p_data->n_disk_count > 0)
			{
				assert(p_data->n_disk_count >= p_data->n_disk_serial);
				sprintf(s_buf, "%u/%u", p_data->n_disk_serial, p_data->n_disk_count);
			}
			else
				sprintf(s_buf, "%u", p_data->n_disk_serial);
			b_result = add_frame_string(mp_tag, ID3V2_FRAME_SET_PART, string(s_buf), false);
			assert(b_result);
		}
	}
	return b_result;
}

bool id3_v2_tag::import_wstring(const tag_base_data_w* p_data)
{
	bool b_result(false);

	int n_result = 0;
	if (p_data != NULL)
	{
		if (mp_tag == NULL)
		{
			mp_tag = id3_tag_new();
			assert(mp_tag != NULL);
			id3_tag_options(mp_tag, ID3_TAG_OPTION_CRC, 0);
			id3_tag_options(mp_tag, ID3_TAG_OPTION_COMPRESSION, 0);
			id3_tag_options(mp_tag, ID3_TAG_OPTION_UNSYNCHRONISATION, 0);

			id3_tag_setlength(mp_tag, TAG_ID3_V2_LEN);			//设置最小长度
		}


		if (!p_data->s_title.empty())
		{
			b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_TITLE_DESC, p_data->s_title, false);
			assert(b_result);
		}
		if (!p_data->s_album_artist.empty())
		{
			b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_ORCHESTRA, p_data->s_album_artist, false);
			assert(b_result);
		}
		if (!p_data->s_artist.empty())
		{
			b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_SOLOIST, p_data->s_artist, false);
			assert(b_result);
		}
		if (!p_data->s_album.empty())
		{
			b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_AMS_TITLE, p_data->s_album, false);
			assert(b_result);
		}
		if (!p_data->s_date.empty())
		{
			//b_result = add_frame_string(mp_tag, ID3V2_FRAME_RECORDING_TIME, p_data->s_date, false);
			b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_PUBLISH_YEAR, p_data->s_date, false);
			assert(b_result);
		}
		if (!p_data->s_genre.empty())
		{
			b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_CONTENT_TYPE, p_data->s_genre, false);
			assert(b_result);
		}
		if (!p_data->s_comment.empty())
		{
			//b_result = add_frame_string(mp_tag, ID3V2_FRAME_COMMENT, p_data->s_comment, false);
			b_result = add_comment_frame_wstring(mp_tag, ID3V2_FRAME_COMMENT, p_data->s_comment, false);
			assert(b_result);
		}
		if (p_data->n_track > 0)
		{
			assert(p_data->n_track <= tag_base_data::MAX_SONG_COUNT);
			assert(p_data->n_song_count <= tag_base_data::MAX_SONG_COUNT);
			wchar_t s_buf[32];
			if (p_data->n_song_count > 0)
			{
				//assert(p_data->n_song_count >= p_data->n_track);
				wsprintfW(s_buf, L"%u/%u", p_data->n_track, p_data->n_song_count);
			}
			else
				wsprintfW(s_buf, L"%u", p_data->n_track);
			b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_SET_TRACK, wstring(s_buf), false);
			assert(b_result);
		}
		if (p_data->n_disk_serial > 0)
		{
			assert(p_data->n_disk_serial <= tag_base_data::MAX_DISK_COUNT);
			assert(p_data->n_disk_count <= tag_base_data::MAX_DISK_COUNT);
			wchar_t s_buf[32];
			if (p_data->n_disk_count > 0)
			{
				//assert(p_data->n_disk_count >= p_data->n_disk_serial);
				wsprintfW(s_buf, L"%u/%u", p_data->n_disk_serial, p_data->n_disk_count);
			}
			else
				wsprintfW(s_buf, L"%u", p_data->n_disk_serial);
			b_result = add_frame_wstring(mp_tag, ID3V2_FRAME_SET_PART, wstring(s_buf), false);
			assert(b_result);
		}
	}
	return b_result;
}
//////////////////////////////////////////////////////////////////////////
id3_v1_factory::id3_v1_factory()
{
}

id3_v1_factory::~id3_v1_factory()
{
}

mp3_tag_interface* id3_v1_factory::create_tag(io_base* p_file, unsigned int n_begin, unsigned int n_end)
{
	int n_result = 0;
	bool b_result(false);
	assert(p_file != NULL);
	if (n_end - n_begin < TAG_ID3_V1_LEN)
		return NULL;
	n_result = p_file->seek(n_end - TAG_ID3_V1_LEN, FILE_BEGIN);
	if (n_result != 0)
		return NULL;
	int n_offset = p_file->get_position();										//当前位置
	unsigned int n_bytes_read = 0;
	unsigned char s_buf[TAG_ID3_V1_LEN];
	n_result = p_file->read(s_buf, TAG_ID3_V1_LEN, &n_bytes_read);		//读128个字节
	if (n_result != 0 || n_bytes_read != TAG_ID3_V1_LEN)
		return NULL;
	n_result = id3_tag_query(s_buf, TAG_ID3_V1_LEN);
	if (n_result != TAG_ID3_V1_LEN)
		return NULL;
	id3_v1_tag* p_tag = new id3_v1_tag;
	b_result = p_tag->init(s_buf, n_offset, TAG_ID3_V1_LEN);
	assert(b_result);
	return p_tag;

}
//////////////////////////////////////////////////////////////////////////
id3_v2_factory::id3_v2_factory()
{
}

id3_v2_factory::~id3_v2_factory()
{
}

mp3_tag_interface* id3_v2_factory::create_tag(io_base* p_file, unsigned int n_begin, unsigned int n_end)
{
	int n_result = 0;
	assert(p_file != NULL);
	if (n_end - n_begin < TAG_ID3_V2_MIN)
		return NULL;
	n_result = p_file->seek(n_begin, FILE_BEGIN);
	if (n_result != 0)
		return NULL;
	int n_offset = p_file->get_position();										//当前位置
	unsigned char s_min_buf[TAG_ID3_V2_MIN];
	unsigned int n_bytes_read = 0;
	n_result = p_file->read(s_min_buf, TAG_ID3_V2_MIN, &n_bytes_read);
	if (n_result != 0 || n_bytes_read != TAG_ID3_V2_MIN)
		return NULL;

	n_result = id3_tag_query(s_min_buf, TAG_ID3_V2_MIN);
	if (n_result > 0)
	{
		int n_length = n_result;
		boost::shared_array<unsigned char> sp_buf(new unsigned char[n_length]);
		n_bytes_read = 0;
		p_file->seek(n_begin, FILE_BEGIN);
		n_result = p_file->read(sp_buf.get(), n_length, &n_bytes_read);
		if (n_result != 0 || n_bytes_read != n_length)
			return NULL;
		id3_v2_tag* p_tag = new id3_v2_tag;
		bool b_result = p_tag->init(sp_buf, n_begin, n_length);
		assert(b_result);
		return p_tag;
	}
	else if (n_result < 0)	//id3 tag v2.4 footer
	{
		assert(false);
	}
	return NULL;
}

}		//end namespace em_mp3_tag
