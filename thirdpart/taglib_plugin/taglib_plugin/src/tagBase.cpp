#include "tagBase.h"
#include <cstdio>
#include <assert.h>
#ifdef _MSC_VER
#include <tchar.h>
#include <Windows.h>
#else
#include <fcntl.h>   
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#endif

std::string wide_to_multi(const wchar_t* c, int code = 65001 /*CP_UTF8*/)
{
	assert(c);
	std::wstring str(c);
#ifdef _MSC_VER
	int size = WideCharToMultiByte(code, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
#else
	size_t size = wcstombs(NULL, str.c_str(), 0);
#endif

	std::string mbstr(size, char(0));
#ifdef _MSC_VER
	WideCharToMultiByte(code, 0, str.c_str(), -1, const_cast<char*>(mbstr.c_str()), size, NULL, NULL);
#else
	wcstombs(const_cast<char*>(mbstr.c_str()), const_cast<wchar_t*>(str.c_str()), (size + 1) * 4);
#endif
	return mbstr;
}

std::wstring multi_to_wide(const char* c, int code = 65001 /*CP_UTF8*/)
{
	assert(c);
	std::string str(c);
#ifdef _MSC_VER
	int size = MultiByteToWideChar(code, 0, str.c_str(), -1, NULL, 0) - 1;
#else
	size_t size = mbstowcs(NULL, str.c_str(), 0);
#endif

	std::wstring wstr(size, wchar_t(0));
#ifdef _MSC_VER
	MultiByteToWideChar(code, 0, str.c_str(), str.length(), const_cast<wchar_t*>(wstr.c_str()), size);
#else
	mbstowcs(const_cast<wchar_t*>(wstr.c_str()), str.c_str(), size);
#endif
	return wstr;
}

static void toStrT(ITag::StrT& sOutStr, const TagLib::String& sInStr)
{
	if (sInStr.isEmpty())
	{
		sOutStr.clear();
		return;
	}

	if (sInStr.isAscii() || sInStr.isLatin1())
	{
		sOutStr = multi_to_wide(sInStr.toCString(), 0);
	}
	else
	{
		sOutStr = multi_to_wide(sInStr.toCString(true));
	}
}

CTagBase::CTagBase()
{}
CTagBase::CTagBase(ITag::TagFileType tagType, spTagFileT tagFile)
	:_tagType(tagType),_tagFile(tagFile)
{
	assert(tagFile);
	TagLib::FileName fName = tagFile->name();
#ifdef _MSC_VER
	toStrT(_sFileName, fName.toString());
#else
	TagLib::String fileName = fName;
	toStrT(_sFileName, fileName);
#endif
}
CTagBase::~CTagBase()
{

}

ITag::TagFileType CTagBase::filetype() const
{
	return _tagType;
}
unsigned long CTagBase::beginpos() const
{
	return 0;
}
unsigned long CTagBase::filesize() const
{
	if (_tagFile)
		return _tagFile->length();
	return 0;
}
const ITag::CharT* CTagBase::filename() const
{
	return _sFileName.c_str();
}
const ITag::CharT* CTagBase::title() const
{
	if (_sTitle.empty() && _tagFile && _tagFile->tag())
	{
		CTagBase* pThis = const_cast<CTagBase*>(this);
		toStrT(pThis->_sTitle, _tagFile->tag()->title());
	}
	return _sTitle.c_str();
}
const ITag::CharT* CTagBase::artist() const
{
	if (_sArtist.empty() && _tagFile && _tagFile->tag())
	{
		CTagBase* pThis = const_cast<CTagBase*>(this);
		toStrT(pThis->_sArtist, _tagFile->tag()->artist());
	}
	return _sArtist.c_str();
}
const ITag::CharT* CTagBase::album() const
{
	if (_sAlbum.empty() && _tagFile)
	{
		CTagBase* pThis = const_cast<CTagBase*>(this);
		toStrT(pThis->_sAlbum, _tagFile->tag()->album());
	}
	return _sAlbum.c_str();
}
const ITag::CharT* CTagBase::comment() const
{
	if (_sComment.empty() && _tagFile && _tagFile->tag())
	{
		CTagBase* pThis = const_cast<CTagBase*>(this);
		toStrT(pThis->_sComment, _tagFile->tag()->comment());
	}
	return _sComment.c_str();
}
const ITag::CharT* CTagBase::genre() const
{
	if (_sGenre.empty() && _tagFile  && _tagFile->tag())
	{
		CTagBase* pThis = const_cast<CTagBase*>(this);
		toStrT(pThis->_sGenre, _tagFile->tag()->genre());
	}
	return _sGenre.c_str();
}
unsigned long CTagBase::year() const
{
	if (_tagFile &&  _tagFile->tag())
		return _tagFile->tag()->year();
	return 0;
}
unsigned long CTagBase::track() const
{
	if (_tagFile  && _tagFile->tag())
		return _tagFile->tag()->track();
	return 0;
}
unsigned long CTagBase::playSec() const
{
	if (_tagFile)
	{
		const TagLib::AudioProperties* audioProperties = _tagFile->audioProperties();
		if (audioProperties)
			return audioProperties->length();
	}
	return 0;
}

unsigned long CTagBase::bitRate() const
{
	if (_tagFile)
	{
		const TagLib::AudioProperties* audioProperties = _tagFile->audioProperties();
		if (audioProperties)
			return audioProperties->bitrate();
	}
	return 0;
}
unsigned long CTagBase::sampleRate() const
{
	if (_tagFile)
	{
		const TagLib::AudioProperties* audioProperties = _tagFile->audioProperties();
		if (audioProperties)
			return audioProperties->sampleRate();
	}
	return 0;
}
unsigned long CTagBase::channels() const
{
	if (_tagFile)
	{
		const TagLib::AudioProperties* audioProperties = _tagFile->audioProperties();
		if (audioProperties)
			return audioProperties->channels();
	}
	return 0;
}

void CTagBase::setTitle(const ITag::CharT* s)
{
	_sTitle = s;
	if (_tagFile  && _tagFile->tag())
	{
		TagLib::String theTitle(s, TagLib::String::UTF8);
		_tagFile->tag()->setTitle(theTitle);
	}
}
void CTagBase::setArtist(const ITag::CharT* s)
{
	_sArtist = s;
	if (_tagFile  && _tagFile->tag())
	{
		TagLib::String theArtist(s, TagLib::String::UTF8);
		_tagFile->tag()->setArtist(theArtist);
	}
}
void CTagBase::setAlbum(const ITag::CharT* s)
{
	_sAlbum = s;
	if (_tagFile  && _tagFile->tag())
	{
		TagLib::String theAlbum(s, TagLib::String::UTF8);
		_tagFile->tag()->setAlbum(theAlbum);
	}
}
void CTagBase::setComment(const ITag::CharT* s)
{
	_sComment = s;
	if (_tagFile  && _tagFile->tag())
	{
		TagLib::String theComment(s, TagLib::String::UTF8);
		_tagFile->tag()->setComment(theComment);
	}
}
void CTagBase::setGenre(const ITag::CharT* s)
{
	_sGenre = s;
	if (_tagFile && _tagFile->tag())
	{
		TagLib::String theGenre(s, TagLib::String::UTF8);
		_tagFile->tag()->setGenre(theGenre);
	}
}
void CTagBase::setYear(unsigned long i)
{
	if (_tagFile  && _tagFile->tag())
	{
		_tagFile->tag()->setYear(i);
	}
}
void CTagBase::setTrack(unsigned long i)
{
	if (_tagFile  && _tagFile->tag())
	{
		_tagFile->tag()->setTrack(i);
	}
}
bool CTagBase::isEmpty() const
{
	return (!_tagFile)
		|| (!_tagFile->tag())
		|| _tagFile->tag()->isEmpty();
}
bool CTagBase::save()
{
	if (!_tagFile)
		return false;
	return _tagFile->save();
}
bool CTagBase::addImage(ITag::CharT* s)
{
	if (!_tagFile) return false;
	TagLib::MPEG::File *pAudioFile = (TagLib::MPEG::File *)_tagFile.get();
	TagLib::String sFileName(s, TagLib::String::UTF8);
	StrT strFileName;
	toStrT(strFileName, sFileName);
	ImageFile imageFile(strFileName.c_str());
	if (!pAudioFile || !pAudioFile->isValid() || !imageFile.isValid())
		return false;
	TagLib::ID3v2::Tag *tag = pAudioFile->ID3v2Tag(true);
	TagLib::ID3v2::AttachedPictureFrame *frame = new TagLib::ID3v2::AttachedPictureFrame;
	frame->setMimeType(imageFile.mimeType());
	frame->setPicture(imageFile.data());
	tag->addFrame(frame);
	return pAudioFile->save();

}
bool CTagBase::exportImage(const ITag::CharT* s)
{
	if (!s)	return false;
	if (_tagFile)
	{
		//get picture
		TagLib::MPEG::File *f = (TagLib::MPEG::File *)_tagFile.get();
		if (!f->hasID3v2Tag() || f->ID3v2Tag()->isEmpty())
			return false;
		/*
		TagLib::ID3v2::FrameList::ConstIterator it = f->ID3v2Tag()->frameList().begin();
		for (; it != f->ID3v2Tag()->frameList().end(); it++)
		{
			if ((*it)->frameID().operator == (TagLib::ByteVector("APIC")))
			{
				HANDLE hFile = CreateFile(s, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (INVALID_HANDLE_VALUE == hFile)
					return false;
				int nWriteBytes = 0;
				size_t size = (*it)->size();
				::WriteFile(hFile, (*it)->render().data(), (*it)->size(), (LPDWORD)&nWriteBytes, NULL);
				::CloseHandle(hFile);
				if (nWriteBytes != size)
					return false;
				return true;
			}
		}*/
		if (f->ID3v2Tag()->frameListMap().size() == 0)
			return false;
		if (f->ID3v2Tag()->frameListMap().find("APIC") == f->ID3v2Tag()->frameListMap().end())
			return false;

		TagLib::ID3v2::FrameList Flist = f->ID3v2Tag()->frameListMap()["APIC"];
		if (Flist.isEmpty())
			return false;
		TagLib::ID3v2::AttachedPictureFrame *p = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(Flist.front());
		size_t size = p->picture().size();
		TagLib::String strPicType = p->mimeType();
		int nPos = strPicType.find('/');
		int nLength = strPicType.size() - nPos - 1;
		TagLib::String strTemp = strPicType.substr(nPos, nLength);

		TagLib::String sFileName(s);
		StrT strFileName;
		toStrT(strFileName, sFileName);
#ifdef _MSC_VER
		HANDLE hFile = CreateFileW(strFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);	
		if (INVALID_HANDLE_VALUE == hFile)
			return false;
		int nWriteBytes = 0;
		::WriteFile(hFile, p->picture().data(), size, (LPDWORD)&nWriteBytes, NULL);
		::CloseHandle(hFile);
		if (nWriteBytes != size)
			return false;
#else
		int file;
		if ((file = open(sFileName.toCString(), O_RDWR)) < 0)
			return false;
		int nWriteBytes = write(file, p->picture().data(), size);
		close(file);
		if (nWriteBytes != size)
			return false;
#endif
		return true;
	}
	return false;
}
unsigned long CTagBase::subTagCnt() const
{
	return 0;
}
ITag* CTagBase::nextSubTag()
{
	return NULL;
}
unsigned long CTagBase::mineType() const
{
	if (!_tagFile)
		return ITag::MineType::UNKONW;
#ifdef _MSC_VER
	ImageFile* pFile = new ImageFile(_tagFile->name());
#else
    ImageFile* pFile = new ImageFile(multi_to_wide(_tagFile->name()).c_str());
#endif
    if (!pFile)
		return ITag::MineType::UNKONW;

	if (pFile->isValid())
	{
		TagLib::String s = pFile->mimeType();

		delete pFile;

		if (s == TagLib::String::null)
		{
			return ITag::MineType::UNKONW;
		}
		else if (s == TagLib::String("image/jpeg"))
		{
			return ITag::MineType::JPEG;
		}
		else if (s == TagLib::String("image/png"))
		{
			return ITag::MineType::PNG;
		}		
	}
	delete pFile;
	return ITag::MineType::UNKONW;
}

#ifdef _MSC_VER
ImageFile::ImageFile(const ITag::CharT* file) : TagLib::File(file)
{
	
}
#else
ImageFile::ImageFile(const ITag::CharT* file) : TagLib::File(wide_to_multi(file).c_str())
{

}
#endif
