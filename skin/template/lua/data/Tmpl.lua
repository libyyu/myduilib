local Tmpl = FLua.Class("Tmpl")

function Tmpl:__constructor()
	self.filepath_ = nil
	self.message_ = nil
	self.message_class_ = nil
end

function Tmpl.new(filename, cls)
	local obj = Tmpl()
	obj.filepath_ = filename
	obj.message_class_ = cls
	obj:Init()
	return obj
end

function Tmpl:toString()
	return string.format("[%s] %s", self:GetTmplName(), tostring(self.message_))
end

function Tmpl:GetTmplName()
	return path.basename(self.filepath_)
end

function Tmpl:GetFilePath()
	return self.filepath_
end

function Tmpl:GetMessageName()
	return self.message_class_:GetName()
end

function Tmpl:SerializeToTable()
	return self.message_:SerializeToTable(true)
end

function Tmpl:Init()
	self.message_ = self.message_class_()
	local file = io.open(helper.UTF8ToMB(self.filepath_), "r")
	if file then
		local function int64_convert(v)
			return make_i64(v, false)
		end
		local function uint64_convert(v)
			return make_i64(v, true)
		end
		local string_type = {
			string = function(v) return v and tostring(v) or "" end,
			bytes = function(v) return v and tostring(v) or "" end,
			double = function(v) return v and tonumber(v) or 0 end,
			float = function(v) return v and tonumber(v) or 0 end,
			int32 = function(v) return v and tonumber(v) or 0 end,
			uint32 = function(v) return v and tonumber(v) or 0 end,
			fixed32 = function(v) return v and tonumber(v) or 0 end,
			sfixed32 = function(v) return v and tonumber(v) or 0 end,
			sint32 = function(v) return v and tonumber(v) or 0 end,
			enum = function(v) return v and tonumber(v) or 0 end,
			bool = function(v) return v=='1' or v == 'true' end,
			int64 = function(v) return int64_convert(v) end,
			uint64 = function(v) return uint64_convert(v) end,
			fixed64 = function(v) return int64_convert(v) end,
			sfixed64 = function(v) return int64_convert(v) end,
			sint64 = function(v) return int64_convert(v) end,
		}

		local line = file:read()
		while line do
			local arr = line:split(':')
			local head = arr[1]:split('@')
			local type_ = head[1]
			local filedName = head[2]

			if string_type[type_] then
				self.message_:setex(filedName, string_type[type_](arr[2]))
			end

			line = file:read()
		end
	end
end

function Tmpl:Save()
	return self:SaveAs(self.filepath_)
end

function Tmpl:SaveAs(filepath)
	local file = io.open(helper.UTF8ToMB(filepath), "w")
	if file then
		local result = self:SerializeToTable()
	    for i=1, #result do
	        local type_ = result[i].type
	        local full_name = result[i].full_name
	        local comment = result[i].comment
	        local value = result[i].value

	        file:write(string.format("%s@%s@%s:%s\n", type_, full_name, comment, value and tostring(value) or ""))
	    end
		file:close()
		return true
	else
		warn("create file failed", filepath, Application.GetLastError())
		return false
	end
end

return Tmpl