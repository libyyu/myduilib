local function testBase()
	print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>begin test base_t", base_t)
	print(base_t, getmetatable(base_t), base_t.class(), base_t.__name, base_t.__index, base_t.print)
	print("filed of base_t")
	for k,v in pairs((base_t)) do
		print(k, v)
	end
	print("---")
	for k,v in pairs(getmetatable(base_t) or {}) do
		print(k, v)
	end
	local b = base_t()
	base_t.print(b)

	print("b = ", b, b.__name)
	print(">>>>>")
	b:print()
	b:print2()
	base_t.print(b)
	print(base_t.t)
	print(b:tryget('print'), b.isnil)
	--b:delete()
	print("b.isnil", b.isnil)
	-- print(b:tryget('print'), b.isnil)
	--print("b",b)
	delbase(b)
	b:delete()
	
	--b:print()
	print(collectgarbage("count"))

	--b:delete()

	print(collectgarbage("count"))

	print(collectgarbage("collect"))

	print(collectgarbage("count"))
	print("end test base_t", b.isnil)
end

testBase()

local function testFoo()
	print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>begin test foo_t")
	print(foo_t, foo_t.__name, foo)
	print("filed of foo_t")
	for k,v in pairs(foo_t) do
		print(k, v)
	end
	local f = foo_t()
	f:base_print()
	print("f = ", f, foo())
	print("f.num", f.num, f.fprint)
	f.num = 18
	print("after set f.num", f.num)
	f:fprint()
	print("f.counter", f.counter)
	f.counter = 4
	f.fprint2()
	f:print()
	print("f.flag", f.flag)
	f.writeonly = 10
	printwriteonly()
	--print(f.writeonly)
	--f.flag = 1
	f:delete()

	local o = foo2()
	print(o)
	print(o:as(boo_t.class()))
	debug_log("a")
	print("end test foo_t")

end

testFoo()

local player = 
{
	speak = function(self, what,...)
		print(self," say ", what,...)
	end,
}

return player