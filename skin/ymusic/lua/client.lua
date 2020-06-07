_G.nMaxSongNumber = 250
_G.MAX_USER_LIST = 30

_G.IDI_APP = 1001
local WM_USER = DuiLib.MsgArgs.WM_USER
local WM_USER_BEGIN = WM_USER + 1024
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