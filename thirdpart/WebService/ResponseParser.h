#pragma once
#include <string>
#include <map>
#include <functional>
#include "std.h"

class CResponseParser
{
public:
	typedef std::function<void*(unsigned char*, size_t)> HandlerFuncT;
	CResponseParser();
	void* Parse(unsigned int RequestType, unsigned char* pbuffer, size_t bytes);
	void AddHandler(unsigned int RequestType, HandlerFuncT pFunc);
protected:
	typedef std::map<unsigned int, HandlerFuncT> ParseFuncMapT;
	typedef ParseFuncMapT::iterator ParseFuncMapIterT;

	ParseFuncMapT m_ParseFuncMap;
};
