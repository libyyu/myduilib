#ifndef _DECODER_TYPE_
#define _DECODER_TYPE_
#pragma once

enum Decoder_Type{
	DT_MPEG =0,        //.MP3 
	DT_OGG,            //.OGG
	DT_OGGFLAC,        //.OGA
	DT_FLAC,           //.FLAC
	DT_AIFF,           //.AIF | .AIFF
	DT_WAV,            //.WAV
	DT_APE,            //.APE
	DT_MPC,            //.MPC
	DT_WV,             //.WV
	DT_SPEEX,          //.SPX
	DT_TRUEAUDIO,      //.TTA
	DT_MP4,            //.M4A | .M4B | .M4P | .MP4 | .3G2 |.AAC
	DT_ASF             //.WMA | .ASF
};

#ifndef _MSC_VER
typedef void* HANDLE;
typedef long long               __int64;
typedef unsigned long long               __uint64;
#define INVALID_HANDLE_VALUE -1
#endif

#endif//_DECODER_TYPE_