#ifdef _WIN32
#	include <winsock2.h>
#	include <windows.h>
#endif
#include "DecoderPluginMgr.h"
#include "utility/log.h"

CXiamiDecoderPluginMgr::CXiamiDecoderPluginMgr(void)
{

}

CXiamiDecoderPluginMgr::~CXiamiDecoderPluginMgr(void)
{
	for (auto& plugin : decoder_plugin_lists)
	{
#if FLIB_COMPILER_WINDOWS
		::FreeLibrary((HMODULE)plugin);
#else
		dlclose(plugin);
#endif
	}
	decoder_plugin_lists.clear();
}

int CXiamiDecoderPluginMgr::load_plugins()
{
	m_decoder_plugins.clear();

	const char* module_path = _FStd(FGetModulePath)();
	std::string module_dir = _FStd(FGetDirectory)(module_path);
	_FStd(FUnitPath)(module_dir);
	std::string s_plugins_dir = _FStd(FJoinPath)(module_dir, "plugins");

	int ret = _FStd(FGetAllFiles)(s_plugins_dir.c_str(), false, [this, &s_plugins_dir](const char* path, bool isdir)->void
	{
		if (!isdir && _FStd(FGetExtension)(path) == "dll" )
		{
			std::string filename = _FStd(FGetFilenameNoExtension)(path);
			_FStd(FPlugin) plugin(false);
			if (!plugin.LoadPlugin(path))
			{
#if FLIB_COMPILER_WINDOWS
				ulong error = ::GetLastError();
#else
				ulong error = errno();
#endif
				sLogError("%s加载失败，错误代码:%lu", filename.c_str(), error);
				return;
			}

			sLog("LoadLibrary成功：%s", filename.c_str());

			GET_DECODER_PROC proc = plugin.Get<GET_DECODER_PROC>("get_xiami_decoder_proc");
			if (proc == NULL)
			{
				sLog("<%s>未找到函数：get_decoder_proc", filename.c_str());
				plugin.Unload();
				return;
			}
			struct decoder_plugin* p_decoder = (struct decoder_plugin*)proc();
			if (p_decoder != NULL && p_decoder->n_size == sizeof(struct decoder_plugin))
			{
				m_decoder_plugins.insert(std::make_pair(p_decoder->get_decoder_type(), p_decoder));
				//sLog("<%s>函数get_decoder_proc添加到集合succeed",s_file_name.GetBuffer());
			}

			decoder_plugin_lists.insert((void*)(_FStd(FPlugin)::plugin_t)plugin);
		}
	});

	assert(!m_decoder_plugins.empty());
	return m_decoder_plugins.size();
}	

decoder_handle CXiamiDecoderPluginMgr::open(const char *tsz_file_name,bool b_is_online ,decoder_plugin **pp_decoder,int nFileType,int nBegin,int nEnd)
{
	std::string s_file_name = tsz_file_name;
	struct decoder_plugin *p_decoder;
	decoder_handle  decoder_handle = NULL;
	decoder_plugins_t::iterator iter;
	iter = m_decoder_plugins.find(nFileType);
	if(iter != m_decoder_plugins.end())
	{
		p_decoder = iter->second;
		if(p_decoder)
			decoder_handle = p_decoder->open(s_file_name.c_str(),b_is_online,nFileType,nBegin,nEnd);
		if(decoder_handle)
		{
			sLog("成功打开文件：%s nFileType=%d", tsz_file_name, nFileType);
			*pp_decoder = p_decoder;
			return decoder_handle;
		}
	}

	decoder_plugins_t::reverse_iterator r_iter;
	for (r_iter = m_decoder_plugins.rbegin();r_iter != m_decoder_plugins.rend();r_iter++)
	{
		p_decoder = r_iter->second;
		if(!p_decoder) continue;
		decoder_handle = p_decoder->open(s_file_name.c_str(),b_is_online,nFileType,nBegin,nEnd);
		if(!decoder_handle) continue;
		*pp_decoder = p_decoder;
		sLog("成功打开文件：%s, nFileType=%d", tsz_file_name, nFileType);
		return decoder_handle;
	}
	sLogError("打开文件失败：%s, nFileType=%d", tsz_file_name, nFileType);
	return NULL;
	
}
