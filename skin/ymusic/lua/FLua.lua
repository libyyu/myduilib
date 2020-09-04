--[[
    FLua
]]
local getmetatable = getmetatable
local setmetatable = setmetatable
local error = error
local print = print
local warn = warn or print
local tonumber = tonumber
local _G = _G

local _, _, majorv, minorv, rev = string.find(_VERSION, "(%d).(%d)[.]?([%d]?)")
local VersionNumber = tonumber(majorv) * 100 + tonumber(minorv) * 10 + (((string.len(rev) == 0) and 0) or tonumber(rev))
-- Declare current version number.
local TX_VERSION = VersionNumber
local TX_VERSION_510 = 510
local TX_VERSION_520 = 520
local TX_VERSION_530 = 530

local _M = {}
do
    _M.__all_class = {}
    _M.__forward_class = {}
    _M.defaultClassName = function()
        local start = 1
        local toFind = "Anonymous" .. start
        while true do
            if _M.__all_class[toFind] then
                start = start + 1
                toFind = "Anonymous" .. start
            else
                return toFind
            end
        end
        return "Anonymous" .. start
    end

    _M.Class = function(parentClass, className, classOption)
        if(type(className) ~= "string") then
            error("register_class argument #2 string expected, but get " .. type(className), 1)
        end
        if _M.__all_class[className] then
            error("dumplicate define class " .. className, 1)
        end
        if parentClass then
            local parentTypeMeta = getmetatable(parentClass)
            if parentTypeMeta.__options.type == "final" then
                error("can not extend final class " .. tostring(parentTypeMeta.__class), 1)
            end

            if classOption and classOption.type == "static" then
            	if parentTypeMeta.__options.type ~= "static" and parentClass.__cname ~= "FBaseObject" then
	                error("can not extend non-static class " .. tostring(parentTypeMeta.__class), 1)
	            end
            end
        end
        local cls
        -- inherited from Lua Object
        if parentClass then
            cls = { __parent = parentClass }
        else
            cls = { }
        end

        local typeMeta = {
            __cname = className,
            __class = cls,
            __check = true,
            __options = classOption or {}
        }

        cls.__cname = className

        cls.__tryget = function(t, k, check)
            local function get(m, k, check)
                local attr = t.__attributes[k]
                if t.__fields[k] then
                    return attr
                end

                local v = rawget(m, k)
                if v then
                    return v
                end

                local pcls = m.__parent
                while pcls ~= nil do
                    v = get(pcls, k, check)
                    if v ~= nil then
                        return v
                    end

                    pcls = pcls.__parent
                end

                if check and not t.__fields[k] then
                    error("attribute \""..k.."\" not found in ".. tostring(cls), 2)
                end

                return nil
            end
            return get(getmetatable(t), k, check)
        end
        cls.__index = function(t, k)
            return cls.__tryget(t, k, true)
        end
        cls.__newindex = function(t, k, v)
            if k == "__cname" or
                k == "__attributes" or
                k == "__fields" or
                k == "__tryget" or
                k == "__class" or
                k == "__constructor" or
                k == "__gc" or
                k == "__destructor" or
                k == "__pointer" then
                error("field: '" .. k .. "' not enable for class '" .. tostring(cls) .. "'", 1)
            end

            if typeMeta.__check then
                if not t.__fields[k] then
                    error(tostring(cls) .." do not have field \""..k.."\" !!", 1)
                end
            end
            t.__attributes[k] = v
            t.__fields[k] = true
        end

        local typeName = "\"Class#" .. className .. "(".. tostring(cls) .. ")\""
        typeMeta.__tostring = function(_)
            return typeName
        end
        typeMeta.__index = function(c, k)
            if k == "__parent" then
                return rawget(c, "__parent")
            end
            local pcls = c
            while pcls ~= nil do
                v = rawget(pcls, k)
                if v ~= nil then
                    return v
                end

                pcls = pcls.__parent
            end
            return nil
        end
        typeMeta.__call = function()
            if typeMeta.__options.type == "abstract" then
                error("can not instantiate abstract class " .. tostring(cls), 1)
            elseif typeMeta.__options.type == "static" then
                error("can not instantiate static class " .. tostring(cls), 1)
            end
            local instance = {__class = cls, __attributes={}, __fields={}}
            instance.__pointer = tostring(instance)
            
            setmetatable(instance, cls)

            local __tostring = cls.__tryget(instance, "toString", false)
            cls.__tostring = function(t)
                if __tostring then 
                    return __tostring(t)
                else
                    local pointer = t.__pointer
                    local objName = "\"" .. className .. "(".. pointer .. ")\""
                    return objName
                end
            end

            do
                local lt = cls.__tryget(instance, "__lt", false)
                if lt then
                    cls.__lt = lt
                end
                local le = cls.__tryget(instance, "__le", false)
                if le then
                    cls.__le = le
                end
                local eq = cls.__tryget(instance, "__eq", false)
                if eq then
                    cls.__eq = eq
                end
                local add = cls.__tryget(instance, "__add", false)
                if add then
                    cls.__add = add
                end
                local sub = cls.__tryget(instance, "__sub", false)
                if sub then
                    cls.__sub = sub
                end
                local mul = cls.__tryget(instance, "__mul", false)
                if mul then
                    cls.__mul = mul
                end
                local div = cls.__tryget(instance, "__div", false)
                if div then
                    cls.__div = div
                end
            end

            local clsList = {}
            local pcls = cls
            while pcls ~= nil do
                table.insert(clsList, pcls)
                pcls = pcls.__parent
            end

            do --gc
                local function destructor_a(o, c)
                    local __destructor = rawget(c, "__destructor")
                    if __destructor then
                        __destructor(o)
                    end
                end
                local function __destructor(o)
                    for i = 1, #clsList do
                        local _cls = clsList[i]
                        destructor_a(o, _cls)
                    end
                end

                if TX_VERSION < TX_VERSION_520 then
                    -- Create a empty userdata with empty metatable.
                    -- And mark gc method for destructor.
                    local proxy = newproxy(true)
                    getmetatable(proxy).__gc = function (o)
                        __destructor(instance)
                    end
        
                    -- Hold the one and only reference to the proxy userdata.
                    rawget(instance, "__gc", proxy)
                else
                    -- Directly set __gc field of the metatable for destructor of this object.
                    cls.__gc = function(o)
                        __destructor(instance)
                    end
                end
            end

            do --constructor
                local function ctor(o, c)
                    local __constructor = rawget(c, "__constructor")
                    if __constructor then
                        __constructor(o)
                    end
                end
                typeMeta.__check = false
                for i = #clsList, 1, -1 do
                    local _cls = clsList[i]
                    ctor(instance, _cls)
                end
                typeMeta.__check = true
            end

            return instance
        end
        setmetatable(cls, typeMeta)
        _M.__all_class[className] = cls
        _M.__forward_class[className] = nil
        return cls
    end

    _M.ForwardClass = function(className)
        if _M.__all_class[className] then
            return _M.__all_class[className]
        end
        if _M.__forward_class[className] then
            return _M.__forward_class[className]
        end
        _M.__forward_class[className] = {}
    end
end

local FBaseObject = _M.Class(nil, "FBaseObject")
do
    function FBaseObject:GetClass()
        return self.__class
    end
    function FBaseObject:GetParent()
        return self.__class.__parent
    end
    function FBaseObject:GetPointer()
        return self.__pointer
    end
    function FBaseObject:tryget(name)
        return self:GetClass().__tryget(self, name, false)
    end

    function FBaseObject:is(class)
        local pcls = self.__class
        while pcls do
            if pcls == class then
                return true
            else
                pcls = pcls.__parent
            end
        end
        return false
    end
end

FLua = {}
function FLua.Class(a, b, classOption)
    local classname, parentclass = nil, nil
    if not b then
        if not a then
            classname = _M.defaultClassName()
            parentclass = FBaseObject
        elseif type(a) == "string" then
            classname = a
            parentclass = FBaseObject
        elseif type(a) == "table" then
            local mt = getmetatable(a)
            if rawget(mt, "__cname") then
                classname = _M.defaultClassName()
                parentclass = a
            else
                error("arguement #1 must be (string, table, nil),but got "..type(a))
            end
        end
    else
        if type(a) ~= "table" then
            error("arguement #1 must be table,but got "..type(a))
        end
        local mt = getmetatable(a)
        if not rawget(mt, "__cname") then
            error("arguement #1 must be FBaseObject,but got "..type(a))
        end
        if type(b) ~= "string" then
            error("arguement #2 must be string,but got "..type(b))
        end
        classname = b
        parentclass = a
    end
    return _M.Class(parentclass, classname, classOption)
end

function FLua.Abstract(a, b)
    classOption = {type="abstract"}
    return FLua.Class(a, b, classOption)
end

function FLua.FinalClass(a, b)
    classOption = {type="final"}
    return FLua.Class(a, b, classOption)
end

function FLua.StaticClass(a, b)
	classOption = {type="static"}
    return FLua.Class(a, b, classOption)
end

function FLua.ForwardClass(className)
    return _M.ForwardClass(className)
end

function FLua.IsClass( cls )
    assert(type(cls) == "table")
    return getmetatable(cls).__class == cls
end
function FLua.Is(obj)
    local mt = getmetatable(obj)
    return mt and getmetatable(mt) and not not getmetatable(mt).__class
end

if false then
    local o = FBaseObject()
    print(o, o:GetClass())
    print(FBaseObject)
    local FTest = FLua.FinalClass()
    local FTest2 = FLua.FinalClass()
    function FTest:__constructor()
        self.a = 1
    end
    function FTest:toString()
        return "<FTest>" .. self:GetPointer()
    end
    function FTest:__destructor()
        print(self, "__destructor")
    end
    local t = FTest()
    local b = FTest()
    print(FTest)
    print(t:GetClass())
    print(t)
    print(t.a)
    t.a = 10
    print(t.a)
    print(t:is(FTest))
    print(t:is(FBaseObject))
    print(t:is(FTest2))
    print(t, b)
    t = nil
    collectgarbage()

    local A = FLua.Abstract("A")
    local a = A()
end

return FLua