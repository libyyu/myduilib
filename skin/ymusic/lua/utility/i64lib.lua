local i64lib = {}

local mt = {magic64=true}

local function param64(v)
	if type(v) == "table" and getmetatable(v) and getmetatable(v).magic64 then
		return v.value
	elseif type(v) == "string" or type(v) == "number" then
		return v
	else
		error("param is not valid")
	end 
end

function i64lib.new(v, unsigned)
	local r = {value=v, unsigned=unsigned}
	setmetatable(r, mt)

	function r:todouble()
		return lib64.Int64ToInt(self.value)
	end
	function r:int()
		return math.floor(lib64.Int64ToInt(self.value))
	end
	function r:equal(ths)
		return lib64.Int64Compare(self.value, param64(ths)) == 0
	end
	function r:ToString()
		return lib64.Int64ToString(self.value)
	end

	return r
end

function mt:__tostring()
	return string.format("i64:%s", lib64.Int64ToString(self.value))
end

function mt:__lt(ths)
	return lib64.Int64Compare(self.value, param64(ths)) == -1
end
function mt:__le(ths)
	return lib64.Int64Compare(self.value, param64(ths)) <= 0
end
function mt:__eq(ths)
	return lib64.Int64Compare(self.value, param64(ths)) == 0
end
function mt:__add(ths)
	print("add")
	local v = lib64.Int64Add(self.value, param64(ths))
	self.value = v
	return self
end
function mt:__sub(ths)
	local v = lib64.Int64Sub(self.value, param64(ths))
	self.value = v
	return self 
end
function mt:__mul(ths)
	local v = lib64.Int64Mul(self.value, param64(ths))
	self.value = v
	return self 
end
function mt:__div(ths)
	local v = lib64.Int64Div(self.value, param64(ths))
	self.value = v
	return self 
end
function mt:__mod(ths)
	local v = lib64.Int64Mod(self.value, param64(ths))
	self.value = v
	return self 
end
function mt:__unm()
	local v = lib64.Int64Unm(self.value)
	self.value = v
	return self 
end
function mt:__pow(ths)
	local v = lib64.Int64Pow(self.value, param64(ths))
	self.value = v
	return self 
end

function mt:bnot()
	local v = lib64.bnot(self.value)
	self.value = v
	return self 
end
function mt:band(ths)
	local v = lib64.band(self.value, param64(ths))
	self.value = v
	return self 
end
function mt:bor(ths)
	local v = lib64.bor(self.value, param64(ths))
	self.value = v
	return self 
end
function mt:bxor(ths)
	local v = lib64.bxor(self.value, param64(ths))
	self.value = v
	return self 
end


function _G.make_i64(v, unsigned)
	return i64lib.new(v, unsigned)
end
function _G.get_i64(v)
	return v.value, v.unsigned
end


