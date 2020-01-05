#ifndef WEBSERVICESTD_H
#define WEBSERVICESTD_H
#pragma once
#include <stddef.h>

#ifdef WEBSERVICE_STATIC_EXPORT
#	define NETWORK_API
#else
#	ifdef _MSC_VER
#		ifdef WEBSERVICE_DLL_EXPORT
#			define WEBSERVICE_API __declspec(dllexport)
#		else
#			define WEBSERVICE_API __declspec(dllimport)
#			pragma comment(lib,"webservice.lib")
#		endif
#	else
#		define WEBSERVICE_API
#	endif
#endif


#ifdef _MSC_VER
//warning C4275: non dll-interface class 'stdext::exception' used as base for dll-interface class 'std::bad_cast'
#pragma warning(disable : 4275)
//warning C4530: C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
#pragma warning(disable : 4530)
//warning C4251: 'behaviac::VariableRegistry::m_proxyHolders' : class 'behaviac::vector<T>' needs to have dll-interface to be used
//by clients of class 'behaviac::VariableRegistry'
#pragma warning(disable : 4251)
//unreferenced formal parameter
#pragma warning(disable : 4100)
#pragma warning(disable : 4127) // conditional expression is constant
//warning C4702: unreachable code
#pragma warning(disable : 4702)
//'strcpy': This function or variable may be unsafe.
#pragma warning(disable : 4996)
#pragma warning(disable : 4595)
#pragma warning(disable : 4819)
#endif//FLIB_COMPILER_MSVC


#endif//WEBSERVICESTD_H