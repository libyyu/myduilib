--[[
	封装 DynamicProtobuf 接口以便在 Lua 中使用
	
	MessageClass: 一个 Message 类型，对应一个 Descriptor
		<NestedMessageName> 读内部 Message 类型
		<NestedEnumValueName> 读内部 Enum 值
		__call 构造 Message 实例
			返回 Message 实例 (message)
		GetDescriptor 取得 Descriptor 指针 (lightuserdata)
		FindFieldDescriptor 取得字段 FieldDescriptor 指针 (lightuserdata)
	
	message: 一个 Message 实例 (table)
		<fieldName> 读写字段值
			等效于 get, set
		__tostring 取得格式化文本
		__gc 销毁 Message
		get 读字段值
			参数为字段名
				对基础字段，读出其值
				对 Enum 字段，读出其数值
				对 Message 字段，读出 message
				对 repeated 字段，读出 repeated_field
		set 写字段值
			参数为字段名和值
				对基础字段，写入其值
				对 Enum 字段，写入其数值
				对 Message 字段，不可写入
				对 repeated 字段，不可写入
		GetMessage 取得 MessageClass
		SerializeToString 序列化
			返回序列化数据 (string)
		ParseFromString 反序列化
			参数为序列化数据 (string)
		HasField 查询可选字段是否有值
			参数为字段名
		SetInParent 是否有必要？
			对 Message 字段，标记必选 Message 字段已存在
	
	repeated_field: repeated 字段值
		[index] 读写序号 index 上的值
			等效于 get, set
		append 添加非 Message 类型子字段
			参数为子字段值
		add 添加 Message 类型子字段，无其他参数，子字段初始化为其默认值
		remove_last 删除最后一个子字段
		
		为兼容旧 pb lua 库，需用 "#" 取得 repeated 字段长度，且 repeated 字段为 table
		为此，只能填充 table，放弃数组的访问检查
		注意：用户需自行保证访问规则，不可写入 message 类型子字段，也不可写入类型不匹配的字段值
]]
importluadll "protos"
local FileDescriptor = DynamicProtobuf.FileDescriptor
local Descriptor = DynamicProtobuf.Descriptor
local EnumDescriptor = DynamicProtobuf.EnumDescriptor
local EnumValueDescriptor = DynamicProtobuf.EnumValueDescriptor
local FieldDescriptor = DynamicProtobuf.FieldDescriptor
local Message = DynamicProtobuf.Message
local type = type
local setmetatable = setmetatable
local getmetatable = getmetatable
local error = error
local newproxy = newproxy
local tostring = tostring
local rawset = rawset
local pairs = pairs
local print = print
local string = string
local _G = _G


local WrapperHelper = FLua.StaticClass("WrapperHelper")
do
	-- 缓存 messageClass，[pDescript] = messageClass
	local l_Wrapped_MessageClass_cache = nil
	
	--清空缓存

	---@return void
	function WrapperHelper.ClearAllCache()
		l_Wrapped_MessageClass_cache = nil
	end
	
	local l_messageClass_meta = nil
	
	--MessageClass: __pDescriptor 为 Descriptor 指针，__memebers 为内部成员 (包含 nested message, enum value name)

	function WrapperHelper.Wrap_MessageClass(pDescriptor)
		l_Wrapped_MessageClass_cache = l_Wrapped_MessageClass_cache or {}
		local cachedMessageClass = l_Wrapped_MessageClass_cache[pDescriptor]
		if cachedMessageClass then
			return cachedMessageClass
		end
		
		if not l_messageClass_meta then
			l_messageClass_meta =
			{
				__index = WrapperHelper._MessageClass__index,
				__newindex = WrapperHelper._MessageClass__newindex,
				__call = WrapperHelper._MessageClass__call,
			}
		end
		local MessageClass = setmetatable({__pDescriptor = pDescriptor, __members = WrapperHelper._MessageClass_MakeMembers(pDescriptor) }, l_messageClass_meta)
		l_Wrapped_MessageClass_cache[pDescriptor] = MessageClass
		return MessageClass
	end
	
	--[[
		message:
			meta.__MessageClass 为 MessageClass
			meta.__pMessage 为 Message 指针
			meta.__pDescriptor 为 Descriptor
			meta.__cachedMembers 为缓存的子 message 及 repeated field
			meta.__gcref 引用住上层对象，避免其指针释放
		param gcref: 通过 gcref 递归引用，应引用到最初创建的 pMessage
		
		为了将修改后 repeated field 值写回 message，增加了方法 _WriteBack
	]]

	function WrapperHelper.Wrap_message(pMessage, gcref)
		local pDescriptor = Message.GetDescriptor(pMessage)
		local message_meta =
		{
			__index = WrapperHelper._message__index,
			__newindex = WrapperHelper._message__newindex,
			__tostring = WrapperHelper._message__tostring,
			
			__MessageClass = WrapperHelper.Wrap_MessageClass(pDescriptor),
			__pMessage = pMessage,
			__pDescriptor = pDescriptor,
			__cachedMembers = false,		--避免触发 __index
		}
		
		if gcref then
			message_meta.__gcref = gcref
		else
			--无 gcref 表示是创建出的 message，需要 gc
			local gcHandler = newproxy(true)
			getmetatable(gcHandler).__gc = function ()
				Message.Delete(pMessage)
			end
			message_meta.__gcref = gcHandler
		end
		
		local message = {}
		
		return setmetatable(message, message_meta)
	end
	
	--[[
		repeated field:
			self.__pMessage 为 Message 指针
			self.__pField 为 FieldDescriptor
			self.__gcref 引用住上层对象，避免其指针释放
			self[i] 元素值
		为了将修改后的值写回 message，增加了方法 _WriteBack
	]]

	function WrapperHelper.Wrap_repeated_field(message, pFieldDescriptor)
		local pMessage = getmetatable(message).__pMessage
		
		local repeated_field_meta =
		{
			__index = WrapperHelper._repeated_field__index,
			__newindex = WrapperHelper._repeated_field__newindex,
			
			__pMessage = pMessage,
			__pField = pFieldDescriptor,
			__gcref = message,
		}
		
		local repeatedField = {}
		
		--填充 table
		local isMessage = FieldDescriptor.is_message(pFieldDescriptor)
		for i = 1, Message.FieldSize(pMessage, pFieldDescriptor) do
			local v = Message.GetRepeated(pMessage, pFieldDescriptor, i - 1)
			if isMessage then
				repeatedField[i] = WrapperHelper.Wrap_message(v, repeatedField)
			else
				repeatedField[i] = v
			end
		end
		
		return setmetatable(repeatedField, repeated_field_meta)
	end
	

	function WrapperHelper.Wrap_Map_field(message, pFieldDescriptor)
		local pMessage = getmetatable(message).__pMessage
		local map = {}
		-- if not FieldDescriptor.is_message(pFieldDescriptor) then
		-- 	error("the map field is not message type!!")
		-- end
		-- pFieldDescriptor 不等于 kvDescriptor !!!!
		for i = 1, Message.FieldSize(pMessage, pFieldDescriptor) do
			local kvMessage = Message.GetRepeated(pMessage, pFieldDescriptor, i - 1)
			local kvDescriptor = Message.GetDescriptor(kvMessage)
			local keyFieldDescriptor = Descriptor.FindFieldByName(kvDescriptor, "key")
			local valueFieldDescriptor = Descriptor.FindFieldByName(kvDescriptor, "value")
			local valueIsMessage = FieldDescriptor.is_message(valueFieldDescriptor)

			local v = Message.Get(kvMessage, valueFieldDescriptor)
			local key = Message.Get(kvMessage, keyFieldDescriptor)
			if valueIsMessage then
				map[key] = WrapperHelper.Wrap_message(v, map)
			else
				map[key] = v
			end
		end
		
		return setmetatable(map, {
			__pMessage = pMessage,
			__pField = pFieldDescriptor,
			__gcref = message,
		})
	end
	-----------------------------------------------------
	
	--
	-- Helper
	--
	
	function WrapperHelper._FieldEnumDefines(pEnumDescriptor, t)
		local ENUM_NAME = EnumDescriptor.name(pEnumDescriptor)
		t[ENUM_NAME] = t[ENUM_NAME] or {}
		
		local ENUM = t[ENUM_NAME]
		local max_value = 0
		for iEnumValue = 1, EnumDescriptor.value_count(pEnumDescriptor) do
			local pEnumValueDescriptor = EnumDescriptor.value(pEnumDescriptor, iEnumValue - 1)
			local name = EnumValueDescriptor.name(pEnumValueDescriptor)
			local number = EnumValueDescriptor.number(pEnumValueDescriptor)
			ENUM[name] = number --建议这种，后期还是采用枚举名称.枚举定义名的形式访问吧
			max_value = number
		end

		--取枚举最后一个值+1，避免定义xxx_COUNT
		local count_name = EnumValueDescriptor.name(pEnumDescriptor).."_COUNT"
		if not t[count_name] then
			local last_index = EnumDescriptor.value_count(pEnumDescriptor)-1
			local pEnumValueDescriptor = EnumDescriptor.value(pEnumDescriptor, last_index)
			local number = EnumValueDescriptor.number(pEnumValueDescriptor)
			ENUM[count_name] = number + 1
		end

		--跟服务器C++一致， 生成一个XXX_MAX
		if not ENUM[ENUM_NAME .. "_MAX"] then
			ENUM[ENUM_NAME .. "_MAX"] = max_value
		end
	end
	
	--
	-- MessageClass
	--
	
	--MessageClass 的内部成员

	function WrapperHelper._MessageClass_MakeMembers(pDescriptor)
		local members = {}
		--子 Message
		for iNestedMessage = 1, Descriptor.nested_type_count(pDescriptor) do
			local pNestedTypeDescriptor = Descriptor.nested_type(pDescriptor, iNestedMessage - 1)
			
			local name = Descriptor.name(pNestedTypeDescriptor)
			local nestedMessageClass = WrapperHelper.Wrap_MessageClass(pNestedTypeDescriptor)
			members[name] = nestedMessageClass
		end
		
		--子 Enum
		for iEnum = 1, Descriptor.enum_type_count(pDescriptor) do
			local pEnumDescriptor = Descriptor.enum_type(pDescriptor, iEnum - 1)
			WrapperHelper._FieldEnumDefines(pEnumDescriptor, members)
		end
		return members
	end
	

	---@param self table
	---@return userdata
	function WrapperHelper._MessageClass_GetDescriptor(self)
		return self.__pDescriptor
	end
	

	function WrapperHelper._MessageClass_FindFieldDescriptor(self, fieldName)
		return Descriptor.FindFieldByName(self.__pDescriptor, fieldName)
	end

	function WrapperHelper._MessageClass_GetName(self)
		return Descriptor.name(self.__pDescriptor)
	end

	function WrapperHelper._MessageClass_GetSourceLocation(self)
		return Descriptor.GetSourceLocation(self.__pDescriptor)
	end
	
	local l_MessageClass_methods = nil
	
	function WrapperHelper._MessageClass__index(self, k)
		if not l_MessageClass_methods then
			l_MessageClass_methods =
			{
				GetDescriptor = WrapperHelper._MessageClass_GetDescriptor,
				FindFieldDescriptor = WrapperHelper._MessageClass_FindFieldDescriptor,
				GenMessageFromAny = WrapperHelper._MessageClass_GenMessageFromAny,
				GetName = WrapperHelper._MessageClass_GetName,
				GetSourceLocation = WrapperHelper._MessageClass_GetSourceLocation,
			}
		end
		
		local method = l_MessageClass_methods[k]
		if method then
			return method
		end
		return self.__members[k]
	end	
	

	function WrapperHelper._MessageClass__newindex(self, k, v)
		error(("invalid write to message class '%s'"):format(Descriptor.full_name(self.__pDescriptor)))
	end
	

	function WrapperHelper._MessageClass__call(self)
		local pMessageProtocol = DynamicProtobuf.GetMessagePrototype(self.__pDescriptor)
		local pMessage = Message.New(pMessageProtocol)
		return WrapperHelper.Wrap_message(pMessage, nil)
	end

	-- Warning TODO: 暂时不可用，请使用ProtocolUtil里面的函数代替
	function WrapperHelper._MessageClass_GenMessageFromAny(self, any)
		local pMessageProtocol = DynamicProtobuf.GetMessagePrototype(self.__pDescriptor)
		local pMessage = Message.New(pMessageProtocol)
		local anyMessage = getmetatable(any).__pMessage
		if Message.UnpackTo(anyMessage, pMessage) then
			return WrapperHelper.Wrap_message(pMessage, nil)
		else
			error("can not generate message from any!!")
		end
	end
	
	--
	-- message
	--
	function WrapperHelper._message_get(self, fieldName)
		local meta = getmetatable(self)
		local pDescriptor = meta.__pDescriptor
		local pFieldDescriptor = Descriptor.FindFieldByName(pDescriptor, fieldName)
		if not pFieldDescriptor then
			error(("no field with name '%s' on Message type '%s'"):format(fieldName, Descriptor.full_name(pDescriptor)))
		end
		
		local pMessage = meta.__pMessage
		
		if FieldDescriptor.is_map(pFieldDescriptor) then
			local cachedMembers = meta.__cachedMembers
			if not cachedMembers then
				cachedMembers = {}
				meta.__cachedMembers = cachedMembers
			end
			
			local cachedMapField = cachedMembers[fieldName]
			if cachedMapField then
				if getmetatable(cachedMapField).__pMessage == pMessage and getmetatable(cachedMapField).__pField == pFieldDescriptor then
					return cachedMapField
				end
			end
			
			local mapField = WrapperHelper.Wrap_Map_field(self, pFieldDescriptor)
			cachedMembers[fieldName] = mapField
			return mapField
		elseif FieldDescriptor.is_repeated(pFieldDescriptor) then
			local cachedMembers = meta.__cachedMembers
			if not cachedMembers then
				cachedMembers = {}
				meta.__cachedMembers = cachedMembers
			end
			
			local cachedRepeatedField = cachedMembers[fieldName]
			if cachedRepeatedField then
				if getmetatable(cachedRepeatedField).__pMessage == pMessage and getmetatable(cachedRepeatedField).__pField == pFieldDescriptor then
					return cachedRepeatedField
				end
			end
			
			local repeatedField = WrapperHelper.Wrap_repeated_field(self, pFieldDescriptor)
			cachedMembers[fieldName] = repeatedField
			return repeatedField
		else
			local v = Message.Get(pMessage, pFieldDescriptor)
			if type(v) == "userdata" then	--message
				local cachedMembers = meta.__cachedMembers
				if not cachedMembers then
					cachedMembers = {}
					meta.__cachedMembers = cachedMembers
				end
				
				local cachedMessageField = cachedMembers[fieldName]
				if cachedMessageField then
					if getmetatable(cachedMessageField).__pMessage == v then
						return cachedMessageField
					end
				end
				
				local message = WrapperHelper.Wrap_message(v, self)
				cachedMembers[fieldName] = message
				return message
			else
				return v
			end
		end
	end
	
	function WrapperHelper._message_set(self, fieldName, value)
		local meta = getmetatable(self)
		local pDescriptor = meta.__pDescriptor
		local pFieldDescriptor = Descriptor.FindFieldByName(pDescriptor, fieldName)
		if not pFieldDescriptor then
			error(("no field with name '%s' on message type '%s'"):format(fieldName, Descriptor.full_name(pDescriptor)))
		end
		
		local pMessage = meta.__pMessage
		Message.Set(pMessage, pFieldDescriptor, value)
	end
	
	function WrapperHelper._message_GetMessage(self)
		local meta = getmetatable(self)
		return meta.__MessageClass
	end
	
	function WrapperHelper._message_SerializeToString(self)
		--(递归)写回 repeated field 字段值
		self:_WriteBack()
		
		local meta = getmetatable(self)
		local pMessage = meta.__pMessage
		return Message.SerializeToString(pMessage)
	end
	
	function WrapperHelper._message_ParseFromString(self, data)
		local meta = getmetatable(self)
		local pMessage = meta.__pMessage
		Message.ParseFromString(pMessage, data)
	end
	
	function WrapperHelper._message_HasField(self, fieldName)
		local meta = getmetatable(self)
		local pMessage = meta.__pMessage
		local pDescriptor = meta.__pDescriptor
		local pFieldDescriptor = Descriptor.FindFieldByName(pDescriptor, fieldName)
		if not pFieldDescriptor then
			return false
		end
		return Message.HasField(pMessage, pFieldDescriptor)
	end

	function WrapperHelper._message_ListFields(self)
		local meta = getmetatable(self)
		local pMessage = meta.__pMessage
		return Message.ListFields(pMessage)
	end

	function WrapperHelper._message_FieldSize(self, fieldName)
		local meta = getmetatable(self)
		local pMessage = meta.__pMessage
		local pDescriptor = meta.__pDescriptor
		local pFieldDescriptor = Descriptor.FindFieldByName(pDescriptor, fieldName)
		if not pFieldDescriptor then
			return 0
		end
		return Message.FieldSize(pMessage, pFieldDescriptor)
	end
	
	function WrapperHelper._message_SetInParent(self)	
	end
	
	function WrapperHelper._message_WriteBack(self)
		local meta = getmetatable(self)
		--只有访问过的字段需要写回
		local cachedMembers = meta.__cachedMembers
		if cachedMembers then
			for fieldName, fieldValue in pairs(cachedMembers) do
				if type(fieldValue) == "table" then
					fieldValue:_WriteBack()
				end
			end
		end
	end
	
	local l_message_methods = nil
	
	function WrapperHelper._message__index(self, k)
		if not l_message_methods then
			l_message_methods =
			{
				get = WrapperHelper._message_get,
				set = WrapperHelper._message_set,
				GetMessage = WrapperHelper._message_GetMessage,
				SerializeToString = WrapperHelper._message_SerializeToString,
				ParseFromString = WrapperHelper._message_ParseFromString,
				HasField = WrapperHelper._message_HasField,
				ListFields = WrapperHelper._message_ListFields,
				FieldSize = WrapperHelper._message_FieldSize,
				
				SetInParent = WrapperHelper._message_SetInParent,
				_WriteBack = WrapperHelper._message_WriteBack,
				tryget = WrapperHelper._message_tryget,
			}
		end

		local method = l_message_methods[k]
		if method then
			return method
		end
		
		return WrapperHelper._message_get(self, k)
	end
	
	function WrapperHelper._message__newindex(self, k, v)
		return WrapperHelper._message_set(self, k, v)
	end
	
	function WrapperHelper._message__tostring(self)
		local meta = getmetatable(self)
		local pMessage = meta.__pMessage
		return Message.DebugString(pMessage)
	end

	function WrapperHelper._message_tryget(self, k)
		return l_message_methods and l_message_methods[k]
	end
	--
	-- repeated field
	--

	function WrapperHelper._repeated_field_append(self, value)
		local meta = getmetatable(self)
		local pMessage = meta.__pMessage
		local pFieldDescriptor = meta.__pField
		Message.Append(pMessage, pFieldDescriptor, value)
		local curLen = Message.FieldSize(pMessage, pFieldDescriptor)
		rawset(self, curLen, value)
	end
	

	---@param self table
	---@return table
	function WrapperHelper._repeated_field_add(self)
		local meta = getmetatable(self)
		local pMessage = meta.__pMessage
		local pFieldDescriptor = meta.__pField
		local pAddedMessage = Message.Add(pMessage, pFieldDescriptor)
		local addedMessage = WrapperHelper.Wrap_message(pAddedMessage, self)
		local curLen = Message.FieldSize(pMessage, pFieldDescriptor)
		rawset(self, curLen, addedMessage)
		return addedMessage
	end
	

	function WrapperHelper._repeated_field_remove_last(self)
		local meta = getmetatable(self)
		local pMessage = meta.__pMessage
		local pFieldDescriptor = meta.__pField
		Message.RemoveLast(pMessage, pFieldDescriptor)
		local curLen = Message.FieldSize(pMessage, pFieldDescriptor)
		for i = #self, curLen+1, -1 do
			rawset(self, i, nil)
		end
	end
	
	function WrapperHelper._repeated_field_WriteBack(self)
		local meta = getmetatable(self)
		local pMessage = meta.__pMessage
		local pFieldDescriptor = meta.__pField
		
		local isMessage = FieldDescriptor.is_message(pFieldDescriptor)
		
		local curLen = Message.FieldSize(pMessage, pFieldDescriptor)
		--写回已有的项
		for i = 1, curLen do
			local v = self[i]
			if isMessage then	--message 类型不能修改子字段值，因此指针本身不能写回
				v:_WriteBack()
			else
				Message.SetRepeated(pMessage, pFieldDescriptor, i - 1, v)
			end
		end
		--把 table.insert 新增的项写回
		for i = curLen+1, #self do
			local v = self[i]
			if isMessage then
				local pSourceMessage = getmetatable(v).__pMessage
				local newMessage = WrapperHelper._repeated_field_add(self)		--注意，回写位置对应是 C++ 对象长度，而非 Lua table 长度
				local pNewMessage = getmetatable(newMessage).__pMessage
				Message.CopyFrom(pNewMessage, pSourceMessage)
			else
				Message.Append(pMessage, pFieldDescriptor, v)		--注意，回写位置对应是 C++ 对象长度，而非 Lua table 长度
			end
		end
	end
	
	local l_repeated_field_methods = nil
	

	function WrapperHelper._repeated_field__index(self, k)
		if not l_repeated_field_methods then
			l_repeated_field_methods =
			{
				append = WrapperHelper._repeated_field_append,
				add = WrapperHelper._repeated_field_add,
				remove_last = WrapperHelper._repeated_field_remove_last,
				
				_WriteBack = WrapperHelper._repeated_field_WriteBack,
			}
		end
		
		local method = l_repeated_field_methods[k]
		if method then
			return method
		end
		
		--字段都在 table 中，所以到达这里时表明 k 值错误
		return nil
		-- error(("bad index key '%s' to repeated field '%s'"):format(tostring(k), FieldDescriptor.full_name(getmetatable(self).__pField)))
	end
	

	function WrapperHelper._repeated_field__newindex(self, k, v)
		--字段都在 table 中，所以到达这里时表明 k 值错误

		error(("bad index key '%s' to repeated field '%s'"):format(tostring(k), FieldDescriptor.full_name(self.__pField)))
	end
end


local DynamicProtobufWrapper = FLua.StaticClass("DynamicProtobufWrapper")
do
	--[[
		清空全局数据，主要是 FileDescriptor 内部数据
	]]

	function DynamicProtobufWrapper.ClearGlobalData()
		DynamicProtobuf.ClearGlobalData()
	end
	
	local l_inited
	local l_filecache = {}
	
	--[[
		读取 FileDescriptor
		param filePath: FileDescriptor 文件路径
		return: 一个表，包含 FileDescriptor 中顶级 Message 和顶级 EnumValue 名
	]]

	function DynamicProtobufWrapper.LoadFile(filePath)
		--加载一份新protos文件时候，清空全局数据
		-- if not l_filecache[filePath] then
		-- 	DynamicProtobufWrapper.ClearGlobalData()
		-- end
		if not l_inited then
			l_inited = true
			DynamicProtobufWrapper.ClearGlobalData()
		end
		l_filecache[filePath] = true
		
		local lib = {}
			
		local pFileDescriptorArray = DynamicProtobuf.LoadFileDescriptor(filePath)
		if not pFileDescriptorArray then
			error("failed to load file descriptor file: " .. tostring(filePath))
		end
		-- mesage
		for iDescriptor = 1, #pFileDescriptorArray do
			local pFileDescriptor = pFileDescriptorArray[iDescriptor]
			for iMessage = 1, FileDescriptor.message_type_count(pFileDescriptor) do
				local pDescriptor = FileDescriptor.message_type(pFileDescriptor, iMessage - 1)
				local name = Descriptor.name(pDescriptor)
				local MessageClass = WrapperHelper.Wrap_MessageClass(pDescriptor)
				lib[name] = MessageClass
			end
		end
		
		-- Enum
		for iDescriptor = 1, #pFileDescriptorArray do
			local pFileDescriptor = pFileDescriptorArray[iDescriptor]
			for iEnum = 1, FileDescriptor.enum_type_count(pFileDescriptor) do
				local pEnumDescriptor = FileDescriptor.enum_type(pFileDescriptor, iEnum - 1)
				WrapperHelper._FieldEnumDefines(pEnumDescriptor, lib)
			end
		end

		return lib
	end

	DynamicProtobufWrapper.EnumFieldDescriptor =	--	杂项定义
	{
	    TYPE_DOUBLE         = 1,
	    TYPE_FLOAT          = 2,
	    TYPE_INT64          = 3,
	    TYPE_UINT64         = 4,
	    TYPE_INT32          = 5,
	    TYPE_FIXED64        = 6,
	    TYPE_FIXED32        = 7,
	    TYPE_BOOL           = 8,
	    TYPE_STRING         = 9,
	    TYPE_GROUP          = 10,
	    TYPE_MESSAGE        = 11,
	    TYPE_BYTES          = 12,
	    TYPE_UINT32         = 13,
	    TYPE_ENUM           = 14,
	    TYPE_SFIXED32       = 15,
	    TYPE_SFIXED64       = 16,
	    TYPE_SINT32         = 17,
	    TYPE_SINT64         = 18,
	    MAX_TYPE            = 18,

	    LABEL_OPTIONAL      = 1,
	    LABEL_REQUIRED      = 2,
	    LABEL_REPEATED      = 3,
	    MAX_LABEL           = 3
	}

	DynamicProtobufWrapper.UnknownFieldType = 
	{
		TYPE_VARINT = 0,
		TYPE_FIXED32 = 1,
		TYPE_FIXED64 = 2,
		TYPE_LENGTH_DELIMITED = 3,
		TYPE_GROUP = 4,
	}
end

return DynamicProtobufWrapper

