require "preload"
require "client"

local App = FLua.Class("App")
do
	function App:__constructor()
		self._spMainWin = nil
		self._spRuntimeState = nil
		self._pGlobalTimer = nil
	end

	function App.Instance()
		return _G.theApp
	end

	function App:ParseCmdLine(szCmdLine)
		warn("解析命令行：", szCmdLine)
		if szCmdLine:find("-console") then
			warn("开启控制台")
			DuiLib.DuiSystem.ToggleConsole(true)
		end
	end

	function App:Init()
		local RuntimeStateT = require "main.RuntimeState"
		self._spRuntimeState = RuntimeStateT()
		self._spRuntimeState:Init(0)
		return not not self._spRuntimeState
	end
	function App:GetRuntimeState()
		return self._spRuntimeState
	end
	function App:GetMainWindow()
		return self._spMainWin
	end

	function App:Run(szCmdLine)
		self:ParseCmdLine(szCmdLine)
		if not self:Init() then
			return
		end
		print("Run App")
		local MainFrame = require "gui.MainWindow"
		local mainFrame = MainFrame.new()
		mainFrame:CreateWindow(nil)
		assert(mainFrame:IsValid())
		print("ShowWindow")
		mainFrame:ShowWindow(true, true)
		mainFrame:CenterWindow()
		self._spMainWin = mainFrame
		Application.SetMainWindow(mainFrame.m_hWin)

		--主线程任务
		self._pGlobalTimer = Timer.AddGlobalTimer(10, function()
			self:OnTick(10)

			-- 强制进行一次垃圾收集
			collectgarbage("collect")
		end)
	end
	function App:Exit()
		if self._pGlobalTimer then
			Timer.RemoveGlobalTimer(self._pGlobalTimer)
			self._pGlobalTimer = nil
		end
		--local ProtoUtil = require "proto.ProtoUtil"
		--ProtoUtil.ClearProtos()
		for panel, v in pairs( _G.FRAME_MAPS ) do
			if v and panel:IsValid() then
				panel:Close()
			end
		end
		--self._spMainWin:Release()
		self._spMainWin = nil
		self._spRuntimeState:UnInit()
		self._spRuntimeState = nil
	end

	function App:OnTick(dt)
		MainThreadTask.tick()
	end
end

return App