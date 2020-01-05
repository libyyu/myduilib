#pragma once
//#include <fstream>
#include "YMusic.h"
#include "taglib_plugin\taglib_plugin\src\itag.h"

class AudioTagInfo
{
public:
	AudioTagInfo();
	virtual ~AudioTagInfo();
	virtual bool save_pic2file(const std::string& savename);
public:
	virtual void init(const std::string& path);
	virtual std::string title() const;
	virtual std::string artist() const;
	virtual std::string album() const;
	virtual std::string comment() const;
	virtual std::string genre() const;
	virtual unsigned int year() const;
	virtual unsigned int track() const;
	virtual unsigned int length() const;
	virtual unsigned int mineType() const;

	virtual void title(const std::string& s_title);
	virtual void artist(const std::string& s_artist);
	virtual void album(const std::string& s_album);
	virtual void comment(const std::string& s_comment);
	virtual void genre(const std::string& s_genre);
	virtual void year(unsigned int u_year);
	virtual void track(unsigned int u_track);
	virtual void save();
private:
	ITag* _pTag;
};



