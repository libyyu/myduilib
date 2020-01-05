#pragma once

class Config
{
public:
    static DuiLib::CDuiString ReadIP(LPCTSTR sFile);
	static void WriteIP(LPCTSTR sIP,LPCTSTR sFile);

	static DuiLib::CDuiString ReadPort(LPCTSTR sFile);
	static void WritePort(LPCTSTR sPort,LPCTSTR sFile);
private:

};
