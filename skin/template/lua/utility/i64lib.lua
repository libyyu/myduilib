local i64lib = {}

local mt = {magic64=true}

function i64lib.new(v, unsigned)
	local r = {value=v, unsigned=unsigned}
	setmetatable(r, mt)
	return r
end

function mt:__tostring()
	return string.format("%s", self.value)
end

function mt:__lt(ths)
	return helper.Int64Compare(self, ths) == -1
end
function mt:__le(ths)
	return helper.Int64Compare(self, ths) <= 0
end
function mt:__eq(ths)
	return helper.Int64Compare(self, ths) == 0
end
function mt:__add(ths)
	local v = helper.Int64Add(self, ths)
	self.value = v.value
	return self 
end
function mt:__sub(ths)
	local v = helper.Int64Sub(self, ths)
	self.value = v.value
	return self 
end
function mt:__mul(ths)
	local v = helper.Int64Mul(self, ths)
	self.value = v.value
	return self 
end
function mt:__div(ths)
	local v = helper.Int64Div(self, ths)
	self.value = v.value
	return self 
end
function mt:__mod(ths)
	local v = helper.Int64Mod(self, ths)
	self.value = v.value
	return self 
end
function mt:__unm()
	local v = helper.Int64Unm(self)
	self.value = v.value
	return self 
end
function mt:__pow(ths)
	local v = helper.Int64Pow(self, ths)
	self.value = v.value
	return self 
end

function _G.make_i64(v, unsigned)
	return i64lib.new(v, unsigned)
end
function _G.get_i64(v)
	return v.value, v.unsigned
end


