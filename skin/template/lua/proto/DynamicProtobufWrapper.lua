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
local UnknownField = DynamicProtobuf.UnknownField
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
	
	function WrapperHelper._FieldEnumDefines(pEnumDescriptor, lib)
		local enum_name = EnumDescriptor.name(pEnumDescriptor)
		lib[enum_name] = lib[enum_name] or {}
		
		local e = lib[enum_name]
		local max_value = 0
		for iEnumValue = 1, EnumDescriptor.value_count(pEnumDescriptor) do
			local pEnumValueDescriptor = EnumDescriptor.value(pEnumDescriptor, iEnumValue - 1)
			local name = EnumValueDescriptor.name(pEnumValueDescriptor)
			local number = EnumValueDescriptor.number(pEnumValueDescriptor)
			e[name] = number --建议这种，后期还是采用枚举名称.枚举定义名的形式访问吧
			max_value = number
		end

		--取枚举最后一个值+1，避免定义xxx_COUNT
		local count_name = enum_name.."_COUNT"
		if not e[count_name] then
			local last_index = EnumDescriptor.value_count(pEnumDescriptor)-1
			local pEnumValueDescriptor = EnumDescriptor.value(pEnumDescriptor, last_index)
			local number = EnumValueDescriptor.number(pEnumValueDescriptor)
			e[count_name] = number + 1
		end

		--跟服务器C++一致， 生成一个XXX_MAX
		if not e[enum_name .. "_MAX"] then
			e[enum_name .. "_MAX"] = max_value
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
	
	function WrapperHelper._MessageClass_GetDescriptor(self)
		return self.__pDescriptor
	end

	function WrapperHelper._MessageClass__GetOptionDescriptor(self)
		return Descriptor.GetOptionDescriptor(self.__pDescriptor)
	end
	
	function WrapperHelper._MessageClass_FindFieldDescriptor(self, fieldName)
		return Descriptor.FindFieldByName(self.__pDescriptor, fieldName)
	end

	function WrapperHelper._MessageClass_GetName(self)
		return Descriptor.name(self.__pDescriptor)
	end

	function WrapperHelper._MessageClass__extension_range_count(self)
		return Descriptor.extension_range_count(self.__pDescriptor)
	end

	local l__ExtensionRange_methods
	function WrapperHelper._MessageClass__ExtensionRange_index(self, k)
		local ExtensionRangeOptions = DynamicProtobuf.ExtensionRangeOptions
		if not l__ExtensionRange_methods then
			l__ExtensionRange_methods =
			{
				unknown_field_count = function(self)
					return ExtensionRangeOptions.unknown_field_count(self.__ExtensionRange)
				end,
				unknown_field = function(self, i)
					return ExtensionRangeOptions.unknown_field(self.__ExtensionRange, i)
				end,
				unknown_fields = function(self)
					return ExtensionRangeOptions.unknown_fields(self.__ExtensionRange)
				end,
			}
		end
		local method = l__ExtensionRange_methods[k]
		if method then
			return method
		end
		return nil
	end
	function WrapperHelper._MessageClass__extension_range_count(self, index)
		assert(index>0 and index <= self:extension_range_count())
		local e = Descriptor.extension_range(self.__pDescriptor, index)
		if e then
			local meta = {
				__index = WrapperHelper._MessageClass__ExtensionRange_index,
			}
			return setmetatable({__ExtensionRange = e.options, options=e.options, start=e.start, tail=e['end']}, meta)
		end
		return nil
	end

	function WrapperHelper._MessageClass_GetSourceLocation(self)
		return Descriptor.GetSourceLocation(self.__pDescriptor)
	end
	function WrapperHelper._MessageClass_GetFieldSourceLocation(self, fieldName)
		local pFieldDescriptor = Descriptor.FindFieldByName(self.__pDescriptor, fieldName)
		return FieldDescriptor.GetSourceLocation(pFieldDescriptor)
	end

	function WrapperHelper._MessageClass__unknow_field_count(self)
		return Descriptor.unknow_field_count(self.__pDescriptor)
	end

	local l__unknow_field_methods
	function WrapperHelper._MessageClass__unknow_field_index(self, k)
		if not l__unknow_field_methods then
			l__unknow_field_methods =
			{
				type = function(self)
					return UnknownField.type(self.__unknow_field)
				end,
				number = function(self)
					return UnknownField.number(self.__unknow_field)
				end,
				varint = function(self)
					return UnknownField.varint(self.__unknow_field)
				end,
				fixed32 = function(self)
					return UnknownField.fixed32(self.__unknow_field)
				end,
				fixed64 = function(self)
					return UnknownField.fixed64(self.__unknow_field)
				end,
				length_delimited = function(self)
					return UnknownField.length_delimited(self.__unknow_field)
				end,
			}
		end
		local method = l__unknow_field_methods[k]
		if method then
			return method
		end
		return nil
	end

	function WrapperHelper._MessageClass__unknow_field(self, index)
		assert(index>0 and index <= self:unknow_field_count())
		local unknow_field = Descriptor.unknow_field(self.__pDescriptor, index-1)
		if unknow_field then
			local meta = {
				__index = WrapperHelper._MessageClass__unknow_field_index,
			}
			return setmetatable({__unknow_field = unknow_field}, meta)
		end
		return nil
	end

	function WrapperHelper._MessageClass__unknow_fields(self)
		local result = {}
		for i=1, self:unknow_field_count() do
			local p = self:unknow_field(i)
			table.insert(result, p)
		end
		return result
	end

	function WrapperHelper._MessageClass__unknow_field_by_value(self, v)
		for i=1, self:unknow_field_count() do
			local p = self:unknow_field(i)
			if p:number() == v then
				return p
			end
		end
	end

	function WrapperHelper._MessageClass__field_unknow_field_count(self, index)
		local fd = self:field(index)
		return FieldDescriptor.unknow_field_count(fd)
	end
	function WrapperHelper._MessageClass__field_unknow_field(self, index, findex)
		assert(findex>0 and findex <= self:field_unknow_field_count(index))
		local fd = self:field(index)
		local unknow_field = FieldDescriptor.unknow_field(fd, findex-1)
		if unknow_field then
			local meta = {
				__index = WrapperHelper._MessageClass__unknow_field_index,
			}
			return setmetatable({__unknow_field = unknow_field}, meta)
		end
		return nil
	end
	function WrapperHelper._MessageClass__field_unknow_field_by_value(self, index, v)
		for findex=1, self:field_unknow_field_count(index) do
			local p = self:field_unknow_field(index, findex)
			if p:number() == v then
				return p
			end
		end
	end

	function WrapperHelper._MessageClass__FindExtensionByName(self, name)
		return Descriptor.FindExtensionByName(self.__pDescriptor, name)
	end

	function WrapperHelper._MessageClass__field_count(self)
		return Descriptor.field_count(self.__pDescriptor)
	end

	function WrapperHelper._MessageClass__field(self, index)
		assert(index >0 and index <= self:field_count())
		return Descriptor.field(self.__pDescriptor, index-1)
	end

	function WrapperHelper._MessageClass__field_name(self, index)
		local fd = self:field(index)
		return FieldDescriptor.name(fd)
	end

	function WrapperHelper._MessageClass_SerializeHeadToTable(self)
		local DynamicProtobufWrapper = FLua.ForwardClass("DynamicProtobufWrapper")
		local result = {}

		local function make_field_full_name(prefix, field_name, idx)
			local pre = #prefix == 0 and "" or prefix .. "."
			if idx then
				local arrayIndex = "[" .. tostring(idx) .. "]"
				return pre .. field_name .. arrayIndex
			else
				return pre .. field_name
			end
		end
		local function make_field_full_comment(prefix, comment, idx)
			local pre = #prefix == 0 and "" or prefix
			if idx then
				local arrayIndex = "[" .. tostring(idx) .. "]"
				return pre .. comment .. arrayIndex
			else
				return pre .. comment
			end
		end

		local function make_head(msg_class, field_prefix, comment_prefix)
			for i=1, msg_class:field_count() do
				local fd = msg_class:field(i)
				local field_name = msg_class:field_name(i)
				local ftype = FieldDescriptor.type(fd)
				if not FieldDescriptor.is_repeated(fd) and ftype ~= DynamicProtobufWrapper.TYPE.TYPE_MESSAGE then--basic type
					local f = {}
					f.type = FieldDescriptor.TypeName(fd, ftype)
					f.full_name = make_field_full_name(field_prefix,field_name)
					local l, t = msg_class:GetFieldSourceLocation(field_name)
					if t and #t >0 then
						f.comment = make_field_full_comment(comment_prefix, t)
					else
						f.comment = make_field_full_comment(comment_prefix, field_name)
					end

					table.insert(result, f)
				elseif FieldDescriptor.is_repeated(fd) and ftype ~= DynamicProtobufWrapper.TYPE.TYPE_MESSAGE then--basic type array
					local default_list_p = msg_class:field_unknow_field_by_value(i, 61003) or msg_class:field_unknow_field_by_value(i, 61004) or nil
					local default_list_size = 0
					if default_list_p then
						local m64 = make_i64(default_list_p:varint())
						default_list_size = m64:int()
					end
					for idx=1, default_list_size do
						local f = {}
						f.type = FieldDescriptor.TypeName(fd, ftype)
						f.full_name = make_field_full_name(field_prefix, field_name, idx)
						local l, t = msg_class:GetFieldSourceLocation(field_name)
						if t and #t >0 then
							f.comment = make_field_full_comment(comment_prefix, t, idx)
						else
							f.comment = make_field_full_comment(comment_prefix, field_name, idx)
						end

						table.insert(result, f)
					end
				elseif not FieldDescriptor.is_repeated(fd) and ftype == DynamicProtobufWrapper.TYPE.TYPE_MESSAGE then--sub message type
					local sub_field_message_class = WrapperHelper.Wrap_MessageClass(FieldDescriptor.message_type(fd))
					local l, t = msg_class:GetFieldSourceLocation(field_name)
					local pre = field_name
					if t and #t >0 then
						pre = t
					end
					make_head(sub_field_message_class, make_field_full_name(field_prefix, field_name), make_field_full_comment(comment_prefix, pre))
				else--sub message type array
					local l, t = msg_class:GetFieldSourceLocation(field_name)
					local pre = field_name
					if t and #t >0 then
						pre = t
					end
					local default_list_p = msg_class:field_unknow_field_by_value(i, 61003) or msg_class:field_unknow_field_by_value(i, 61004) or nil
					local default_list_size = 0
					if default_list_p then
						local m64 = make_i64(default_list_p:varint())
						default_list_size = m64:int()
					end
					local sub_field_message_class = WrapperHelper.Wrap_MessageClass(FieldDescriptor.message_type(fd))
					for idx=1, default_list_size do
						make_head(sub_field_message_class, make_field_full_name(field_prefix, field_name, idx), make_field_full_comment(comment_prefix, pre, idx))
					end
				end
			end
		end
		
		make_head(self, "", "")

		return result
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
				GetOptionDescriptor = WrapperHelper._MessageClass__GetOptionDescriptor,
				GetSourceLocation = WrapperHelper._MessageClass_GetSourceLocation,
				GetFieldSourceLocation = WrapperHelper._MessageClass_GetFieldSourceLocation,
				unknow_field_count = WrapperHelper._MessageClass__unknow_field_count,
				unknow_field = WrapperHelper._MessageClass__unknow_field,
				unknow_fields = WrapperHelper._MessageClass__unknow_fields,
				unknow_field_by_value = WrapperHelper._MessageClass__unknow_field_by_value,
				extension_range_count = WrapperHelper._MessageClass__extension_range_count,
				extension_range = WrapperHelper._MessageClass__extension_range,
				FindExtensionByName = WrapperHelper._MessageClass__FindExtensionByName,
				field_count = WrapperHelper._MessageClass__field_count,
				field = WrapperHelper._MessageClass__field,
				field_name = WrapperHelper._MessageClass__field_name,
				field_unknow_field_count = WrapperHelper._MessageClass__field_unknow_field_count,
				field_unknow_field = WrapperHelper._MessageClass__field_unknow_field,
				field_unknow_field_by_value = WrapperHelper._MessageClass__field_unknow_field_by_value,
				SerializeHeadToTable = WrapperHelper._MessageClass_SerializeHeadToTable,
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

	function WrapperHelper._message_getex(self, fieldName)
		local function getfield(msg, field_names)
			local meta = getmetatable(msg)
			local pDescriptor = meta.__pDescriptor
			if not pDescriptor then
				return nil
			end

			local field_name = table.remove(field_names, 1)
			if #field_names == 0 then --最终字段
				--x[1], x
				local a, _, ids = string.find(field_name, "%[(%d+)%]")
				if ids then
					local real_name = string.sub(field_name, 1, a-1)
					return WrapperHelper._message_get(msg, real_name)[tonumber(ids)]
				else
					return WrapperHelper._message_get(msg, field_name)
				end
			else
				--x[1].a , a.x
				local a, _, ids = string.find(field_name, "%[(%d+)%]")
				if ids then
					local real_name = string.sub(field_name, 1, a-1)
					local sub_msg = WrapperHelper._message_get(msg, real_name)[tonumber(ids)]
					return getfield(sub_msg, field_names)
				else
					local sub_msg = WrapperHelper._message_get(msg, field_name)
					return getfield(sub_msg, field_names)
				end
			end
		end

		local field_names = fieldName:split('.')
		Utils.printValue("_message_getex", fieldName, field_names)
		if #field_names == 0 then
			error("getex fileName " .. fieldName, 1)
		end

		return getfield(self, field_names)
	end

	function WrapperHelper._message_setex(self, fieldName, v)
		local function setfield(msg, field_names)
			local meta = getmetatable(msg)
			local pDescriptor = meta.__pDescriptor
			if not pDescriptor then
				error("_message_setex error: __pDescriptor is nil")
				return false
			end
			local msg_class = msg:GetMessage()

			--TODO:需要处理非基础类型
			--x, x[1], x.a, x[1].a
			local field_name = table.remove(field_names, 1)
			if #field_names == 0 then --最终字段
				--x[1], x
				local a, _, ids = string.find(field_name, "%[(%d+)%]")
				if ids then				
					local real_name = string.sub(field_name, 1, a-1)
					if msg_class:FindFieldDescriptor(real_name) then
						local count_size = #(WrapperHelper._message_get(msg, real_name))

						local is_message = msg:IsMessage(real_name)
						if is_message then
							local sub_msg
							if tonumber(ids) < count_size then
								sub_msg = WrapperHelper._message_get(msg, real_name)[tonumber(ids)]
							else
								sub_msg = WrapperHelper._message_get(msg, real_name):add()
							end
							sub_msg:CopyFrom(v)
						else
							if tonumber(ids) < count_size then
								WrapperHelper._message_get(msg, real_name)[tonumber(ids)] = v
							else
								WrapperHelper._message_get(msg, real_name):append(v)
							end
						end
					else
						error("no field_name " .. real_name)
						return false
					end
				else
					if msg_class:FindFieldDescriptor(field_name) then
						local is_message = msg:IsMessage(field_name)
						if is_message then
							local sub_msg = WrapperHelper._message_get(msg, field_name)
							sub_msg:CopyFrom(v)
						else
							WrapperHelper._message_set(msg, field_name, v)
						end
					else
						error("no field_name " .. field_name)
						return false
					end
				end
			else
				--x[1].a , a.x
				local a, _, ids = string.find(field_name, "%[(%d+)%]")
				if ids then
					local real_name = string.sub(field_name, 1, a-1)
					if msg_class:FindFieldDescriptor(real_name) then
						local count_size = #(WrapperHelper._message_get(msg, real_name))
						local sub_msg
						if tonumber(ids) < count_size then 
							sub_msg = WrapperHelper._message_get(msg, real_name)[tonumber(ids)]
						else
							sub_msg = WrapperHelper._message_get(msg, real_name):add()
						end
						return setfield(sub_msg, field_names)
					else
						error("no field_name " .. real_name)
						return false
					end
				else
					if msg_class:FindFieldDescriptor(field_name) then
						local sub_msg = WrapperHelper._message_get(msg, field_name)
						return setfield(sub_msg, field_names)
					else
						error("no field_name " .. field_name)
						return false
					end
				end
			end

			return true
		end
		local field_names = fieldName:split(".")
		if #field_names == 0 then
			error("setex fileName " .. fieldName, 1)
		end

		return setfield(self, field_names)
	end
	
	function WrapperHelper._message_GetMessage(self)
		local meta = getmetatable(self)
		return meta.__MessageClass
	end

	function WrapperHelper._message_CopyFrom(self, ths)
		local meta = getmetatable(self)
		local pMessage = meta.__pMessage
		local meta_ths = getmetatable(ths)
		local pMessage_ths = meta_ths.__pMessage
		Message.CopyFrom(pMessage, pMessage_ths)
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

	function WrapperHelper._message_field_count(self)
		return self:GetMessage():field_count()
	end

	function WrapperHelper._message_field(self, index)
		assert(index>0 and index <= self:field_count())
		return self:GetMessage():field(index)
	end

	function WrapperHelper._message_field_name(self, index)
		assert(index>0 and index <= self:field_count())
		local fd = self:GetMessage():field(index)
		return FieldDescriptor.name(fd)
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
				getex = WrapperHelper._message_getex,
				setex = WrapperHelper._message_setex,
				GetMessage = WrapperHelper._message_GetMessage,
				SerializeToString = WrapperHelper._message_SerializeToString,
				ParseFromString = WrapperHelper._message_ParseFromString,
				HasField = WrapperHelper._message_HasField,
				ListFields = WrapperHelper._message_ListFields,
				FieldSize = WrapperHelper._message_FieldSize,
				field_count = WrapperHelper._message_field_count,
				field_name = WrapperHelper._message_field_name,
				field = WrapperHelper._message_field,
				
				SetInParent = WrapperHelper._message_SetInParent,
				_WriteBack = WrapperHelper._message_WriteBack,
				tryget = WrapperHelper._message_tryget,
				CopyField = WrapperHelper._message_CopyField,
				GetFieldIndex = WrapperHelper._message_GetFieldIndex,
				CopyFrom = WrapperHelper._message_CopyFrom,
				IsRepeated = WrapperHelper._message_IsRepeated,
				IsMessage = WrapperHelper._message_IsMessage,
				SerializeToTable = WrapperHelper._message_SerializeToTable,
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

	function WrapperHelper._message_CopyField(self, src, field_name)
		local self_meta = getmetatable(self)
		local self_message = self_meta.__pMessage
		if not self_message then
			warn("message is nil.")
			return false
		end
		if self_meta.__cachedMembers then
			self_meta.__cachedMembers[field_name] = nil
		end
		local src_message = getmetatable(src).__pMessage
		if not src_message then
			warn("src message is nil.")
			return false
		end
		local self_descriptor = Message.GetDescriptor(self_message)
		if not self_descriptor then
			warn("message type is unknown:", self_message)
			return false
		end

		local src_descriptor = Message.GetDescriptor(src_message)
		if not src_descriptor then
			warn("src message type is unknown:", src_message)
			return false
		end

		if self_descriptor ~= src_descriptor then
			warn(string.format("different message type:self(%s), src(%s)", Descriptor.name(self_descriptor), Descriptor.name(src_descriptor)))
			return false
		end

		local field_descriptor = Descriptor.FindFieldByName(self_descriptor, field_name)
		Message.ClearField(self_message, field_descriptor)
		if FieldDescriptor.is_repeated(field_descriptor) then
			Message.CopyRepeatField(self_message, field_descriptor, src_message)
		else
			local is_message = FieldDescriptor.is_message(field_descriptor)
			if is_message then
				local dest_field_message = Message.Get(self_message, field_descriptor)
				local src_field_message = Message.Get(src_message, field_descriptor)
				Message.CopyFrom(dest_field_message, src_field_message)
			else
				Message.Set(self_message, field_descriptor, Message.Get(src_message, field_descriptor))
			end
		end
		return true
	end

	function WrapperHelper._message_IsMessage(self, field_name)
		local self_meta = getmetatable(self)
		local self_message = self_meta.__pMessage
		if not self_message then
			warn("message is nil.")
			return false
		end
		local self_descriptor = Message.GetDescriptor(self_message)
		if not self_descriptor then
			warn("message type is unknown:", self_message)
			return false
		end

		local field_descriptor = Descriptor.FindFieldByName(self_descriptor, field_name)
		return FieldDescriptor.is_message(field_descriptor)
	end

	function WrapperHelper._message_GetFieldIndex(self, field_name)
		local self_meta = getmetatable(self)
		local self_message = self_meta.__pMessage
		if not self_message then
			warn("message is nil.")
			return -1
		end

		local self_descriptor = Message.GetDescriptor(self_message)
		if not self_descriptor then
			warn("message type is unknown:", self_message)
			return -1
		end

		local field_descriptor = Descriptor.FindFieldByName(self_descriptor, field_name)
		if field_descriptor then
			return FieldDescriptor.index(field_descriptor)
		end
		return -1
	end

	--
	-- repeated field
	--

	function WrapperHelper._message_IsRepeated(self, field_name)
		local self_meta = getmetatable(self)
		local self_message = self_meta.__pMessage
		if not self_message then
			warn("message is nil.")
			return false, false
		end

		local self_descriptor = Message.GetDescriptor(self_message)
		if not self_descriptor then
			warn("message type is unknown:", self_message)
			return false, false
		end

		local field_descriptor = Descriptor.FindFieldByName(self_descriptor, field_name)
		local is_repeated = FieldDescriptor.is_repeated(field_descriptor)
		local cpp_type = FieldDescriptor.cpp_type(field_descriptor)
		local DynamicProtobufWrapper = FLua.ForwardClass("DynamicProtobufWrapper")
		return is_repeated, cpp_type == DynamicProtobufWrapper.CPPTYPE.CPPTYPE_MESSAGE
	end

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

	function WrapperHelper._message_SerializeToTable(self, withHead)
		local DynamicProtobufWrapper = FLua.ForwardClass("DynamicProtobufWrapper")
		local result = {}
		if withHead then
			result = self:GetMessage():SerializeHeadToTable()
		end
		local cursor = 0
		local function make_body(msg, msg_class)
			for i=1, msg_class:field_count() do
				local fd = msg_class:field(i)
				local field_name = msg_class:field_name(i)
				local ftype = FieldDescriptor.type(fd)

				if not FieldDescriptor.is_repeated(fd) then 
					if ftype == DynamicProtobufWrapper.TYPE.TYPE_MESSAGE then
						local sub_field_message_class = WrapperHelper.Wrap_MessageClass(FieldDescriptor.message_type(fd))
						make_body(msg[field_name], sub_field_message_class)
					else
						cursor = cursor + 1
						result[cursor] = result[cursor] or {}
						result[cursor].value = msg[field_name]
					end
				else
					local default_list_p = msg_class:field_unknow_field_by_value(i, 61003) or msg_class:field_unknow_field_by_value(i, 61004) or nil
					local default_list_size = 0
					if default_list_p then
						local msg_id_64 = make_i64(default_list_p:varint())
						default_list_size = msg_id_64:int()
					end

					local local_list_size = msg:FieldSize(field_name)

					for i=1, default_list_size do
						if i <= local_list_size then
							if ftype == DynamicProtobufWrapper.TYPE.TYPE_MESSAGE then
								make_body(msg[field_name][i], msg[field_name][i]:GetMessage())
							else
								cursor = cursor + 1
								result[cursor] = result[cursor] or {}
								result[cursor].value = msg[field_name][i]
							end
						else
							if ftype == DynamicProtobufWrapper.TYPE.TYPE_MESSAGE then
								local sub_field_message_class = WrapperHelper.Wrap_MessageClass(FieldDescriptor.message_type(fd))
								local pMessageProtocol = DynamicProtobuf.GetMessagePrototype(sub_field_message_class.__pDescriptor)
								local pMessage = Message.New(pMessageProtocol)
								local sub_msg = WrapperHelper.Wrap_message(pMessage, nil)
								make_body(sub_msg, sub_msg:GetMessage())
							else
								
							end
						end
					end
				end
			end
		end
		
		make_body(self, self:GetMessage())

		return result
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
		local wrapper = {}
			
		local pFileDescriptorArray = DynamicProtobuf.LoadFileDescriptor(filePath)
		if not pFileDescriptorArray then
			error("failed to load file descriptor file: " .. tostring(filePath))
		end
		-- mesage
		for iDescriptor = 1, #pFileDescriptorArray do
			local pFileDescriptor = pFileDescriptorArray[iDescriptor]
			local fileName = FileDescriptor.name(pFileDescriptor)
			if not fileName:find("google/protobuf") then
				wrapper[fileName] = { FileDescriptor = pFileDescriptor, Messages = {}, Enums = {}, MessageExtension={}}

				for iMessage = 1, FileDescriptor.message_type_count(pFileDescriptor) do
					local pDescriptor = FileDescriptor.message_type(pFileDescriptor, iMessage - 1)
					local name = Descriptor.name(pDescriptor)
					local MessageClass = WrapperHelper.Wrap_MessageClass(pDescriptor)
					lib[name] = MessageClass

					table.insert(wrapper[fileName].Messages, name)
				end

				for iEnum = 1, FileDescriptor.enum_type_count(pFileDescriptor) do
					local pEnumDescriptor = FileDescriptor.enum_type(pFileDescriptor, iEnum - 1)
					WrapperHelper._FieldEnumDefines(pEnumDescriptor, lib)
					
					local name = EnumDescriptor.name(pEnumDescriptor)
					wrapper[fileName].Enums[name] = pEnumDescriptor
				end

				for i=1, FileDescriptor.extension_count(pFileDescriptor) do
					local pFileDescriptor = FileDescriptor.extension(pFileDescriptor, i-1)
					local name = FieldDescriptor.name(pFileDescriptor)
					local value = FieldDescriptor.number(pFileDescriptor)
					wrapper[fileName].MessageExtension[name] = value
				end
			end		
		end
		
		return lib, wrapper
	end

	DynamicProtobufWrapper.TYPE =	--	杂项定义
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
	}

	DynamicProtobufWrapper.Label = 
	{
		LABEL_OPTIONAL      = 1,
	    LABEL_REQUIRED      = 2,
	    LABEL_REPEATED      = 3,
	    MAX_LABEL           = 3
	}

	DynamicProtobufWrapper.CPPTYPE =               --  CppType
	{
		CPPTYPE_INT32       = 1,     -- TYPE_INT32, TYPE_SINT32, TYPE_SFIXED32
		CPPTYPE_INT64       = 2,     -- TYPE_INT64, TYPE_SINT64, TYPE_SFIXED64
		CPPTYPE_UINT32      = 3,     -- TYPE_UINT32, TYPE_FIXED32
		CPPTYPE_UINT64      = 4,     -- TYPE_UINT64, TYPE_FIXED64
		CPPTYPE_DOUBLE      = 5,     -- TYPE_DOUBLE
		CPPTYPE_FLOAT       = 6,     -- TYPE_FLOAT
		CPPTYPE_BOOL        = 7,     -- TYPE_BOOL
		CPPTYPE_ENUM        = 8,     -- TYPE_ENUM
		CPPTYPE_STRING      = 9,     -- TYPE_STRING, TYPE_BYTES
		CPPTYPE_MESSAGE     = 10,    -- TYPE_MESSAGE, TYPE_GROUP
		MAX_CPPTYPE         = 10,    -- Constant useful for defining lookup tables
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

