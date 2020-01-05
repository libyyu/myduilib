#include "ResponseParser.h"
#include <assert.h>

CResponseParser::CResponseParser()
{
}

void* CResponseParser::Parse(unsigned int RequestType, unsigned char* pbuffer, size_t bytes)
{
	ParseFuncMapIterT iter = m_ParseFuncMap.find(RequestType);
	if(iter != m_ParseFuncMap.end())
	{
		HandlerFuncT pFunc = iter->second;
		if(pFunc)
		{
			return pFunc(pbuffer, bytes);
		}
	}
	return NULL;
}

void CResponseParser::AddHandler(unsigned int RequestType, HandlerFuncT pFunc)
{
	ParseFuncMapIterT iter = m_ParseFuncMap.find(RequestType);
	assert(iter == m_ParseFuncMap.end() && pFunc);
	m_ParseFuncMap.insert(std::pair<unsigned int, HandlerFuncT>(RequestType, pFunc));
}
