#include "PlayerFactory.h"
#include <cassert>
#include "OnlinePlayer.h"

CPlayerFactory::CPlayerFactory(void)
{
}


CPlayerFactory::~CPlayerFactory(void)
{
	ScopedLockT lockObj(_lock);
	for (playerMapT::iterator it = _playerMap.begin(); it != _playerMap.end(); ++it)
	{
		spPlayerT thePlayer = it->second;
		if (thePlayer)
		{
			thePlayer->SetCallBack(NULL);
			thePlayer->Stop();
		}
	}
	_playerMap.clear();
}

IPlayer* CPlayerFactory::createPlayer()
{
	ScopedLockT lockObj(_lock);
	spPlayerT thePlayer(new COnlinePlayer());
	assert(thePlayer);
	if(!thePlayer)
		return NULL;

	_playerMap.insert(playerMapT::value_type(thePlayer.get(),thePlayer));
	return thePlayer.get();
}

void CPlayerFactory::destroyPlayer(IPlayer* pPlayer)
{
	assert(pPlayer);
	if(!pPlayer)
		return;

	spPlayerT thePlayer;
	{
		ScopedLockT lockObj(_lock);
		playerMapIterT it = _playerMap.find(pPlayer);
		if(_playerMap.end() != it)
		{
			thePlayer = it->second;
			_playerMap.erase(it);
		}
	}

	if(thePlayer)
	{
		thePlayer->SetCallBack(NULL);
		thePlayer->Stop();
	}
}