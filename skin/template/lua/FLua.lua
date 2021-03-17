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
            __options = classOption or {}
        }

        cls.__cname = className

        cls.__tryget = function(t, k, check)
            local function get(t, c, k, check)
                local v = rawget(t, k)
                if v then
                    return v
                end
                local attr = t.__attributes[k]
                if t.__fields[k] then
                    return attr
                end

                local v = rawget(c, k)
                if v then
                    return v
                end

                local pcls = c.__parent
                while pcls ~= nil do
                    v = get(t, pcls, k, check)
                    if v ~= nil then
                        return v
                    end

                    pcls = pcls.__parent
                end

                if check and not t.__fields[k] and k ~= "__no_strict_access_check" then
                    error("attribute \""..k.."\" not found in ".. tostring(cls), 2)
                end

                return nil
            end
            return get(t, getmetatable(t), k, check)
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
                k == "__pointer" or
                k == "__gcproxy" or
                k == "__no_strict_access_check" then
                error("field: '" .. k .. "' not enable for class '" .. tostring(cls) .. "'", 1)
            end
            
            if not rawget(t, "__no_strict_access_check") then
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
            local instance = {__class = cls, __attributes={}, __fields={}, __no_strict_access_check=true}
            instance.__pointer = tostring(instance)
            
            setmetatable(instance, cls)

            if not rawget(cls, "__tostring") then
                local toString = rawget(cls, "toString")
                cls.__tostring = function(t)
                    if toString then 
                        return toString(t)
                    else
                        local pointer = t.__pointer
                        local objName = "\"" .. className .. "(".. pointer .. ")\""
                        return objName
                    end
                end
            end

            local clsList = {}
            local pcls = cls
            while pcls ~= nil do
                table.insert(clsList, pcls)
                pcls = pcls.__parent
            end

            if not rawget(cls, "__destructor") then --gc
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

                local createGcProxy
                if TX_VERSION < TX_VERSION_520 then
                    function createGcProxy (callback)
                        local proxy = newproxy(true)
                        local meta = getmetatable(proxy)
                        meta.__gc = callback
                        return proxy
                    end
                else
                    function createGcProxy (callback)
                        local proxy = {}
                        local meta = {}
                        meta.__gc = callback
                        return setmetatable(proxy, meta)
                    end
                end
                rawset(instance, "__gcproxy", createGcProxy(function(_)
                    __destructor(instance)
                end))
            end

            do --constructor
                local function ctor(o, c)
                    local __constructor = rawget(c, "__constructor")
                    if __constructor then
                        __constructor(o)
                    end
                end
                
                for i = #clsList, 1, -1 do
                    local _cls = clsList[i]
                    ctor(instance, _cls)
                end
                rawset(instance, "__no_strict_access_check", nil)
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
        self.c = nil
    end
    function FTest:toString()
        return "<FTest>--" .. self:GetPointer()
    end
    function FTest:__destructor()
        print(self, "__destructor")
    end
    function FTest:__lt(ths)
        return true 
    end
    local t = FTest()
    local b = FTest()
    print(FTest)
    print(t:GetClass())
    print(t)
    print(t.a)
    t.a = 10
    print(t.a, b.a)
    t.c = 0
    print(t.c)
    print(t:is(FTest))
    print(t:is(FBaseObject))
    print(t:is(FTest2))
    print(t, b)

    print(t < b)
    --t.b = 10
    t = nil
    collectgarbage()

    local A = FLua.Abstract("A")
    local a = A()
end

return FLua