#include "AVPlayer.h"
#include <cmath>
#include <vector>
#include <cassert>
#ifdef _MSC_VER
#include <Windows.h>
#else
#include <errno.h>
#endif
#include "flib.h"
//#include "vlc/vlc.h"
//#pragma comment(lib, "libvlc.lib")
//#pragma comment(lib, "libvlccore.lib")


enum libvlc_event_e {
	/* Append new event types at the end of a category.
	 * Do not remove, insert or re-order any entry.
	 * Keep this in sync with lib/event.c:libvlc_event_type_name(). */
	libvlc_MediaMetaChanged = 0,
	libvlc_MediaSubItemAdded,
	libvlc_MediaDurationChanged,
	libvlc_MediaParsedChanged,
	libvlc_MediaFreed,
	libvlc_MediaStateChanged,
	libvlc_MediaSubItemTreeAdded,

	libvlc_MediaPlayerMediaChanged = 0x100,
	libvlc_MediaPlayerNothingSpecial,
	libvlc_MediaPlayerOpening,
	libvlc_MediaPlayerBuffering,
	libvlc_MediaPlayerPlaying,
	libvlc_MediaPlayerPaused,
	libvlc_MediaPlayerStopped,
	libvlc_MediaPlayerForward,
	libvlc_MediaPlayerBackward,
	libvlc_MediaPlayerEndReached,
	libvlc_MediaPlayerEncounteredError,
	libvlc_MediaPlayerTimeChanged,
	libvlc_MediaPlayerPositionChanged,
	libvlc_MediaPlayerSeekableChanged,
	libvlc_MediaPlayerPausableChanged,
	libvlc_MediaPlayerTitleChanged,
	libvlc_MediaPlayerSnapshotTaken,
	libvlc_MediaPlayerLengthChanged,
	libvlc_MediaPlayerVout,

	libvlc_MediaListItemAdded = 0x200,
	libvlc_MediaListWillAddItem,
	libvlc_MediaListItemDeleted,
	libvlc_MediaListWillDeleteItem,

	libvlc_MediaListViewItemAdded = 0x300,
	libvlc_MediaListViewWillAddItem,
	libvlc_MediaListViewItemDeleted,
	libvlc_MediaListViewWillDeleteItem,

	libvlc_MediaListPlayerPlayed = 0x400,
	libvlc_MediaListPlayerNextItemSet,
	libvlc_MediaListPlayerStopped,

	libvlc_MediaDiscovererStarted = 0x500,
	libvlc_MediaDiscovererEnded,

	libvlc_VlmMediaAdded = 0x600,
	libvlc_VlmMediaRemoved,
	libvlc_VlmMediaChanged,
	libvlc_VlmMediaInstanceStarted,
	libvlc_VlmMediaInstanceStopped,
	libvlc_VlmMediaInstanceStatusInit,
	libvlc_VlmMediaInstanceStatusOpening,
	libvlc_VlmMediaInstanceStatusPlaying,
	libvlc_VlmMediaInstanceStatusPause,
	libvlc_VlmMediaInstanceStatusEnd,
	libvlc_VlmMediaInstanceStatusError
};


void On_VLC_Event_Callback(int event_type, void* event, void* data)
{
	CAVPlayer* pThis = (CAVPlayer*)(data);
	assert(pThis);

	pThis->OnVLC_Event(event_type, event);
}

static void On_VLC_Event_Playing(void* event, void* data)
{
	On_VLC_Event_Callback(libvlc_MediaPlayerPlaying, event, data);
}
static void On_VLC_Event_PosChanged(void* event, void* data)
{
	On_VLC_Event_Callback(libvlc_MediaPlayerPositionChanged, event, data);
}
static void On_VLC_Event_EndReached(void* event, void* data)
{
	On_VLC_Event_Callback(libvlc_MediaPlayerEndReached, event, data);
}

enum
{
	em_stop = 0,
	em_play = 1,
	em_pause = 2,
};

CAVPlayer::CAVPlayer(void) 
: m_hPlugin(NULL)
, m_pVLC_Inst(NULL)
, m_pVLC_Player(NULL)
, m_hWnd(NULL)
, m_pCallback(NULL)
, m_bStatus(em_stop)
{
	Init();
}

CAVPlayer::~CAVPlayer(void)
{
    Release();
}

void CAVPlayer::Init()
{
	if (!m_hPlugin)
	{
		_FStd(FPlugin)* plugin = new _FStd(FPlugin)();
#ifdef _MSC_VER
		if (plugin->LoadPlugin("libvlc.dll"))
#elif defined(__APPLE_CC__)
		if (plugin->LoadPlugin("/libvlc.dylib"))
#else
		if (plugin->LoadPlugin("./libvlc.so"))
#endif
		{
			m_hPlugin = plugin;
		}
		else
		{
			delete plugin;
#ifdef _MSC_VER
			printf("\n\tFailed load libvlc.dll， err = %d\n", ::GetLastError());
#else
			printf("\n\tFailed load libvlc.dll， err = %d\n", errno);
#endif
		}
	}

    if (! m_pVLC_Inst && m_hPlugin)
    {
		typedef void* (*pfn_libvlc_new) (int, const char* const*);
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		m_pVLC_Inst = plugin->Call<void*, pfn_libvlc_new>("libvlc_new", 0, NULL);
    }
}

void CAVPlayer::Release()
{
    Stop();

    if (m_pVLC_Inst)
    {
		typedef void (*pfn_libvlc_release) (void*);
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		pfn_libvlc_release pfunc = plugin->Get< pfn_libvlc_release >("libvlc_release");
		pfunc(m_pVLC_Inst);
        m_pVLC_Inst  = NULL;
    }

	_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
	if (plugin) delete plugin;
	m_hPlugin = NULL;
}

bool CAVPlayer::PlayFile(const std::string &strPath)
{
	// 如果播放引擎没有创建，则创建它
    if (! m_pVLC_Inst)
    {
        Init();
    }

	// 如果链接地址为空，或播放引擎没创建，则直接返回
    if(strPath.empty() || ! m_pVLC_Inst)
    {
        return false;
    }
	std::string spFilePath = _FStd(FNormalize)(strPath.c_str());

	// 停止之前的
    Stop();

    bool bRet = false;
    void *m = NULL;//
	_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
    {
		typedef void* (*pfn_libvlc_media_new_path) (void*, const char*);
		pfn_libvlc_media_new_path pfunc = plugin->Get< pfn_libvlc_media_new_path >("libvlc_media_new_path");
		m = pfunc(m_pVLC_Inst, spFilePath.c_str());

    }

    if (m)
    {
		typedef void* (*pfn_libvlc_media_player_new_from_media) (void*);
		pfn_libvlc_media_player_new_from_media pfunc = plugin->Get< pfn_libvlc_media_player_new_from_media >("libvlc_media_player_new_from_media");

        if ((m_pVLC_Player = pfunc(m)))
        {
			typedef void (*pfn_libvlc_media_player_set_hwnd) (void*, void*);
			pfn_libvlc_media_player_set_hwnd pfunc_set_hwnd = plugin->Get< pfn_libvlc_media_player_set_hwnd >("libvlc_media_player_set_hwnd");

			typedef int (*pfn_libvlc_media_player_play) (void*);
			pfn_libvlc_media_player_play pfunc_play = plugin->Get< pfn_libvlc_media_player_play >("libvlc_media_player_play");

			typedef void* (*pfn_libvlc_media_player_event_manager) (void*);
			pfn_libvlc_media_player_event_manager pfunc_event_manager = plugin->Get< pfn_libvlc_media_player_event_manager >("libvlc_media_player_event_manager");

			pfunc_set_hwnd(m_pVLC_Player, m_hWnd);
			int code = pfunc_play(m_pVLC_Player);
			m_bStatus = em_play;//播放状态
            // 事件管理
            void *vlc_evt_man = pfunc_event_manager(m_pVLC_Player);

			typedef void (*pfn_libvlc_callback_t)(void*, void*);
			typedef int (*pfn_libvlc_event_attach) (void*, int, pfn_libvlc_callback_t, void*);
			pfn_libvlc_event_attach pfunc_event_attach = plugin->Get< pfn_libvlc_event_attach >("libvlc_event_attach");

			pfunc_event_attach(vlc_evt_man, libvlc_MediaPlayerPlaying, On_VLC_Event_Playing, this);
			pfunc_event_attach(vlc_evt_man, libvlc_MediaPlayerPositionChanged, On_VLC_Event_PosChanged, this);
			pfunc_event_attach(vlc_evt_man, libvlc_MediaPlayerEndReached, On_VLC_Event_EndReached, this);
            bRet = true;
        }

		typedef void (*pfn_libvlc_media_release) (void*);
		pfn_libvlc_media_release pfunc_media_release = plugin->Get< pfn_libvlc_media_release >("libvlc_media_release");
		pfunc_media_release(m);
    }

	if (!bRet)
	{
		typedef const char* (*pfn_libvlc_errmsg) (void);
		pfn_libvlc_errmsg perrorf = plugin->Get<pfn_libvlc_errmsg>("libvlc_errmsg");
		const char* err = perrorf();
		printf("\n\terr = %s", err);
	}

    return bRet;
}

bool CAVPlayer::PlayURL(const std::string& strURL)
{
	// 如果播放引擎没有创建，则创建它
	if (!m_pVLC_Inst)
	{
		Init();
	}

	// 如果链接地址为空，或播放引擎没创建，则直接返回
	if (strURL.empty() || !m_pVLC_Inst)
	{
		return false;
	}

	// 停止之前的
	Stop();

	bool bRet = false;
	void* m = NULL;//

	// 网络路径
	{
		typedef void* (*pfn_libvlc_media_new_location) (void*, const char*);
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		pfn_libvlc_media_new_location pfunc = plugin->Get< pfn_libvlc_media_new_location >("libvlc_media_new_location");
		m = pfunc(m_pVLC_Inst, strURL.c_str());
	}

	if (m)
	{
		typedef void* (*pfn_libvlc_media_player_new_from_media) (void*);
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		pfn_libvlc_media_player_new_from_media pfunc = plugin->Get< pfn_libvlc_media_player_new_from_media >("libvlc_media_player_new_from_media");

		if ((m_pVLC_Player = pfunc(m)))
		{
			typedef void (*pfn_libvlc_media_player_set_hwnd) (void*, void*);
			pfn_libvlc_media_player_set_hwnd pfunc_set_hwnd = plugin->Get< pfn_libvlc_media_player_set_hwnd >("libvlc_media_player_set_hwnd");

			typedef int (*pfn_libvlc_media_player_play) (void*);
			pfn_libvlc_media_player_play pfunc_play = plugin->Get< pfn_libvlc_media_player_play >("libvlc_media_player_play");

			typedef void* (*pfn_libvlc_media_player_event_manager) (void*);
			pfn_libvlc_media_player_event_manager pfunc_event_manager = plugin->Get< pfn_libvlc_media_player_event_manager >("libvlc_media_player_event_manager");

			pfunc_set_hwnd(m_pVLC_Player, m_hWnd);
			pfunc_play(m_pVLC_Player);
			m_bStatus = em_play;//播放状态
			// 事件管理
			void* vlc_evt_man = pfunc_event_manager(m_pVLC_Player);

			typedef void (*pfn_libvlc_callback_t)(void*, void*);
			typedef int (*pfn_libvlc_event_attach) (void*, int, pfn_libvlc_callback_t, void*);
			pfn_libvlc_event_attach pfunc_event_attach = plugin->Get< pfn_libvlc_event_attach >("libvlc_event_attach");

			pfunc_event_attach(vlc_evt_man, libvlc_MediaPlayerPlaying, On_VLC_Event_Playing, this);
			pfunc_event_attach(vlc_evt_man, libvlc_MediaPlayerPositionChanged, On_VLC_Event_PosChanged, this);
			pfunc_event_attach(vlc_evt_man, libvlc_MediaPlayerEndReached, On_VLC_Event_EndReached, this);
			bRet = true;
		}

		typedef void (*pfn_libvlc_media_release) (void*);
		pfn_libvlc_media_release pfunc_media_release = plugin->Get< pfn_libvlc_media_release >("libvlc_media_release");
		pfunc_media_release(m);
	}

	return bRet;
}

void CAVPlayer::Stop()
{
    if (m_pVLC_Player)
    {
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		typedef void (*pfn_libvlc_media_player_stop) (void*);
		pfn_libvlc_media_player_stop pfunc_player_stop = plugin->Get< pfn_libvlc_media_player_stop >("libvlc_media_player_stop");

		typedef void (*pfn_libvlc_media_player_release) (void*);
		pfn_libvlc_media_player_release pfunc_player_release = plugin->Get< pfn_libvlc_media_player_release >("libvlc_media_player_release");

		pfunc_player_stop(m_pVLC_Player);      /* Stop playing */
		pfunc_player_release(m_pVLC_Player);   /* Free the media_player */
        m_pVLC_Player = NULL;
		m_bStatus = em_stop;
    }	
}

int CAVPlayer::Play()
{
	int ret = -1;
    if (m_pVLC_Player)
    {
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		typedef int (*pfn_libvlc_media_player_play) (void*);
		pfn_libvlc_media_player_play pfunc_player_play = plugin->Get< pfn_libvlc_media_player_play >("libvlc_media_player_play");

		ret = pfunc_player_play(m_pVLC_Player);
		m_bStatus = em_play;
    }
	return ret;
}

void CAVPlayer::Pause()
{
    if (m_pVLC_Player)
    {
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		typedef void (*pfn_libvlc_media_player_pause) (void*);
		pfn_libvlc_media_player_pause pfunc_player_pause = plugin->Get< pfn_libvlc_media_player_pause >("libvlc_media_player_pause");

		pfunc_player_pause(m_pVLC_Player);
		m_bStatus = em_pause;
    }
}

void CAVPlayer::Volume(int iVol)
{
    if (iVol < 0)
    {
        return;
    }

    if (m_pVLC_Player)
    {
		typedef int (*libvlc_audio_set_volume) (void*, int);
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		libvlc_audio_set_volume pfunc = plugin->Get< libvlc_audio_set_volume >("libvlc_audio_set_volume");
		pfunc(m_pVLC_Player, int(iVol * 2.5));// 如果放到100，感觉比迅雷的100少了30，所以这里用1.3倍音量
        
		// libvlc_audio_set_volume((libvlc_media_player_t*)m_pVLC_Player, int(iVol * 2.5)); // 如果放到100，感觉比迅雷的100少了30，所以这里用1.3倍音量
    }
}

void CAVPlayer::VolumeIncrease()
{
    //if (m_pVLC_Player)
    //{
    //    int iVol = libvlc_audio_get_volume((libvlc_media_player_t*)m_pVLC_Player);
    //    Volume((int)ceil(iVol * 1.1));
    //}
	if (m_pVLC_Player)
	{
		typedef int (*pfn_libvlc_audio_get_volume) (void*);
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		pfn_libvlc_audio_get_volume pfunc = plugin->Get< pfn_libvlc_audio_get_volume >("libvlc_audio_get_volume");
		int iVol = pfunc(m_pVLC_Player);
		Volume((int)floor(iVol * 1.1));
	}
}

void CAVPlayer::VolumeReduce()
{
    //if (m_pVLC_Player)
    //{
    //    int iVol = libvlc_audio_get_volume((libvlc_media_player_t*)m_pVLC_Player);
    //    Volume((int)floor(iVol * 0.9));
    //}

	if (m_pVLC_Player)
	{
		typedef int (*pfn_libvlc_audio_get_volume) (void*);
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		pfn_libvlc_audio_get_volume pfunc = plugin->Get< pfn_libvlc_audio_get_volume >("libvlc_audio_get_volume");
		int iVol = pfunc(m_pVLC_Player);
		Volume((int)floor(iVol * 0.9));
	}
}

int CAVPlayer::GetPos()
{   
    //if (m_pVLC_Player)
    //{
    //    return (int)(1000 * libvlc_media_player_get_position((libvlc_media_player_t*)m_pVLC_Player));
    //}

	if (m_pVLC_Player)
	{
		typedef float (*pfn_libvlc_media_player_get_position) (void*);
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		pfn_libvlc_media_player_get_position pfunc = plugin->Get< pfn_libvlc_media_player_get_position >("libvlc_media_player_get_position");

		return (int)(1000 * pfunc(m_pVLC_Player));
	}

    return 0;
}

void CAVPlayer::SeekTo(int iPos)
{
    if (iPos < 0 || iPos > 1000)
    {
        return;
    }

    if (m_pVLC_Player)
    {
    //    libvlc_media_player_set_position((libvlc_media_player_t*)m_pVLC_Player, iPos/(float)1000.0);
		typedef void (*pfn_libvlc_media_player_set_position) (void*, float);
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		pfn_libvlc_media_player_set_position pfunc = plugin->Get< pfn_libvlc_media_player_set_position >("libvlc_media_player_set_position");
		pfunc(m_pVLC_Player, iPos / (float)1000.0);

    }
}

void CAVPlayer::SeekForward(int seconds)
{
    //int iPos = GetPos();
    //SeekTo((int)ceil(iPos * 1.1));

    // 一次快退5秒
    if (m_pVLC_Player)
	{        
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		typedef int64_t (*pfn_libvlc_media_player_get_time) (void*);
		pfn_libvlc_media_player_get_time pfunc = plugin->Get< pfn_libvlc_media_player_get_time >("libvlc_media_player_get_time");

		int64_t i_time = pfunc(m_pVLC_Player) + 1000 * seconds;

        if (i_time > GetTotalTime())
        {
            i_time = GetTotalTime();
        }

		typedef void (*pfn_libvlc_media_player_set_time) (void*, int64_t);
		pfn_libvlc_media_player_set_time pfunc2 = plugin->Get< pfn_libvlc_media_player_set_time >("libvlc_media_player_set_time");
		pfunc2(m_pVLC_Player, i_time);
    }
}

void CAVPlayer::SeekBackward(int seconds)
{
    if (m_pVLC_Player)
    {
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		typedef int64_t (*pfn_libvlc_media_player_get_time) (void*);
		pfn_libvlc_media_player_get_time pfunc = plugin->Get< pfn_libvlc_media_player_get_time >("libvlc_media_player_get_time");

		int64_t i_time = pfunc(m_pVLC_Player) - 1000 * seconds;

        if (i_time < 0)
        {
            i_time = 0;
        }
		typedef void (*pfn_libvlc_media_player_set_time) (void*, int64_t);
		pfn_libvlc_media_player_set_time pfunc2 = plugin->Get< pfn_libvlc_media_player_set_time >("libvlc_media_player_set_time");
		pfunc2(m_pVLC_Player, i_time);
    }
}

void CAVPlayer::SetHWND(H_HWND hwnd )
{
#ifdef _MSC_VER
    if (::IsWindow((HWND)hwnd))
    {
        m_hWnd = hwnd;
    }
#endif
}

H_HWND CAVPlayer::GetHWND()
{
    return m_hWnd;
}

int  CAVPlayer::GetStatus()
{
	return m_bStatus;
}

bool CAVPlayer::IsURL(const std::string &strPath)
{
	bool bURL = false;
	std::vector<std::string> vctURL;

	vctURL.push_back("http"); 
	vctURL.push_back("https"); 
	vctURL.push_back("ftp");
	vctURL.push_back("rstp");

	for (unsigned i = 0; i < vctURL.size(); i++)
	{
		// 实际使用请判断大小写
		if (! strPath.compare(0, vctURL[i].size(), vctURL[i]))
		{
			bURL = true;
			break;
		}
	}
	return bURL;
}

bool CAVPlayer::IsValid()
{
	return NULL != m_hPlugin && NULL != m_pVLC_Inst;
}

bool CAVPlayer::IsOpen()
{
    return NULL != m_pVLC_Player;
}

bool CAVPlayer::IsPlaying()
{
    if (m_pVLC_Player)
    {
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		typedef int (*pfn_libvlc_media_player_is_playing) (void*);
		pfn_libvlc_media_player_is_playing pfunc = plugin->Get< pfn_libvlc_media_player_is_playing >("libvlc_media_player_is_playing");

        return (1 == pfunc(m_pVLC_Player));
    }

    return false;
}

__int64 CAVPlayer::GetTotalTime()
{
    if (m_pVLC_Player)
    {
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		typedef int64_t (*pfn_libvlc_media_player_get_length) (void*);
		pfn_libvlc_media_player_get_length pfunc = plugin->Get< pfn_libvlc_media_player_get_length >("libvlc_media_player_get_length");

        return pfunc(m_pVLC_Player);
    }

    return 0;
}

__int64 CAVPlayer::GetTime()
{
    if (m_pVLC_Player)
    {
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		typedef int64_t(*pfn_libvlc_media_player_get_time) (void*);
		pfn_libvlc_media_player_get_time pfunc = plugin->Get< pfn_libvlc_media_player_get_time >("libvlc_media_player_get_time");

        return pfunc(m_pVLC_Player);
    }

    return 0;
}

int CAVPlayer::GetVolume()
{
    if (m_pVLC_Player)
    {
		_FStd(FPlugin)* plugin = (_FStd(FPlugin)*)m_hPlugin;
		typedef int (*pfn_libvlc_audio_get_volume) (void*);
		pfn_libvlc_audio_get_volume pfunc = plugin->Get< pfn_libvlc_audio_get_volume >("libvlc_audio_get_volume");

        return pfunc(m_pVLC_Player);
    }

    return 0;
}

void CAVPlayer::OnVLC_Event(int event_type, void *data)
{
    switch(event_type)
    {
    case libvlc_MediaPlayerPlaying:
		if (m_pCallback) m_pCallback->OnPlaying(this, data);
        break;
    case libvlc_MediaPlayerPositionChanged:
		if (m_pCallback) m_pCallback->OnPosChanged(this, data);
        break;
    case libvlc_MediaPlayerEndReached:
		if (m_pCallback) m_pCallback->OnEndReached(this, data);
        break;
    default:
        break;
    } 
}

#ifdef   __cplusplus
extern "C" {
#endif

	PLAYCENTER_API void test_player()
	{
		CAVPlayer* player = new CAVPlayer();
#if FLIB_COMPILER_WINDOWS
		bool ret = player->PlayFile("d:\\Workspace\\test\\t\\Debug\\aaaa.mp3");
		printf("playfile ret = %d\n", ret);
#elif FLIB_COMPILER_MACOSX
		bool ret = player->PlayFile("/Volumes/SHARED/WorkSpace/duilib/1-07 生日快乐 (Live).mp3");
		printf("playfile ret = %d\n", ret);
#else
		printf("platform not implement")
#endif
		
		printf("exit \n");
	}

#ifdef   __cplusplus
}
#endif
