
function InitEnv()
	--设置入口环境，必须
	local PaintManagerUI = DuiLib.CPaintManagerUI
	PaintManagerUI.SetResourcePath(ROOT_DIR .. "/skin/ymusic")
	--if DuiLib.IsDebug then
		PaintManagerUI.SetResourceType(DuiLib.UILIB_RESOURCETYPE.UILIB_FILE)
	--else
	--	PaintManagerUI.SetResourceType(DuiLib.UILIB_RESOURCETYPE.UILIB_ZIP)
	--	PaintManagerUI.SetResourceZip("YMusic.dat")
	--end
	warn("InstancePath:", PaintManagerUI.GetInstancePath())
	warn("ResourcePath:", PaintManagerUI.GetResourcePath())
	--创建app对象
	local App = require "app"
	_G.theApp = App()
end
function main(lpCmdLine)
	print("main",lpCmdLine)
	--设置启动环境
	InitEnv()
	--启动app
	theApp:Run(lpCmdLine)
end

function exit(success)
	if theApp then
		theApp:Exit()
		theApp = nil
	end
	DuiLib.QuitApp()
	print("Exit App")
end