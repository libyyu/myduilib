#include "PlayerFactory.h"
#ifdef _WIN32
#	include <winsock2.h>
#	include <windows.h>
#endif
#include "Utility/log.h"

#ifdef   __cplusplus
extern "C" {
#endif

	CPlayerFactory thePlayerFactory;
	PLAYER_PLUGIN_DLL_API IPlayer*  create_player()
	{
		return thePlayerFactory.createPlayer();
	}
	PLAYER_PLUGIN_DLL_API void destroy_player(IPlayer* pPlayer)
	{
		return thePlayerFactory.destroyPlayer(pPlayer);
	}

	PLAYER_PLUGIN_DLL_API int test_player(const char* filename, const char* urlpath, int type)
	{
		IPlayer* player = create_player();
		if (player)
		{
			std::string dir = _FStd(FGetDirectory(filename));
			_FStd(FMakeDir(dir.c_str()));
			sLog("play:%s --> %s\n", filename, urlpath ? urlpath : "<local>");
			return player->StartToPlay(filename, urlpath, type, 0, 0, 0, NULL) ? 0 : -1;
		}
		return 1;
	}

#ifdef   __cplusplus
}
#endif