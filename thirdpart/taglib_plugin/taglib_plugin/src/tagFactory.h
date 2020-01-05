#pragma once

#include <map>
#include <string>
#include "itag.h"

class CTagFactory
{
	CTagFactory();
public:
	virtual ~CTagFactory();
	static CTagFactory& instance();
public:
	ITag* create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		ITag::ReadAudioStyle style,
		int* nerr);

	ITag* create(
		const ITag::CharT* pfile,
		ITag::TagFileType tagtype,
		bool readAutioInfo,
		ITag::ReadAudioStyle style,
		int* nerr);

	void close(ITag* tag);
protected:
	ITag* defaultCreate(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		ITag::ReadAudioStyle style,
		int* nerr);
protected:
	static ITag* MPEG_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* OGG_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* OGGFLAC_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* FLAC_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* AIFF_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* WAV_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* APE_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* MPC_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* WV_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* SPEEX_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* TRUEAUDIO_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* MP4_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* ASF_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);

	static ITag* CUE_Create(
		const ITag::CharT* pfile,
		bool readAutioInfo,
		int readStyle,
		int* nerr);
protected:
	typedef ITag* (*TagCreateFun)(const ITag::CharT*,bool,int,int*);
	typedef std::map<ITag::StrT, ITag::TagFileType> ExtMapT;
	typedef std::map<ITag::TagFileType,TagCreateFun> CreateFunMapT;

	ExtMapT       _extMap;
	CreateFunMapT _createFunMap;
private:
};