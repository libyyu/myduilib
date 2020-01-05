#pragma once
#include <unordered_map>
#include <mutex>
#include <memory>
#include "IPlayer.h"

class CPlayerFactory
{
public:
	CPlayerFactory(void);
	~CPlayerFactory(void);
public:
	IPlayer* createPlayer();
	void destroyPlayer(IPlayer* pPlayer);
protected:
	typedef std::shared_ptr<IPlayer>				spPlayerT;
	typedef std::unordered_map<IPlayer*,spPlayerT>	playerMapT;
	typedef playerMapT::const_iterator				playerMapIterT;

	typedef std::mutex								LockT;
#if _HAS_CXX17
	typedef std::scoped_lock						ScopedLockT;
#else
	typedef std::lock_guard<LockT>					ScopedLockT;
#endif

	LockT           _lock;
	playerMapT      _playerMap;
};
