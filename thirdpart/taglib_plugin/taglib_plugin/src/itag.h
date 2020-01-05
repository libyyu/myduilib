#pragma once
#include <string>
#ifdef _MSC_VER
#ifdef TAGLIB_PLUGIN_DLL_EXPORT
#define TAGLIB_PLUGIN_DLL_API __declspec(dllexport)
#else
#define TAGLIB_PLUGIN_DLL_API __declspec(dllimport)
#endif
#pragma warning (disable : 4511) // copy operator could not be generated
#pragma warning (disable : 4512) // assignment operator could not be generated
#pragma warning (disable : 4702) // unreachable code (bugs in Microsoft's STL)
#pragma warning (disable : 4786) // identifier was truncated
#pragma warning (disable : 4996) // function or variable may be unsafe (deprecated)
#pragma warning (disable : 4302)
#pragma warning (disable : 4251)
#pragma warning (disable : 4217)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS // eliminate deprecation warnings for VS2005
#endif
#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE
#endif
#else
#define TAGLIB_PLUGIN_DLL_API
#endif

#ifndef TAG_TEXT
#define TAG_TEXT(x) L##x
#endif

class ITag
{
public:
	typedef std::wstring StrT;
	typedef wchar_t CharT;
	enum TagFileType{
		TAG_MPEG =0,        //.MP3 
		TAG_OGG,            //.OGG
		TAG_OGGFLAC,        //.OGA
		TAG_FLAC,           //.FLAC
		TAG_AIFF,           //.AIF | .AIFF
		TAG_WAV,            //.WAV
		TAG_APE,            //.APE
		TAG_CUE,			//.CUE
		TAG_MPC,            //.MPC
		TAG_WV,             //.WV
		TAG_SPEEX,          //.SPX
		TAG_TRUEAUDIO,      //.TTA

		TAG_MP4,            //.M4A | .M4B | .M4P | .MP4 | .3G2 | .AAC
		TAG_ASF             //.WMA | .ASF
	};
	enum ReadAudioStyle {
		//! Read as little of the file as possible
		Fast = 0,
		//! Read more of the file and make better values guesses
		Average,
		//! Read as much of the file as needed to report accurate values
		Accurate
	};
	enum TagErrCode {
		TAG_NOERROR = 0,
		TAG_NOSUPPORT
	};
	enum MineType {
		UNKONW = 0,
		JPEG,
		PNG
	};
public:
	virtual ~ITag(){};

	virtual TagFileType filetype() const = 0;
	virtual unsigned long beginpos() const = 0;
	virtual unsigned long filesize() const = 0;
	virtual const CharT* filename() const = 0;
    virtual const CharT* title() const = 0;
    virtual const CharT* artist() const = 0;
    virtual const CharT* album() const = 0;
    virtual const CharT* comment() const = 0;
    virtual const CharT* genre() const = 0;
    virtual unsigned long year() const = 0;
    virtual unsigned long track() const = 0;
	virtual unsigned long playSec() const = 0;
	virtual unsigned long bitRate() const = 0;
	virtual unsigned long sampleRate() const = 0;
	virtual unsigned long channels() const = 0;
	virtual unsigned long mineType() const = 0;
    virtual void setTitle(const CharT* s) = 0;
    virtual void setArtist(const CharT* s) = 0;
    virtual void setAlbum(const CharT* s) = 0;
    virtual void setComment(const CharT* s) = 0;
    virtual void setGenre(const CharT* s) = 0;
	virtual void setYear(unsigned long i) = 0;
	virtual void setTrack(unsigned long i) = 0;
	virtual bool save() = 0;
	virtual bool addImage(CharT* s) = 0;
	virtual bool exportImage(const CharT* s) = 0;
	virtual bool isEmpty() const = 0;
	virtual unsigned long subTagCnt() const = 0;
	virtual ITag* nextSubTag()  = 0;

protected:
	ITag(){};
private:
	ITag(const ITag &){};
	ITag &operator=(const ITag &){return *this;};
};


#ifdef   __cplusplus
extern "C" {
#endif

	TAGLIB_PLUGIN_DLL_API ITag*  plugin_load_tag(const char* pfile,
		                                bool readAutioInfo,
										ITag::ReadAudioStyle style,
		                                int* nerr
										);
	TAGLIB_PLUGIN_DLL_API ITag* plugin_load_tag_type(const char* pfile,
		                                    ITag::TagFileType tagtype,
											bool readAutioInfo,
											ITag::ReadAudioStyle style,
											int* nerr);
	TAGLIB_PLUGIN_DLL_API void  plugin_close_tag(ITag* ptag);

#ifdef   __cplusplus
}
#endif