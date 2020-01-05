#pragma once
#include "decoder_plugin.h"
#include <map>
#include <set>

class CXiamiDecoderPluginMgr
{
public:
	CXiamiDecoderPluginMgr(void);
	~CXiamiDecoderPluginMgr(void);

	int load_plugins();
	decoder_handle open(const char *tsz_file_name,bool b_is_online ,decoder_plugin **pp_decoder, int nFileType = 0,int nBegin = 0,int nEnd = 0);

private :
	typedef std::multimap<int,decoder_plugin*> decoder_plugins_t;
	decoder_plugins_t m_decoder_plugins;
	std::set<void*> decoder_plugin_lists;
};
