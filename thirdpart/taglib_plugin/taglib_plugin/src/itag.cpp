#include "itag.h"
#include "tagFactory.h"
#ifdef _MSC_VER
#include <Windows.h>
#else
#ifndef CP_UTF8
#define CP_UTF8 65001
#endif
#endif
extern std::string wide_to_multi(const wchar_t*, int);
extern std::wstring multi_to_wide(const char*, int);

/************************************************************************/
/*                                                                      */
/************************************************************************/
#ifdef   __cplusplus
extern "C" {
#endif

	TAGLIB_PLUGIN_DLL_API ITag* plugin_load_tag(
		const char* pfile,
		bool readAutioInfo,
		ITag::ReadAudioStyle style,
		int* nerr
		)
	{
		ITag::StrT wpfile = multi_to_wide(pfile, CP_UTF8);
		return CTagFactory::instance().create(wpfile.c_str(),readAutioInfo,style,nerr);
	}
	TAGLIB_PLUGIN_DLL_API ITag* plugin_load_tag_type(
		const char* pfile,
		ITag::TagFileType tagtype, 
		bool readAutioInfo,
		ITag::ReadAudioStyle style, 
		int* nerr)
	{
		ITag::StrT wpfile = multi_to_wide(pfile, CP_UTF8);
		return CTagFactory::instance().create(wpfile.c_str(),tagtype,readAutioInfo,style,nerr);
	}
	TAGLIB_PLUGIN_DLL_API void   plugin_close_tag(ITag* ptag)
	{
		return CTagFactory::instance().close(ptag);
	}

#ifdef   __cplusplus
}
#endif