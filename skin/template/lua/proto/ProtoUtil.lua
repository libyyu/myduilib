importluadll "protos"
local proto_file = "pb/config_common.proto"
local FileDescriptor = DynamicProtobuf.FileDescriptor
local Descriptor = DynamicProtobuf.Descriptor
local EnumDescriptor = DynamicProtobuf.EnumDescriptor
local EnumValueDescriptor = DynamicProtobuf.EnumValueDescriptor
local FieldDescriptor = DynamicProtobuf.FieldDescriptor
local Message = DynamicProtobuf.Message

local ProtoUtils = FLua.FinalClass("ProtoUtils")

function ProtoUtils:__constructor()
    self.m_MessageWrapperMap = nil
    self.m_MessageFileWrapper = nil
    self.m_EnumCacheMap = nil
    self.m_MessageBinaryDataMap = {}
    self.m_MessagePosInfoMap = {}
    self.m_MessageMap = {}
    self.m_PbName = ""
    self.m_DataPath = ""
end

function ProtoUtils.CreateWithPath(path, name)
    local self = ProtoUtils()
    self.m_DataPath = path
    self.m_PbName = name
    return self
end

function ProtoUtils:LoadProtoFileIfNeed()
    if not self.m_MessageWrapperMap then
        self.m_MessageWrapperMap, self.m_MessageFileWrapper = require "proto.DynamicProtobufWrapper".LoadFile(self.m_PbName)
    end
end

function ProtoUtils:ConstructMessageDataInner(message_name)
    self:LoadProtoFileIfNeed()
    local data = self.m_MessageBinaryDataMap[message_name]
    if data then return end
    data = GameUtil.ReadFileAllContent(self.m_DataPath..string.lower(message_name)..".bin")
    if not data then
         warn("Failed to read pb data:" .. message_name)
    end
    self.m_MessageBinaryDataMap[message_name] = data
    self.m_MessagePosInfoMap[message_name] = {}
    if not data then
        return
    end
    local posMap = self.m_MessagePosInfoMap[message_name]
    local pos = 1
    while pos <= #data do
        local br = BinaryReader.CreateFromLuaString(data:sub(pos, pos+7))
        local tid = br:ReadInt32()
        local len = br:ReadInt32()
        posMap[tid] = pos
        pos = pos + 8 + len
    end
end

--[[
	根据模板名称，读取index=index的数据
]]
function ProtoUtils:GetTemplateByIndex( message_name, index)
    if index <= 0 then return nil end
    self:LoadProtoFileIfNeed()
    -- 先检查缓存池里面有没有这个消息
    local msg = self.m_MessageMap[message_name] and self.m_MessageMap[message_name][index]
    if msg then return msg end
    local msgClass = self.m_MessageWrapperMap[message_name]
    if not msgClass then
        warn("can not find message type = " .. message_name .. ", tid = " .. index)
        return nil
    end

    self:ConstructMessageDataInner(message_name)
    local data = self.m_MessageBinaryDataMap[message_name]
    if not data or #data == 0 then
        return nil
    end
    local pos = self.m_MessagePosInfoMap[message_name][index]
    if not pos then
        warn("can not find message type2 = " .. message_name .. ", tid = " .. index)
        return nil
    end
    local br = BinaryReader.CreateFromLuaString(data:sub(pos, pos+7))
    local _, len = br:ReadInt32(), br:ReadInt32()
    local msg = msgClass()
    msg:ParseFromString(string.sub(data, pos+8, pos+8+len-1))
    if not self.m_MessageMap[message_name] then
        self.m_MessageMap[message_name] = setmetatable({}, {__mode="v"})
    end
    self.m_MessageMap[message_name][index] = msg
    return msg
end

--[[
    获取模板的所有index
]]

function ProtoUtils:GetTemplateIndexes(message_name)
    self:ConstructMessageDataInner(message_name)
    local rt = {}
    for i,_ in pairs(self.m_MessagePosInfoMap[message_name]) do
        table.insert(rt, i)
    end
    return rt
end

function ProtoUtils:GetTmplClass(message_name)
    self:LoadProtoFileIfNeed()
    return self.m_MessageWrapperMap[message_name]
end

function ProtoUtils:GetAllTemplate()
    self:LoadProtoFileIfNeed()
    local result = {}
    for name, m in pairs(self.m_MessageWrapperMap) do
        if type(m) == "table" and m.__pDescriptor then
            table.insert(result, m)
        end
    end

    return result
end

function ProtoUtils:GetEnumTable(name)
	self:LoadProtoFileIfNeed()
    return self.m_MessageWrapperMap[name] or {}
end

function ProtoUtils:GetMessageExtensionValue(name)
    self:LoadProtoFileIfNeed()
    for fileName, v in pairs(self.m_MessageFileWrapper) do
        for vname, value in pairs(v.MessageExtension) do
            if vname == name then
                return value
            end
        end
    end
end

function ProtoUtils:GetEnumSourceLocation(name, fieldNameOrfieldValue)
    self:LoadProtoFileIfNeed()
    if self.m_EnumCacheMap and self.m_EnumCacheMap[name] then
        local fileName = self.m_EnumCacheMap[name]
        local pEnumDescriptor = self.m_MessageFileWrapper[fileName].Enums[name]
        local pEnumValueDescriptor
        if type(fieldNameOrfieldValue) == "number" then
            pEnumValueDescriptor = EnumDescriptor.FindValueByNumber(pEnumDescriptor, fieldNameOrfieldValue)
        elseif IsInt64(fieldNameOrfieldValue) then
            if type(fieldNameOrfieldValue) == "table" then
                pEnumValueDescriptor = EnumDescriptor.FindValueByNumber(pEnumDescriptor, fieldNameOrfieldValue:tostring())
            else
                pEnumValueDescriptor = EnumDescriptor.FindValueByNumber(pEnumDescriptor, fieldNameOrfieldValue)
            end
        elseif type(fieldNameOrfieldValue) == "string" then
            pEnumValueDescriptor = EnumDescriptor.FindValueByName(pEnumDescriptor, fieldNameOrfieldValue)
        else
            error(("bad argument #3, expected 'number' or 'string', got '%s'"):format(type(fieldNameOrfieldValue)))
        end
        if pEnumValueDescriptor then
            return EnumValueDescriptor.GetSourceLocation(pEnumValueDescriptor)
        end
    end
    self.m_EnumCacheMap = {}
    for fileName, v in pairs(self.m_MessageFileWrapper) do
        for enum_name, pEnumDescriptor in pairs(v.Enums) do
            if enum_name == name then
                self.m_EnumCacheMap[name] = fileName
                local pEnumValueDescriptor
                if type(fieldNameOrfieldValue) == "number" then
                    pEnumValueDescriptor = EnumDescriptor.FindValueByNumber(pEnumDescriptor, fieldNameOrfieldValue)
                elseif IsInt64(fieldNameOrfieldValue) then
                    if type(fieldNameOrfieldValue) == "table" then
                        pEnumValueDescriptor = EnumDescriptor.FindValueByNumber(pEnumDescriptor, fieldNameOrfieldValue:tostring())
                    else
                        pEnumValueDescriptor = EnumDescriptor.FindValueByNumber(pEnumDescriptor, fieldNameOrfieldValue)
                    end
                elseif type(fieldNameOrfieldValue) == "string" then
                    pEnumValueDescriptor = EnumDescriptor.FindValueByName(pEnumDescriptor, fieldNameOrfieldValue)
                else
                    error(("bad argument #3, expected 'number' or 'string', got '%s'"):format(type(fieldNameOrfieldValue)))
                end
                if pEnumValueDescriptor then
                    return EnumValueDescriptor.GetSourceLocation(pEnumValueDescriptor)
                end
            end
        end
    end
end


--[[
	清空缓存
]]

function ProtoUtils:Cleanup()
    self.m_MessageWrapperMap = nil
    self.m_MessageFileWrapper = nil
    self.m_MessageBinaryDataMap = {}
    self.m_MessagePosInfoMap = {}
    self.m_MessageMap = {}
    self.m_EnumCacheMap = nil
end


function ProtoUtils:GetEnumTableInMessage(msgName, enumTypeName)
    return self.m_MessageWrapperMap[msgName] and self.m_MessageWrapperMap[msgName][enumTypeName] or {}
end

-- 取得any message
function ProtoUtils:GetUnpackedAnyMessage(msg)
    local pathList = string.split(msg.type_url, ".")
    local anyType = pathList[#pathList]
    local unpackedMessage = self.m_MessageWrapperMap[anyType]()
    unpackedMessage:ParseFromString(msg.value)
    return unpackedMessage
end

DynamicProtobuf.SetReadCallback(function (filename)
	local data, size = _G.LoadAsset(filename, nil)
	return data, size
end)
_G.config_common = ProtoUtils.CreateWithPath(".", proto_file)


return ProtoUtils