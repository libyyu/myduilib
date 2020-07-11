

local Utils = FLua.StaticClass("Utils")

function Utils.Value2String(sth)
	if type(sth) ~= "table" then
 		return tostring(sth)
 	else
 		local mt = getmetatable(sth)
		if mt and mt.__tostring then
			return tostring(sth)
		end
 	end
 	local function make_key(k)
		if type(k) == "number" then
			return "[" .. tostring(k) .. "]"
		else
			return tostring(k)
		end
	end
	local function make_value(v)
		if type(v) == "string" then
			return "\"" .. v .. "\""
		else
			return tostring(v)
		end
	end

	local sb = {}
	local space, deep = string.rep(' ', 4), 0
	space = "\t" .. space
	local function _dump(t, level)
		local mt = getmetatable(t)
		if mt and mt.__pMessage and mt.__pDescriptor then--是一个pb协议
			local v = tostring(t)
			local arr = v:split("\n")
			for _, v in ipairs(arr) do
				sb[#sb+1] = string.rep(space, level + 1) .. tostring(v)
			end
		elseif mt and mt.__tostring then
			sb[#sb+1] = string.rep(space, level + 1) .. tostring(t)
		else
			for key, v in pairs(t) do
				if type(v) == "table" then
					sb[#sb+1] = string.format("%s%s = {", string.rep(space, level + 1), make_key(key))
					_dump(v, level+1)
					sb[#sb+1] = string.format("%s},", string.rep(space, level + 1))
				else
					sb[#sb+1] = string.format("%s%s = %s,", string.rep(space, level + 1), make_key(key), make_value(v))
				end
			end
		end
	end

	sb[#sb+1] = "\n\t{"
	_dump(sth, deep)
	sb[#sb+1] = "\t}"
 	return table.concat(sb, "\n")
end

function Utils.printValue(...)
	local sb = {}
	for _,v in ipairs({...}) do
		sb[#sb+1] = Utils.Value2String(v)
	end
	warn(table.concat(sb, " "))
end

local userdata_hash_id_table = setmetatable({}, { __mode = "k" })
local userdata_hash_ud_table = setmetatable({}, { __mode = "k" })
function Utils.CastToString(ud)
	assert( type(ud) == "userdata",  "argument #1 must be userdata")
	local id = DuiLib.UserDataToUInt64(ud)
	userdata_hash_id_table[id] = ud
	userdata_hash_ud_table[ud] = id
	return id
end
function Utils.CastToUserData(id)
	if userdata_hash_id_table[id] then
		local ud = userdata_hash_id_table[id]
		if ud and not ud.isnil then
			return ud
		else
			userdata_hash_id_table[id] = nil
			if ud then
				userdata_hash_ud_table[ud] = nil
			end
		end
	end
	return nil
end

return Utils