#include <time.h>
#include <assert.h>
#ifdef _WIN32
#	include <winsock2.h>
#	include <windows.h>
#endif
#include "WebServiceCatch.h"
#include "flib.h"
#include "md5_checksum.h"
int CWebServiceCatch::cacheVer = 1;

bool CWebServiceCatch::LoadCache(const std::string& url, std::string& buffer)
{
	std::string sFilePath = GetFilePath(url);
	FILE *fp = fopen(sFilePath.c_str(),"rb");
	if (!fp) return false;
	int version;
	time_t nTime;

	fread(&version,1,sizeof(int),fp);
	fread(&nTime,1,sizeof(time_t),fp);
	time_t nCurTime =  time(NULL);

	if(nCurTime >= nTime || cacheVer != version)
	{
		fclose(fp);
		return false;
	}
	_FStd(FBuffer) fbuffer;
	while (!feof(fp))
	{
		if (fbuffer.full())
			fbuffer.resize();
		int n = fread(fbuffer[fbuffer.wpos()], 1, fbuffer.nextwriteblocksize(), fp);
		fbuffer.wpos(fbuffer.wpos() + n);
	}
	fclose(fp);

	buffer.clear();
	buffer.resize(fbuffer.cnt());
	buffer.append((const char*)fbuffer.data(), fbuffer.cnt());

	return !buffer.empty();
}

void CWebServiceCatch::SaveCache(const std::string& url, const std::string& buffer,int nTimeLen)
{
	int nTotalWrite = 0;
	std::string sFilePath = GetFilePath(url);
	assert(!sFilePath.empty());
	assert(!buffer.empty());
	if(sFilePath.empty() || buffer.empty()) return;

	FILE *fp = fopen(sFilePath.c_str(),"wb");
	if(!fp) return;

	time_t nCurTime = time(NULL);
	nCurTime += (time_t)nTimeLen;
	fwrite(&cacheVer,sizeof(int),1,fp);
	fwrite(&nCurTime,sizeof(time_t),1,fp);

	_FStd(FBuffer) fbuffer;
	fbuffer << buffer;
	const size_t oldPos = fbuffer.rpos();
	while (!fbuffer.empty())
	{
		int  n = fwrite(fbuffer[fbuffer.rpos()], 1, fbuffer.nextreadblocksize(), fp);
		fbuffer.rpos(fbuffer.rpos() + n);
		nTotalWrite += n;
	}
	fbuffer.rpos(oldPos, false);

	fclose(fp);
	nTotalWrite += sizeof(int) + sizeof(time_t);
}

std::string CWebServiceCatch::GetFilePath(const std::string& url)
{
	//TODO:
	std::string md5_FileName = em_utility::md5_checksum::get_md5((const unsigned char*)url.c_str(), (unsigned int)url.length());

	static std::string ansiFilePathStr;
	
	if (ansiFilePathStr.empty())
	{
		ansiFilePathStr = "./";
	}
	
	assert(!ansiFilePathStr.empty());

	return (ansiFilePathStr + md5_FileName);
}