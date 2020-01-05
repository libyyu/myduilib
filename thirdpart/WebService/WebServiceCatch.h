#ifndef WEBSERVICECATCH_H
#define WEBSERVICECATCH_H
#pragma once
#include <string>
#include "std.h"

enum WEBSERVICE_API CatchStrategyType
{
	CST_CATCH         = 1,   //强制catch
	CST_NOCATCH       = 1<<1,//非强制catch
	CST_NET           = 1<<2,//有网络
	CST_NONET         = 1<<3,//无网络
	CST_UPDATA        = 1<<4

};
class WEBSERVICE_API CWebServiceCatch
{
	static int cacheVer;
public:
	bool LoadCache(const std::string& url, std::string& buffer);
	void SaveCache(const std::string& url, const std::string& buffer,int nTimeLen);
private:
	std::string GetFilePath(const std::string& url);
};

#endif//WEBSERVICECATCH_H

