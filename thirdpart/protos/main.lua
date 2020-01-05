package.cpath = "../Projects/windows/x86/release/?.dll;"..package.cpath
local _G = _G
local proto_path = ... or "."
print (proto_path)

local proto_file = proto_path .. "/config_common.proto"

local protos = require "protos"


function _G.table_string(sth)
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
			 	result[#result+1] = string.format("%s[%s] = \n%s{",string.rep(space, deep - 1),key,string.rep(space, deep - 1))
	 			_dump(v)

			    result[#result+1] = string.format("%s},",string.rep(space, deep - 1))
	 	        deep = deep - 2
	 		else
	 			result[#result+1] = string.format("%s[%s] = %s,",string.rep(space, deep + 1),key,v)
	 		end
	 	end
 	end
 	result[#result+1] = string.format("Table = \n{")
 	_dump(sth)
 	result[#result+1] = string.format("}")
 	return table.concat( result, "\n")
end

-- local msg = protos.GetMessage(proto_file, "gp_create_role_days")

-- print(table_string(msg))
-- print("11-----------------------------------------------------")
-- local msg2 = protos.GetMessage(proto_file, "gp_mold_spirit_re")
-- print(table_string(msg2))
-- print("22-----------------------------------------------------")
-- local trailing_comments, leading_comments = protos.GetComment(proto_file, "MainSpirit.SubSpirit", "mold_progress")
-- print(trailing_comments, leading_comments)
-- print("33-----------------------------------------------------")
-- local trailing_comments, leading_comments = protos.GetComment(proto_file, "gp_mold_spirit_re", "weapon_spirits")
-- print(trailing_comments, leading_comments)
-- print("44-----------------------------------------------------")
-- local trailing_comments, leading_comments, trailing_comments2, leading_comments2 = protos.GetComment(proto_file, "GP_Begin", "type")
-- print(trailing_comments, leading_comments, trailing_comments2, leading_comments2)
-- print("55-----------------------------------------------------")
-- local trailing_comments, leading_comments = protos.GetComment(proto_file, "gp_mold_spirit_re")
-- print(trailing_comments, leading_comments)

function _G.GetMessageComment(proto_file, message_name)
	local _, _, trailing_comments2 = protos.GetComment(proto_file, message_name, "type_t")
	return message_name.." ["..trailing_comments2.."]"
end

function _G.GetFieldComment(proto_file, message_name, field_name)
	local trailing_comments = protos.GetComment(proto_file, message_name, field_name)
	return trailing_comments
end

local name = GetMessageComment(proto_file, "SlingShot")
print(name)

local assets = protos.GetAllMessages(proto_file, function(msg, name)
	if msg.type_t then return true end
	return false
end)
for i, v in ipairs( assets ) do
	print( i, v .. GetMessageComment(proto_file, v))
end

protos.ClearProto("")
