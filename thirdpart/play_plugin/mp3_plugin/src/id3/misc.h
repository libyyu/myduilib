#pragma once
#include <time.h>
#include <vector>
#include <string>

namespace em_mp3_tag
{
namespace misc
{
class date_time_helper
{
public:
	static const unsigned short MONTH_OFFSET = 1;
	static const unsigned short DEFAULT_DAY = 15;
	static const unsigned short DEFAULT_MONTH = 6;
	static const unsigned short MIN_YEAR = 1900;
	static const unsigned short MAX_YEAR = 2500;
	static const unsigned short MIN_MONTH = 1;
	static const unsigned short MAX_MONTH = 12;
public:
	static time_t convert_2_time(unsigned short n_year, unsigned short n_month = DEFAULT_MONTH);
};

class string_helper
{
public:
	typedef std::vector<std::string>::iterator split_iter;
	static std::vector<std::string> split_string(const std::string& s_source, const std::string& s_tag);
};

}		//end namespace misc

}		//end namespace em_mp3_tag
