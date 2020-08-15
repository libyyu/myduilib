local CRuntimeState = FLua.Class("CRuntimeState")

function CRuntimeState:__constructor()
	self._isOnline = false

	self._spCacheMgr = nil
	self._spFilePathMgr = nil
	self._spLogMgr = nil
	self._spDbMgr = nil
--	self._spWinNotifyCenter = nil
	self._spUserInfo = nil
	self._spConfigMgr = nil
	self._spWebServiceMgr = nil
	self._spHttpDowner = nil
	self._spEvent = nil
end

function CRuntimeState:Init(dwMainThreadId)
	--构建随机种子
	math.randomseed(tostring(os.time()):reverse():sub(1, 6))
	--初始未登录
	self._isOnline = false
	--
	local FLibEventT = require "utility.FLibEvent"
	self._spEvent = FLibEventT.New("logic")
	--文件管理器
	local FilePathMgrT = require "main.FilePathMgr"
	self._spFilePathMgr = FilePathMgrT()
	self._spFilePathMgr:Init()
	--缓存管理器
	local CacheMgrT = require "main.CacheMgr"
	self._spCacheMgr = CacheMgrT()
	self._spCacheMgr:Init(self._spFilePathMgr:GetDefaultCachePath())
	--日志管理器
	local str = self._spFilePathMgr:GetLogPath()
	local strLogFileName = str .. APP_NAME .. ".log"
	warn("Init Logger", strLogFileName)
	Application.CreateLogger(strLogFileName)
	--数据库管理器
	local DatabaseManagerT = require "database.DatabaseManager"
	self._spDbMgr = DatabaseManagerT()
	self._spDbMgr:SetDatabasePath(self._spFilePathMgr:GetDatabasePath())
	self._spDbMgr:GetGlobalDBase()
	self._spDbMgr:SetCurrentUser(self._spUserInfo)
	self._spDbMgr:GetUserDB()
	----网络管理类
	--_spWebServiceMgr = spWebServiceMgrT(new CWebServiceMgr());
	--_spWebServiceMgr->SetNotifyMsg(WM_WEB_SERVICE);
	----配置管理器
	local ConfigMgrT = require "config.ConfigMgr"
	self._spConfigMgr = ConfigMgrT()
	self._spConfigMgr:Load()
	----文件下载管理容器
	--_spHttpDowner = spHttpDownerT(new em_utility::down_http_file);
	--assert(_spHttpDowner);
	--_spHttpDowner->start_thread();
	----初始化libcurl环境
	--HttpManagerProto::Instance()->CurlGlobalInit();
	--StreamThread::Instance()->Startup();

	-- 强制进行一次垃圾收集
    collectgarbage("collect")
end
function CRuntimeState:UnInit()
end
function CRuntimeState:IsMainThread()
	return true
end
function CRuntimeState:IsOnline()
end
function CRuntimeState:GetCurrentContext(sflag)
	local UserContextT = require "main.UserContext"
	local ctx = CUserContext()
	ctx:Create(self, self._spUserInfo, self:GetDbMgr():GetUserDB())
	return ctx
end
function CRuntimeState:SetUserInfo(user)
	if self._spUserInfo ~= user then
		if self._spUserInfo then --注销
			--self._spUserInfo->bKeepLogged = false;
			print("注销");
			--print("theDbMgr->GetGlobalDBase():: %x",theDbMgr->GetGlobalDBase());
			assert(self:GetDbMgr():GetGlobalDBase())
			assert(self:GetDbMgr():GetGlobalDBase():UpdateUserInfo(self._spUserInfo))
		end

		self:GetDbMgr():SetCurrentUser(user)
		self._spUserInfo = user
		self:GetDbMgr():GetUserDBase()
		if self._spUserInfo then
			assert(self:GetDbMgr():GetGlobalDBase():UpdateLastUserInfo(self._spUserInfo))
		end
		assert(self._spPlayListMgr)
		self._spPlayListMgr:LoadAllPlayList()
		--_spConfigMgr->Load();
		--PostMessage(theMainWin->GetHWND(),WM_LOGIN,(unsigned long)&user,0);
	end
end
function CRuntimeState:GetLogUserInfo()
	return self._spUserInfo
end
function CRuntimeState:GetCacheMgr()
	return self._spCacheMgr
end
function CRuntimeState:GetFilePathMgr()
	return self._spFilePathMgr
end
function CRuntimeState:GetLogMgr()
end
function CRuntimeState:GetDbMgr()
	return self._spDbMgr
end
function CRuntimeState:GetConfigMgr()
	return self._spConfigMgr
end
function CRuntimeState:GetEventMgr()
	return self._spEvent
end


return CRuntimeState