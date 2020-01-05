#pragma once
#include "BasePlayer.h"

class CLocalPlayer : 
	public CBasePlayer
{
public:
	CLocalPlayer();
	~CLocalPlayer();
public:
	virtual unsigned long GetDownloadBytes() const;
protected:
	virtual void OnPlay(
		const std::string& wsFile,
		const std::string& wsUrl,
		int nFileType,
		int nBegin,
		int nEnd);
};
