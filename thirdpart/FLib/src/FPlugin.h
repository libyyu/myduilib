#ifndef __FLIB_PLUGIN_H__
#define __FLIB_PLUGIN_H__
#pragma once
#include "FType.h"

_FStdBegin

class F_DLL_API FPlugin
{
public:
	typedef void* plugin_t;

	FPlugin(bool auto_unload = true);
	FPlugin(const char* plugin_name, bool auto_unload = true);
	~FPlugin();
	void Unload();
	bool LoadPlugin(const char* plugin);

	void* GetSymbol(const char* symbol);

	bool Has(const char* symbol_name);

	template <typename FUNC>
	inline FUNC Get(const char* symbol_name)
	{
		void* addr = GetSymbol(symbol_name);
		if (!addr)
		{
			return NULL;
		}
		return (FUNC)(addr);
	}

	template <typename R, typename ... Args>
	inline R Call(const char* symbol_name, const Args &... args)
	{
		typedef R(*function_t)(Args...);
		function_t function_ = Get<function_t>(symbol_name);
		if(!function_) return R();
		return function_(args...);
	}

	inline bool IsLoaded() const
	{
		return _plugin != NULL;
	}

	operator plugin_t() const
	{
		return _plugin;
	}
protected:
private:
	plugin_t _plugin;
	bool _auto_unload;
};


_FStdEnd


#endif//__FLIB_PLUGIN_H__
