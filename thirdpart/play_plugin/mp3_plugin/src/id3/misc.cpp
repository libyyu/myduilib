#include "misc.h"

#include <string>
#include <vector>
#include <cassert>
#include <boost/lexical_cast.hpp>

using namespace std;

namespace em_mp3_tag
{
namespace misc
{

time_t date_time_helper::convert_2_time(unsigned short n_year, unsigned short n_month)
{
	assert(n_year >= MIN_YEAR && n_year <= MAX_YEAR);
	assert(n_month >= MIN_MONTH && n_month <= MAX_MONTH);
	struct tm when;
	when.tm_year = n_year - MIN_YEAR;
	when.tm_mon = n_month - MONTH_OFFSET;
	when.tm_mday = DEFAULT_DAY;
	when.tm_hour = 0;
	when.tm_min = 0;
	when.tm_sec = 0;
	time_t n_time = mktime(&when);
	//assert(sizeof(time_t) == sizeof(int));
//	assert(n_time >= 0);
	return n_time;
}

//////////////////////////////////////////////////////////////////////////
vector<string> string_helper::split_string(const string& s_source, const string& s_tag)
{
	vector<string> result;
	string::size_type n_pos;
	string::size_type n_begin = 0;
	string s_value;
	while (true)
	{
		n_pos = s_source.find(s_tag, n_begin);
		if (n_pos == string::npos)			//ц╩спур╣╫
		{
			s_value = s_source.substr(n_begin, string::npos);
			result.push_back(s_value);
			break;
		}
		else
		{
			s_value = s_source.substr(n_begin, n_pos);
			n_begin = n_pos + s_tag.length();
			result.push_back(s_value);
			if (n_begin >= s_source.length())
				break;
		}
	}
	return result;
}

}		//end namespace misc

}		//end namespace em_mp3_tag
