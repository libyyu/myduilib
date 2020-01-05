#pragma once

#ifdef AGENT_STATIC_EXPORT
#define AGENT_DLL_API
#else
#ifdef AGENT_DLL_EXPORT
#define AGENT_DLL_API __declspec(dllexport)
#else
#define AGENT_DLL_API __declspec(dllimport)
#pragma comment(lib,"Agent.lib")
#endif
#endif

#ifdef   __cplusplus
extern "C" {
#endif

	AGENT_DLL_API bool  AsynSendPacket(unsigned int nIndex,const void* buf,size_t cnt);
	AGENT_DLL_API bool  InitAgentClient(int nport);

#ifdef   __cplusplus
}
#endif