#include "cueTag.h"
#include <cassert>
#include <fstream>

bool CCueTag::loadCurTag(
	const CharT* pfile,
	bool readAutioInfo,
	int readStyle)
{
	assert(pfile);
#ifdef _MSC_VER
	std::wifstream fs(pfile);
#else
	extern std::string wide_to_multi(const wchar_t*, int);
	std::string ss = wide_to_multi(pfile, 65001);
	std::wifstream fs(ss.c_str());
#endif
	if(!fs.is_open())
		return false;

	const StrT artistKey(L"PERFORMER \"");
	const StrT titleKey(L"TITLE \"");
	const StrT apefileKey(L"FILE \"");
	const CharT      endKey = L'\"';

	bool isParseArtits    = false;
	bool isParseTitle     = false;
	bool isParseApeFile   = false;
	while (!fs.eof())
	{
		StrT inbuf;
		std::getline(fs,inbuf);
		if(inbuf.empty())
			continue;

		if(!isParseArtits)
		{
			StrT::size_type b = inbuf.find(artistKey) + artistKey.size();
			if(StrT::npos != b)
			{
				StrT::size_type e = inbuf.rfind(endKey) - sizeof(endKey)*sizeof(char);
				if(StrT::npos != e && e >= b)
				{
					_sArtist = inbuf.substr(b,e);

					//sprintf("aritst:%s\n",_sArtist.c_str());
					isParseArtits = true;
					continue;
				}
			}
		}

		if(!isParseTitle)
		{
			StrT::size_type b = inbuf.find(titleKey) + titleKey.size();
			if(StrT::npos != b)
			{
				StrT::size_type e = inbuf.rfind(endKey);
				if(StrT::npos != e && e >= b)
				{
					_sTitle = inbuf.substr(b,e);
					isParseTitle = true;
					continue;
				}
			}
		}

		if(!isParseApeFile)
		{
			StrT::size_type b = inbuf.find(apefileKey) + apefileKey.size();
			if(StrT::npos != b)
			{
				StrT::size_type e = inbuf.rfind(endKey);
				if(StrT::npos != e && e >= b)
				{
					StrT apeFile;
					apeFile = inbuf.substr(b,e);
					isParseApeFile = true;

					StrT filename(pfile);
					std::replace(filename.begin(),filename.end(), L'\\', L'/');
					
					StrT::size_type pos = filename.rfind(L'/');
					apeFile = filename.substr(0,pos+1) + apeFile;
					continue;
				}
			}
		}
	}


	return true;
}
