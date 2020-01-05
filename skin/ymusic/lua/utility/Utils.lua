

local Utils = FLua.StaticClass("Utils")

function Utils.Value2String(sth)
	if type(sth) ~= "table" then
 		return tostring(sth)
 	end
 	local space, deep = string.rep(' ', 4), 0
 	local result = {}
 	local function _dump(t)
		for k,v in pairs(t) do
			local key = tostring(k)
		 	if type(v) == "table" then
				deep = deep + 2
			 	result[#result+1] = string.format("%s[%s] =\n%s{",string.rep(space, deep - 1),key,string.rep(space, deep - 1))
	 			_dump(v)

			    result[#result+1] = string.format("%s},",string.rep(space, deep - 1))
	 	        deep = deep - 2
	 		else
	 			result[#result+1] = string.format("%s[%s] =%s,",string.rep(space, deep + 1),key, tostring(v))
	 		end
	 	end
 	end
 	result[#result+1] = "Table =\n{"
 	_dump(sth)
 	result[#result+1] = "}"
 	return table.concat( result, "\n")
end

function Utils.printValue(sth)
	print(Utils.Value2String(sth))
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