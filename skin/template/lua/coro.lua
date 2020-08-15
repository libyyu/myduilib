_G.coro = {}
local coroTable = {}
local coroToRun = {}
local stopTag = "stop"

local function newCoro()
	local coro
	coro = coroutine.create(function()
		while true do
			local func = rawget(coroToRun, coro)
			if func then 
				func()
			end
			coroutine.yield(stopTag)
		end
	end)
	return coro
end

function _G.coro.start(func)
	for _, coro in ipairs(coroTable) do
		if rawget(coroToRun, coro) == nil then
			rawset(coroToRun, coro, func)
			return
		end
	end
	
	local coro = newCoro()
	table.insert(coroTable, coro)
	rawset(coroToRun, coro, func)
end

function _G.coro.yield()
	return coroutine.yield()
end

function _G.coro.wait(milliseconds)
	local startTime = Timer.GetTickCount()
	while Timer.GetTickCount() - startTime < milliseconds do
		coroutine.yield()
	end
end

function _G.coro.async_complete_task(task_func)
	local is_complete = false
	local function OnFinish()
		is_complete = true
	end
	task_func(OnFinish)
	while not is_complete do
		coroutine.yield()
	end
end

function _G.coro.async_load(path)
	local result
	coro.async_complete_task(function (OnFinish)
		
	end)
	return result
end

function _G.coro.clear()
	coroTable = {}
end

function _G.TickCoroutine(DeltaTime)
	for coro, func in pairs(coroToRun) do
		if coro and func then
			local ret1, ret2 = coroutine.resume(coro)
			if (not ret1) or (ret2 == stopTag) then
				rawset(coroToRun, coro, nil)
				if not ret1 then
					printerror(debug.traceback(coro, ret2, 0))
				end
			end
		end
	end
end
