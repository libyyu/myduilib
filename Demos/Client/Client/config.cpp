#include "stdafx.h"
#include "config.h"

DuiLib::CDuiString Config::ReadIP(LPCTSTR sFile)
{
	if(!mci::file_exist(sFile))
		return _T("127.0.0.1");
	TCHAR tIP[MAX_PATH] = {0};
	GetPrivateProfileString(_T("ipconfig"),_T("ip"),_T("127.0.0.1"),tIP,MAX_PATH,sFile);
	
	return DuiLib::CDuiString(tIP);
}
void Config::WriteIP(LPCTSTR sIP,LPCTSTR sFile)
{
	WritePrivateProfileString(_T("ipconfig"),_T("ip"),sIP,sFile);
}
DuiLib::CDuiString Config::ReadPort(LPCTSTR sFile)
{
	if(!mci::file_exist(sFile))
		return _T("5556");
	TCHAR tPort[MAX_PATH] = {0};
	GetPrivateProfileString(_T("ipconfig"),_T("port"),_T("127.0.0.1"),tPort,MAX_PATH,sFile);
	return DuiLib::CDuiString(tPort);
}
void Config::WritePort(LPCTSTR sPort,LPCTSTR sFile)
{
	WritePrivateProfileString(_T("ipconfig"),_T("port"),sPort,sFile);
}