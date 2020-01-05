#ifdef _MSC_VER
#include <tchar.h>
#endif
#include "tagFactory.h"
#include "tagBase.h"
#include "cueTag.h"
#include <assert.h>

CTagFactory& CTagFactory::instance()
{
	static CTagFactory inst;
	return inst;
}

CTagFactory::CTagFactory()
{
	//TagLib::ID3v2::FrameFactory::instance()->setDefaultTextEncoding(TagLib::String::UTF8);

	_createFunMap.insert(std::make_pair(ITag::TAG_MPEG,&CTagFactory::MPEG_Create));
	_createFunMap.insert(std::make_pair(ITag::TAG_OGG,&CTagFactory::OGG_Create));
	_createFunMap.insert(std::make_pair(ITag::TAG_OGGFLAC,&CTagFactory::OGGFLAC_Create));
	_createFunMap.insert(std::make_pair(ITag::TAG_FLAC,&CTagFactory::FLAC_Create));
	_createFunMap.insert(std::make_pair(ITag::TAG_AIFF,&CTagFactory::AIFF_Create));
	_createFunMap.insert(std::make_pair(ITag::TAG_WAV,&CTagFactory::WAV_Create));
	_createFunMap.insert(std::make_pair(ITag::TAG_APE,&CTagFactory::APE_Create));
	_createFunMap.insert(std::make_pair(ITag::TAG_CUE,&CTagFactory::CUE_Create));
	_createFunMap.insert(std::make_pair(ITag::TAG_MPC,&CTagFactory::MPC_Create));
	//_createFunMap.insert(std::make_pair(ITag::TAG_WV,&CTagFactory::WV_Create));
	//_createFunMap.insert(std::make_pair(ITag::TAG_SPEEX,&CTagFactory::SPEEX_Create));
	//_createFunMap.insert(std::make_pair(ITag::TAG_TRUEAUDIO,&CTagFactory::TRUEAUDIO_Create));
	_createFunMap.insert(std::make_pair(ITag::TAG_MP4,&CTagFactory::MP4_Create));
	_createFunMap.insert(std::make_pair(ITag::TAG_ASF,&CTagFactory::ASF_Create));


	_extMap.insert(std::make_pair(TAG_TEXT("MP3"),ITag::TAG_MPEG));
	_extMap.insert(std::make_pair(TAG_TEXT("OGG"),ITag::TAG_OGG));
	_extMap.insert(std::make_pair(TAG_TEXT("OGA"),ITag::TAG_OGGFLAC));
	_extMap.insert(std::make_pair(TAG_TEXT("FLAC"),ITag::TAG_FLAC));
	_extMap.insert(std::make_pair(TAG_TEXT("AIF"),ITag::TAG_AIFF));
	_extMap.insert(std::make_pair(TAG_TEXT("AIFF"),ITag::TAG_AIFF));
	_extMap.insert(std::make_pair(TAG_TEXT("WAV"),ITag::TAG_WAV));
	_extMap.insert(std::make_pair(TAG_TEXT("APE"),ITag::TAG_APE));
	//_extMap.insert(std::make_pair(TAG_TEXT("CUE"),ITag::TAG_CUE));
	_extMap.insert(std::make_pair(TAG_TEXT("MPC"),ITag::TAG_MPC));
	//_extMap.insert(std::make_pair(TAG_TEXT("WV"),ITag::TAG_WV));
	//_extMap.insert(std::make_pair(TAG_TEXT("SPX"),ITag::TAG_SPEEX));
	//_extMap.insert(std::make_pair(TAG_TEXT("TTA"),ITag::TAG_TRUEAUDIO));
	_extMap.insert(std::make_pair(TAG_TEXT("M4A"),ITag::TAG_MP4));
	_extMap.insert(std::make_pair(TAG_TEXT("M4B"),ITag::TAG_MP4));
	_extMap.insert(std::make_pair(TAG_TEXT("M4P"),ITag::TAG_MP4));
	_extMap.insert(std::make_pair(TAG_TEXT("MP4"),ITag::TAG_MP4));
	_extMap.insert(std::make_pair(TAG_TEXT("3G2"),ITag::TAG_MP4));
	_extMap.insert(std::make_pair(TAG_TEXT("WMA"),ITag::TAG_ASF));
	_extMap.insert(std::make_pair(TAG_TEXT("ASF"),ITag::TAG_ASF));
	_extMap.insert(std::make_pair(TAG_TEXT("AAC"),ITag::TAG_APE));
}

CTagFactory::~CTagFactory()
{

}


ITag* CTagFactory::create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	ITag::ReadAudioStyle style,
	int* nerr)
{
	assert(pfile);
	if(!pfile)
		return NULL;

	ITag::StrT s(pfile);
	ITag::StrT::size_type pos = s.rfind(TAG_TEXT("."));
	if(ITag::StrT::npos == pos)
		return NULL;

	ITag::StrT ext(s.substr(pos+1));
	std::transform(ext.begin(),ext.end(),ext.begin(),toupper);
	if(ext.empty())
		return NULL;

	ExtMapT::iterator it = _extMap.find(ext);
	if(_extMap.end() != it)
	{
		return CTagFactory::create(pfile,it->second,readAutioInfo,style,nerr);
	}

	return defaultCreate(pfile,readAutioInfo,style,nerr);
}

ITag* CTagFactory::create(
	const ITag::CharT* pfile,
	ITag::TagFileType tagtype,
	bool readAutioInfo,
	ITag::ReadAudioStyle style,
	int* nerr)
{
	assert(pfile);
	if(!pfile)
		return NULL;


	CreateFunMapT::iterator it = _createFunMap.find(tagtype);
	if(_createFunMap.end() != it)
	{
		assert(it->second);
		ITag* pTag = it->second(pfile,readAutioInfo,(int)style,nerr);
		if(pTag)
		{
 			if(readAutioInfo && (0 >= pTag->playSec()))
 			{
 				plugin_close_tag(pTag);
 				return defaultCreate(pfile,readAutioInfo,style,nerr);
 			}
		
			return pTag;
		}
	}
	return defaultCreate(pfile,readAutioInfo,style,nerr);
}

ITag* CTagFactory::defaultCreate(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	ITag::ReadAudioStyle style,
	int* nerr)
{
	ITag* pTag = NULL;
	for (CreateFunMapT::reverse_iterator iter = _createFunMap.rbegin(),iend = _createFunMap.rend();iter != iend;++iter)
	{
		assert(iter->second);
		ITag* pTmpTag = iter->second(pfile,readAutioInfo,(int)style,nerr);
		if(pTmpTag)
		{
			if(!pTmpTag->isEmpty() && (pTmpTag->playSec() > 0))
			{
				if(pTag)
					plugin_close_tag(pTag);
				return pTmpTag;
			}

			if((pTmpTag->playSec() > 0))
			{
				if(pTag && (0 >= pTag->playSec()))
				{
					plugin_close_tag(pTag);
					pTag = pTmpTag;
					continue;
				}
			}

			if(!pTag)
				pTag = pTmpTag;
			else
				plugin_close_tag(pTmpTag);
		}
	}
	if(!pTag && nerr)
		*nerr = ITag::TAG_NOSUPPORT;
	return pTag;
}

void CTagFactory::close(ITag* tag)
{
	if(tag)
	{
		delete tag;
	}
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef _MSC_VER
	#define TOFILENAME(x) x
#else
	extern std::string wide_to_multi(const wchar_t*, int);
	#define TOFILENAME(x) wide_to_multi(x, 65001).c_str()
#endif
ITag* CTagFactory::MPEG_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo, 
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::MPEG::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
		return NULL;

	return (new CTagBase(ITag::TAG_MPEG,theFile));
}

ITag* CTagFactory::OGG_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::Ogg::Vorbis::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
		return NULL;

	return (new CTagBase(ITag::TAG_OGG,theFile));
}

ITag* CTagFactory::OGGFLAC_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::Ogg::FLAC::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
	{
		return CTagFactory::OGG_Create(pfile,readAutioInfo,readStyle,nerr);
	}

	return (new CTagBase(ITag::TAG_OGGFLAC,theFile));
}

ITag* CTagFactory::FLAC_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::FLAC::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
		return NULL;

	return (new CTagBase(ITag::TAG_FLAC,theFile));
}

ITag* CTagFactory::AIFF_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::RIFF::AIFF::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
		return NULL;

	return (new CTagBase(ITag::TAG_AIFF,theFile));
}

ITag* CTagFactory::WAV_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::RIFF::WAV::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
	{
		theFile = spTagFileT(new TagLib::WavPack::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
		if(!theFile || !theFile->isValid())
		{
			return NULL;
		}
	}

	return (new CTagBase(ITag::TAG_WAV,theFile));
}

ITag* CTagFactory::APE_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::APE::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
		return NULL;

	return (new CTagBase(ITag::TAG_APE,theFile));
}


ITag* CTagFactory::MPC_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::MPC::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
		return NULL;

	return (new CTagBase(ITag::TAG_MPC,theFile));
}

ITag* CTagFactory::WV_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::WavPack::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
		return NULL;

	return (new CTagBase(ITag::TAG_WV,theFile));
}

ITag* CTagFactory::SPEEX_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::Ogg::Speex::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
		return NULL;

	return (new CTagBase(ITag::TAG_SPEEX,theFile));
}

ITag* CTagFactory::TRUEAUDIO_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::TrueAudio::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
		return NULL;

	return (new CTagBase(ITag::TAG_TRUEAUDIO,theFile));
}

ITag* CTagFactory::MP4_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::MP4::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
		return NULL;

	return (new CTagBase(ITag::TAG_MP4,theFile));
}

ITag* CTagFactory::ASF_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
	spTagFileT  theFile(new TagLib::ASF::File(TOFILENAME(pfile),readAutioInfo,(TagLib::AudioProperties::ReadStyle)readStyle));
	if(!theFile || !theFile->isValid())
		return NULL;

	return (new CTagBase(ITag::TAG_ASF,theFile));
}

ITag* CTagFactory::CUE_Create(
	const ITag::CharT* pfile,
	bool readAutioInfo,
	int readStyle,
	int* nerr)
{
// 	spTagFileT  theFile(new TagLib::ASF::File(TOFILENAME(pfile), readAutioInfo, (TagLib::AudioProperties::ReadStyle)readStyle));
// 	if (!theFile || !theFile->isValid())
// 		return NULL;
// 
// 	return (new CTagBase(ITag::TAG_CUE, theFile));
	CCueTag* pTag = new CCueTag();
	if (pTag->loadCurTag(pfile, readAutioInfo, readStyle))
		*nerr = ITag::TagErrCode::TAG_NOERROR;
	else
		*nerr = ITag::TagErrCode::TAG_NOSUPPORT;
	
	return pTag;
}

//int main()
//{
//	int err = 0;
//	ITag* ptag = xiami_load_tag("C:\\Users\\Public\\Music\\Sample Music\\Kalimba.mp3", true, ITag::ReadAudioStyle::Fast, &err);
//	const char* author = ptag->artist();
//	return 0;
//}