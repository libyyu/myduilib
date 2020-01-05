#include "tag_base.h"
#include <time.h>
#include <boost/lexical_cast.hpp>
#include "misc.h"
#include <string>
#include "All.h"
#include "CharacterHelper.h"

using namespace std;

namespace em_mp3_tag
{

const char* const_enum_map::ma_mpeg_versions[] = { ("MPEG 2.5"), (""), ("MPEG 2"), ("MPEG 1") };
const char* const_enum_map::ma_mpeg_layers[] = { ("Layer I"), ("Layer II"), ("Layer III") };
const char* const_enum_map::ma_channel_modes[] = { ("Stereo"), ("Joint Stereo"), ("Dual Channel"), ("Single Channel") };
const char* const_enum_map::ma_emphasis[] = { ("None"), ("50/15ms"), (""), ("CCIT J.17") };
const char* const_enum_map::ma_tags[] = { ("id3_tag_v1"), ("id3_tag_v2"), ("ape tag"), ("lyric3 tag") };
const char* const_enum_map::ma_check_results[] = { ("CRC check error"), ("sample change"), ("layer change"), ("version change"),
	("frame offset error"), ("last frame truncated"), ("free space found end"), ("seek first frame failed"), ("success") };


//////////////////////////////////////////////////////////////////////////
void tag_base_data::clear()
{
	s_artist = "";
	s_album = "";
	s_title = "";
	s_date = "";
	n_track = 0;
	s_genre = "";
	n_genre_index = 0;
	s_album_artist = "";
	n_disk_serial = 0;
	n_disk_count = 0;
	n_gmt_publish = 0;
	n_song_count = 0;
}

void tag_base_data::generate_gmt_publish()
{
	if (s_date.empty())
		return;
	if (s_date.length() == 4)			//"2006"
	{
		unsigned short n_year = 0;
		try
		{
			n_year = boost::lexical_cast<unsigned short>(s_date);
		}
		catch (...)
		{
		}

		if (n_year >= misc::date_time_helper::MIN_YEAR && n_year <= misc::date_time_helper::MAX_YEAR)
		{
			time_t n_time = misc::date_time_helper::convert_2_time(n_year);
			n_gmt_publish = (unsigned int)n_time;
		}
	}
	else if (s_date.length() == 6)		//"200602"
	{
		string s_year = s_date.substr(0, 4);
		string s_month = s_date.substr(4, 2);
		unsigned short n_year = 0;
		unsigned short n_month = 0;
		try
		{
			n_year = boost::lexical_cast<unsigned short>(s_year);
		}
		catch (...) 
		{
		}
		try
		{
			n_month =boost::lexical_cast<unsigned short>(s_month);
		}
		catch (...)
		{
		}
		if (n_year >= misc::date_time_helper::MIN_YEAR && n_year <= misc::date_time_helper::MAX_YEAR 
			&& n_month >= misc::date_time_helper::MIN_MONTH && n_month <= misc::date_time_helper::MAX_MONTH)
		{
			time_t n_time = misc::date_time_helper::convert_2_time(n_year, n_month);
			n_gmt_publish = (unsigned int)n_time;
		}
	}	
	else					//check "YYYY.MM" or "YYYY.M" format
	{
		vector<string> items = misc::string_helper::split_string(s_date, ".");
		if (items.size() == 2)
		{
			unsigned short n_year = 0;
			unsigned short n_month = 0;
			try
			{
				n_year = boost::lexical_cast<unsigned short>(items[0]);
			}
			catch (...)
			{
			}
			try
			{
				n_month = boost::lexical_cast<unsigned short>(items[1]);
			}
			catch (...)
			{
			}
			if (n_year >= misc::date_time_helper::MIN_YEAR && n_year <= misc::date_time_helper::MAX_YEAR 
				&& n_month >= misc::date_time_helper::MIN_MONTH && n_month <= misc::date_time_helper::MAX_MONTH)
			{
				time_t n_time = misc::date_time_helper::convert_2_time(n_year, n_month);
				n_gmt_publish = (unsigned int)n_time;
			}
		}
	}
}

void tag_base_data_w::clear()
{
	s_artist = L"";
	s_album = L"";
	s_title = L"";
	s_date = L"";
	n_track = 0;
	s_genre = L"";
	n_genre_index = 0;
	s_album_artist = L"";
	n_disk_serial = 0;
	n_disk_count = 0;
	n_gmt_publish = 0;
	n_song_count = 0;
}

void tag_base_data_w::generate_gmt_publish()
{
	em_mp3_tag::str_utf8* ss = em_mp3_tag::GetUTF8FromUTF16(s_date.c_str());
	string ss_date = (const char*)ss;
	delete[]ss;

	if (s_date.empty())
		return;
	if (s_date.length() == 4)			//"2006"
	{
		unsigned short n_year = 0;
		try
		{
			n_year = boost::lexical_cast<unsigned short>(s_date);
		}
		catch (...)
		{
		}

		if (n_year >= misc::date_time_helper::MIN_YEAR && n_year <= misc::date_time_helper::MAX_YEAR)
		{
			time_t n_time = misc::date_time_helper::convert_2_time(n_year);
			n_gmt_publish = (unsigned int)n_time;
		}
	}
	else if (s_date.length() == 6)		//"200602"
	{
#ifdef _WIN32
		string s_year = (CW2A(s_date.substr(0, 4).c_str())).m_szBuffer;
		string s_month = (CW2A(s_date.substr(4, 2).c_str())).m_szBuffer;
#else
		string s_year = ss_date.substr(0, 4);
		string s_month = ss_date.substr(4, 2);
#endif		
		unsigned short n_year = 0;
		unsigned short n_month = 0;
		try
		{
			n_year = boost::lexical_cast<unsigned short>(s_year);
		}
		catch (...) 
		{
		}
		try
		{
			n_month =boost::lexical_cast<unsigned short>(s_month);
		}
		catch (...)
		{
		}
		if (n_year >= misc::date_time_helper::MIN_YEAR && n_year <= misc::date_time_helper::MAX_YEAR 
			&& n_month >= misc::date_time_helper::MIN_MONTH && n_month <= misc::date_time_helper::MAX_MONTH)
		{
			time_t n_time = misc::date_time_helper::convert_2_time(n_year, n_month);
			n_gmt_publish = (unsigned int)n_time;
		}
	}	
	else					//check "YYYY.MM" or "YYYY.M" format
	{
		vector<string> items = misc::string_helper::split_string(ss_date, ".");
		if (items.size() == 2)
		{
			unsigned short n_year = 0;
			unsigned short n_month = 0;
			try
			{
				n_year = boost::lexical_cast<unsigned short>(items[0]);
			}
			catch (...)
			{
			}
			try
			{
				n_month = boost::lexical_cast<unsigned short>(items[1]);
			}
			catch (...)
			{
			}
			if (n_year >= misc::date_time_helper::MIN_YEAR && n_year <= misc::date_time_helper::MAX_YEAR 
				&& n_month >= misc::date_time_helper::MIN_MONTH && n_month <= misc::date_time_helper::MAX_MONTH)
			{
				time_t n_time = misc::date_time_helper::convert_2_time(n_year, n_month);
				n_gmt_publish = (unsigned int)n_time;
			}
		}
	}
}
}		//end namespace em_mp3_tag
