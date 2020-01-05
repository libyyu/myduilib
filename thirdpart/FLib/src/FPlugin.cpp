#define FLIB_DLL
#include "FPlugin.h"
#include <locale.h>
#include <iostream>
#include <sstream> 
#include <stdarg.h>
#if FLIB_COMPILER_WINDOWS
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

_FStdBegin

FPlugin::FPlugin(bool auto_unload /*= true*/) :_plugin(NULL), _auto_unload(auto_unload) {}
FPlugin::FPlugin(const char* plugin_name, bool auto_unload/* = true*/) :_plugin(NULL), _auto_unload(auto_unload)
{
	LoadPlugin(plugin_name);
}
FPlugin::~FPlugin()
{
	if (_auto_unload)
		Unload();
}
void FPlugin::Unload()
{
	if (!IsLoaded())
		return;
#if FLIB_COMPILER_WINDOWS
	::FreeLibrary((HMODULE)_plugin);
#else
	dlclose(_plugin);
#endif
	_plugin = NULL;
}

bool FPlugin::LoadPlugin(const char* plugin)
{
#if FLIB_COMPILER_WINDOWS
	_plugin = ::LoadLibraryA(plugin);
#else
	_plugin = dlopen(plugin, RTLD_NOW);
#endif		
	return IsLoaded();
}

void* FPlugin::GetSymbol(const char* symbol)
{
	if (!IsLoaded())
	{
		return NULL;
	}
#if FLIB_COMPILER_WINDOWS
	void* const addr = ::GetProcAddress((HMODULE)_plugin, symbol);
#else
	void* const addr = dlsym(_plugin, symbol);
#endif
	return addr;
}

bool FPlugin::Has(const char* symbol_name)
{
	return IsLoaded() && !!GetSymbol(symbol_name);
}

_FStdEnd

