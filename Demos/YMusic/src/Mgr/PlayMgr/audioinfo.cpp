#include "StdAfx.h"
#include "audioinfo.h"
#include <fstream>

#ifdef _WIN32
//#include <atlstr.h>
//#include <atlconv.h>
#include <Windows.h>
#endif

void toWStr(std::wstring& sOutStr, std::string& sInStr)
{
	if (sInStr.empty())
	{
		sOutStr.clear();
		return;
	}

	//sOutStr = CA2W(sInStr.c_str(), CP_ACP);

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN64__)
	int size = MultiByteToWideChar(CP_ACP, 0, sInStr.c_str(), -1, NULL, 0) - 1;
#else
	size_t size = mbstowcs(NULL, sInStr.c_str(), 0);
#endif

	std::wstring wstr(size, wchar_t(0));
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN64__)
	MultiByteToWideChar(CP_ACP, 0, sInStr.c_str(), sInStr.length(), const_cast<wchar_t*>(wstr.c_str()), size);
#else
	mbstowcs(const_cast<wchar_t*>(wstr.c_str()), sInStr.c_str(), size);
#endif
	sOutStr = wstr;
}
void toMStr(std::string& sOutStr, std::wstring& sInStr)
{
	if (sInStr.empty())
	{
		sOutStr.clear();
		return;
	}
	//sOutStr = CW2A(sInStr.c_str(), CP_ACP);

	setlocale(LC_ALL, "");
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN64__)
	int size = WideCharToMultiByte(CP_ACP, 0, sInStr.c_str(), -1, NULL, 0, NULL, NULL);
#else
	size_t size = wcstombs(NULL, sInStr.c_str(), 0);
#endif

	std::string mbstr(size, char(0));
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(_WIN64) || defined(WIN64) || defined(__WIN64__)
	WideCharToMultiByte(CP_ACP, 0, sInStr.c_str(), -1, const_cast<char*>(mbstr.c_str()), size, NULL, NULL);
#else
	wcstombs(const_cast<char*>(mbstr.c_str()), const_cast<wchar_t*>(sInStr.c_str()), (size + 1) * 4);
#endif
	sOutStr = mbstr;
}

AudioTagInfo::AudioTagInfo()
{
	_pTag = NULL;
}
AudioTagInfo::~AudioTagInfo()
{
	if (_pTag)
	{
		plugin_close_tag(_pTag);
	}
	_pTag = NULL;
}

bool AudioTagInfo::save_pic2file(const std::string& savename)
{
	if (_pTag)
	{
		std::wstring str;
		toWStr(str, std::string(savename.c_str()));
		return _pTag->exportImage(str.c_str());
	}
	return false;
}

void AudioTagInfo::init(const std::string& path)
{
	//std::wstring str;
	//toWStr(str, std::string(path.c_str()));
	int err = 0;
	_pTag = plugin_load_tag(path.c_str(), true, ITag::ReadAudioStyle::Average, &err);
	assert(_pTag);
}
std::string AudioTagInfo::title() const
{
	if (_pTag)
	{
		string s_return;
		toMStr(s_return, wstring(_pTag->title()));
		return s_return;
	}
	return "";
}
std::string AudioTagInfo::artist() const
{
	if (_pTag)
	{
		string s_return;
		toMStr(s_return, wstring(_pTag->artist()));
		return s_return;
	}
	return "";
}
std::string AudioTagInfo::album() const
{
	if (_pTag)
	{
		string s_return;
		toMStr(s_return, wstring(_pTag->album()));
		return s_return;
	}
	return "";
}
std::string AudioTagInfo::comment() const
{
	if (_pTag)
	{
		string s_return;
		toMStr(s_return, wstring(_pTag->comment()));
		return s_return;
	}
	return "";
}
std::string AudioTagInfo::genre() const
{
	if (_pTag)
	{
		string s_return;
		toMStr(s_return, wstring(_pTag->genre()));
		return s_return;
	}
	return "";
}
unsigned int AudioTagInfo::track() const
{
	if (_pTag)
	{
		return _pTag->track();
	}
	return 0;
}
unsigned int AudioTagInfo::year() const
{
	if (_pTag)
	{
		return _pTag->year();
	}
	return 0;
}
unsigned int AudioTagInfo::length() const
{
	if (_pTag)
	{
		return _pTag->filesize();
	}
	return 0;
}

unsigned int AudioTagInfo::mineType() const
{
	if (_pTag)
	{
		return _pTag->mineType();
	}
	return ITag::MineType::UNKONW;
}

void AudioTagInfo::title(const std::string& s_title)
{
	if (_pTag)
	{
		std::wstring str;
		toWStr(str, std::string(s_title.c_str()));
		_pTag->setTitle(str.c_str());
	}
}
void AudioTagInfo::artist(const std::string& s_artist)
{
	if (_pTag)
	{
		std::wstring str;
		toWStr(str, std::string(s_artist.c_str()));
		_pTag->setArtist(str.c_str());
	}
}
void AudioTagInfo::album(const std::string& s_album)
{
	if (_pTag)
	{
		std::wstring str;
		toWStr(str, std::string(s_album.c_str()));
		_pTag->setAlbum(str.c_str());
	}
}
void AudioTagInfo::comment(const std::string& s_comment)
{
	if (_pTag)
	{
		std::wstring str;
		toWStr(str, std::string(s_comment.c_str()));
		_pTag->setComment(str.c_str());
	}
}
void AudioTagInfo::genre(const std::string& s_genre)
{
	if (_pTag)
	{
		std::wstring str;
		toWStr(str, std::string(s_genre.c_str()));
		_pTag->setGenre(str.c_str());
	}
}
void AudioTagInfo::year(unsigned int u_year)
{
	if (_pTag)
	{
		_pTag->setYear(u_year);
	}
}
void AudioTagInfo::track(unsigned int u_track)
{
	if (_pTag)
	{
		_pTag->setTrack(u_track);
	}
}
void AudioTagInfo::save()
{
	if (_pTag)
	{
		_pTag->save();
	}
}
