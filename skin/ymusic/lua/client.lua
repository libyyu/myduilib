_G.nMaxSongNumber = 250
_G.MAX_USER_LIST = 30
_G.APP_NAME = "YTing"
_G.IDI_APP = 2
local WM_USER = DuiLib.MsgArgs.WM_USER
local WM_USER_BEGIN = WM_USER + 1024

_G.TBPFLAG =
{
    TBPF_NOPROGRESS	= 0,
    TBPF_INDETERMINATE	= 0x1,
    TBPF_NORMAL	= 0x2,
    TBPF_ERROR	= 0x4,
    TBPF_PAUSED	= 0x8
}

_G.THUMBBUTTONMASK =
{
    THB_BITMAP	= 0x1,
    THB_ICON	= 0x2,
    THB_TOOLTIP	= 0x4,
    THB_FLAGS	= 0x8
}
_G.THBN_CLICKED = 0x1800

_G.USER_MSG = Enum.make
{
	"WM_WEB_SERVICE", "=", WM_USER_BEGIN,
	"WM_HTTP_FILE_DOWNLOAD",
	"WM_INSERT_EXPRESSION",
	"WM_MENU_NOTIFY",
	"WM_TRAY_NOTIFY",
	"WM_ENDING_SCROLL",
}

--定义菜单类型
_G.emMenuType = Enum.make
{
	"EMT_MAIN",
	"EMT_PLAYLIST",
	"EMT_SONGLIST",
	"EMT_SONGITEM",
	"EMT_LRCVIEW",
	"EMT_PLAYMODE",
	"EMT_TONEWLIST",
	"EMT_TOOTHERLIST",
}
--计时器
_G.emTimerType = Enum.make
{
	"EMT_PLAYING_TIMERID", "=", 0x100,
	"EMT_SHOWINFO_TIMERID"
}
--播放列表类型
_G.PlayListType = 
{
	plt_sys = 0,		--系统列表,如默认列表;
	plt_auto = 1, 		--智能列表，推荐，自动生成;
	plt_Roaming = 2,	--漫游;
	plt_user = 3,  		--用户自建列表;
};