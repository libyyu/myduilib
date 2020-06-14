/*
	Protocol Buffers dynamic message 相关功能的 Lua 绑定
	从效率上考虑，所有函数都以 lightuserdata 为对象指针，无法提供足够的类型检查，实际应封装后再使用
*/

#define LUA_LIB
#include <lua.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/parser.h>
#include <google/protobuf/any.pb.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/compiler/importer.h>
#include <vector>
#include <string>
#include <sstream>
namespace pb = google::protobuf;

static pb::DescriptorPool& descriptorPool();
static pb::DynamicMessageFactory& dynamicMessageFactory();
static lua_State* gL = NULL;
//
// Helper

//[-0, +0, e], lightUdIndex 上应有相应类型对象的指针 (lightuserdata)
template <typename ClassType>
static ClassType* checkClassInstancePtr(lua_State* L, int lightUdIndex)
{
	luaL_checktype(L, lightUdIndex, LUA_TLIGHTUSERDATA);
	return (ClassType*)lua_touserdata(L, lightUdIndex);
}

//[-1, +0], 推入对象 lightuserdata 指针 (或 nil 当对象为 NULL)
template <typename ClassType>
static void pushClassInstancePtr(lua_State* L, ClassType* ptr)
{
	if (ptr)
		lua_pushlightuserdata(L, (void*)ptr);
	else
		lua_pushnil(L);
}

static pb::uint64 checkUInt64(lua_State *L, int n)
{
	size_t len;
	const char* str = luaL_checklstring(L, n, &len);
	if (len != 8)
	{
		luaL_error(L, "bad uint64 string length (8 expected, got %d)", int(len));
		return 0;
	}

	return *(pb::uint64*)str;
}

static void pushUInt64(lua_State *L, pb::uint64 value)
{
	lua_pushlstring(L, (const char*)&value, 8);
}

static pb::int64 checkInt64(lua_State *L, int n)
{
	size_t len;
	const char* str = luaL_checklstring(L, n, &len);
	if (len != 8)
	{
		luaL_error(L, "bad int64 string length (8 expected, got %d)", int(len));
		return 0;
	}

	return *(pb::int64*)str;
}

static void pushInt64(lua_State *L, pb::int64 value)
{
	lua_pushlstring(L, (const char*)&value, 8);
}


// CheckValue<T> 用于取参数

template <typename RetType>
struct CheckValue;

template <>
struct CheckValue<pb::int32>
{
	static pb::int32 value(lua_State* L, int n)
	{
		return luaL_checkinteger(L, n);
	}
};

template <>
struct CheckValue<pb::int64>
{
	static pb::int64 value(lua_State* L, int n)
	{
		return checkInt64(L, n);
	}
};

template <>
struct CheckValue<pb::uint32>
{
	static pb::uint32 value(lua_State* L, int n)
	{
		return (pb::uint32)(pb::uint64)(pb::int64)luaL_checkinteger(L, n);
	}
};

template <>
struct CheckValue<pb::uint64>
{
	static pb::uint64 value(lua_State* L, int n)
	{
		return checkUInt64(L, n);
	}
};

template <>
struct CheckValue<float>
{
	static float value(lua_State* L, int n)
	{
		return (float)luaL_checknumber(L, n);
	}
};

template <>
struct CheckValue<double>
{
	static double value(lua_State* L, int n)
	{
		return luaL_checknumber(L, n);
	}
};

template <>
struct CheckValue<bool>
{
	static bool value(lua_State* L, int n)
	{
		return lua_toboolean(L, n) != 0;
	}
};

template <>
struct CheckValue<const char*>
{
	static const char* value(lua_State* L, int n)
	{
		return luaL_checkstring(L, n);
	}
};

template <>
struct CheckValue<std::string>
{
	static std::string value(lua_State* L, int n)
	{
		size_t len;
		char const* str = luaL_checklstring(L, n, &len);
		return std::string(str, len);
	}
};

template <>
struct CheckValue<std::string const&>
{
	static std::string value(lua_State* L, int n)
	{
		return CheckValue<std::string>::value(L, n);
	}
};

template <typename RetType>
struct CheckValue<RetType*>
{
	static RetType* value(lua_State* L, int n)
	{
		return checkClassInstancePtr<RetType>(L, n);
	}
};

template <typename RetType>
struct CheckValue<RetType const&>
{
	static RetType const& value(lua_State* L, int n)
	{
		return *checkClassInstancePtr<RetType>(L, n);
	}
};

// PushValue<T> 用于压返回值

static void PushValue(lua_State* L, pb::int32 v)
{
	lua_pushinteger(L, v);
}

static void PushValue(lua_State* L, pb::int64 v)
{
	pushInt64(L, v);
}

static void PushValue(lua_State* L, pb::uint32 v)
{
	lua_pushnumber(L, (double)v);
}

static void PushValue(lua_State* L, pb::uint64 v)
{
	pushUInt64(L, v);
}

static void PushValue(lua_State* L, float v)
{
	lua_pushnumber(L, v);
}

static void PushValue(lua_State* L, double v)
{
	lua_pushnumber(L, v);
}

static void PushValue(lua_State* L, bool v)
{
	lua_pushboolean(L, v);
}

static void PushValue(lua_State* L, pb::UnknownField::Type v)
{
	lua_pushnumber(L, (int)v);
}

static void PushValue(lua_State* L, char const* v)
{
	lua_pushstring(L, v);
}

static void PushValue(lua_State* L, std::string const& v)
{
	lua_pushlstring(L, v.c_str(), v.size());
}

template <typename ValueType>
static void PushValue(lua_State* L, ValueType* v)
{
	pushClassInstancePtr(L, v);
}

template <typename ValueType>
static void PushValue(lua_State* L, ValueType const& v)
{
	pushClassInstancePtr(L, &v);
}

// BindLuaCFunction_n 用于把 member function 转成 LuaCFunction，n 为参数数量

// RetType f()
template <typename ClassType, typename RetType, RetType (ClassType::*memberFunc)()>
struct BindLuaFunc_0
{
	static int value(lua_State* L)
	{
		ClassType* pObj = checkClassInstancePtr<ClassType>(L, 1);
		RetType ret = (pObj->*memberFunc)();
		PushValue(L, ret);
		return 1;
	}
};

// void f()
template <typename ClassType, void (ClassType::*memberFunc)()>
struct BindLuaFunc_0<ClassType, void, memberFunc>
{
	static int value(lua_State* L)
	{
		ClassType* pObj = checkClassInstancePtr<ClassType>(L, 1);
		(pObj->*memberFunc)();
		return 0;
	}
};

// RetType f() const
template <typename ClassType, typename RetType, RetType (ClassType::*memberFunc)() const>
struct BindLuaFunc_0_const
{
	static int value(lua_State* L)
	{
		ClassType* pObj = checkClassInstancePtr<ClassType>(L, 1);
		RetType ret = (pObj->*memberFunc)();
		PushValue(L, ret);
		return 1;
	}
};

// void f() const
template <typename ClassType, void (ClassType::*memberFunc)() const>
struct BindLuaFunc_0_const<ClassType, void, memberFunc>
{
	static int value(lua_State* L)
	{
		ClassType* pObj = checkClassInstancePtr<ClassType>(L, 1);
		(pObj->*memberFunc)();
		return 0;
	}
};

// RetType f(Param1Type param1)
template <typename ClassType, typename RetType, typename Param1Type, RetType (ClassType::*memberFunc)(Param1Type)>
struct BindLuaFunc_1
{
	static int value(lua_State* L)
	{
		ClassType* pObj = checkClassInstancePtr<ClassType>(L, 1);
		Param1Type param1 = CheckValue<Param1Type>::value(L, 2);
		auto ret = (pObj->*memberFunc)(param1);
		PushValue(L, ret);
		return 1;
	}
};

// void f(Param1Type param1)
template <typename ClassType, typename Param1Type, void (ClassType::*memberFunc)(Param1Type)>
struct BindLuaFunc_1<ClassType, void, Param1Type, memberFunc>
{
	static int value(lua_State* L)
	{
		ClassType* pObj = checkClassInstancePtr<ClassType>(L, 1);
		Param1Type param1 = CheckValue<Param1Type>::value(L, 2);
		(pObj->*memberFunc)(param1);
		return 0;
	}
};

// RetType f(Param1Type param1) const
template <typename ClassType, typename RetType, typename Param1Type, RetType (ClassType::*memberFunc)(Param1Type) const>
struct BindLuaFunc_1_const
{
	static int value(lua_State* L)
	{
		ClassType* pObj = checkClassInstancePtr<ClassType>(L, 1);
		Param1Type param1 = CheckValue<Param1Type>::value(L, 2);
		auto ret = (pObj->*memberFunc)(param1);
		PushValue(L, ret);
		return 1;
	}
};

// void f(Param1Type param1) const
template <typename ClassType, typename Param1Type, void (ClassType::*memberFunc)(Param1Type) const>
struct BindLuaFunc_1_const<ClassType, void, Param1Type, memberFunc>
{
	static int value(lua_State* L)
	{
		ClassType* pObj = checkClassInstancePtr<ClassType>(L, 1);
		Param1Type param1 = CheckValue<Param1Type>::value(L, 2);
		(pObj->*memberFunc)(param1);
		return 0;
	}
};

// End Helper
//

//
// DynamicProtobuf.FileDescriptor
//
static const struct luaL_Reg FileDescriptor_funcs[] =
{
	{ "name", BindLuaFunc_0_const<pb::FileDescriptor const, std::string const&, &pb::FileDescriptor::name>::value },
	{ "package", BindLuaFunc_0_const<pb::FileDescriptor const, std::string const&, &pb::FileDescriptor::package>::value },
	{ "message_type_count", BindLuaFunc_0_const<pb::FileDescriptor const, int, &pb::FileDescriptor::message_type_count>::value },
	{ "message_type", BindLuaFunc_1_const<pb::FileDescriptor const, pb::Descriptor const*, int, &pb::FileDescriptor::message_type>::value },
	{ "enum_type_count", BindLuaFunc_0_const<pb::FileDescriptor const, int, &pb::FileDescriptor::enum_type_count>::value },
	{ "enum_type", BindLuaFunc_1_const<pb::FileDescriptor const, pb::EnumDescriptor const*, int, &pb::FileDescriptor::enum_type>::value },
	{ "FindMessageTypeByName", BindLuaFunc_1_const<pb::FileDescriptor const, pb::Descriptor const*, std::string const&, &pb::FileDescriptor::FindMessageTypeByName>::value },
	{ "FindEnumTypeByName", BindLuaFunc_1_const<pb::FileDescriptor const, pb::EnumDescriptor const*, std::string const&, &pb::FileDescriptor::FindEnumTypeByName>::value },
	{ "FindEnumValueByName", BindLuaFunc_1_const<pb::FileDescriptor const, pb::EnumValueDescriptor const*, std::string const&, &pb::FileDescriptor::FindEnumValueByName>::value },
	{ "DebugString", BindLuaFunc_0_const<pb::FileDescriptor const, std::string, &pb::FileDescriptor::DebugString>::value },
	{ NULL, NULL },
};

//
// DynamicProtobuf.Descriptor
//
static int Descriptor_unknow_field_count(lua_State* L)
{
	const pb::Descriptor* self = checkClassInstancePtr<pb::Descriptor const>(L, 1);
	if (!self)
	{
		luaL_error(L, "Descriptor_unknow_field_count: argument #1 must be Descriptor");
		return 0;
	}
	int count = self->options().unknown_fields().field_count();
	lua_pushinteger(L, count);
	return 1;
}

static int Descriptor_unknow_field(lua_State* L)
{
	const pb::Descriptor* self = checkClassInstancePtr<pb::Descriptor const>(L, 1);
	if (!self)
	{
		luaL_error(L, "Descriptor_unknow_field: argument #1 must be Descriptor");
		return 0;
	}
	if (!lua_isnumber(L, 2))
	{
		luaL_error(L, "Descriptor_unknow_field: argument #2 must be integer");
		return 0;
	}
	int index = lua_tointeger(L, 2);
	int count = self->options().unknown_fields().field_count();
	if (index < 0 || index >= count)
	{
		luaL_error(L, "Descriptor_unknow_field: argument #2 must be range[0, %d]", count - 1);
		return 0;
	}
	//这里v必须使用引用方式，不然传递到lua会导致数据异常
	const pb::UnknownField& v = self->options().unknown_fields().field(index);
	PushValue(L, &v);

	return 1;
}

static const struct luaL_Reg Descriptor_funcs[] =
{
	{ "name", BindLuaFunc_0_const<pb::Descriptor const, std::string const&, &pb::Descriptor::name>::value },
	{ "full_name", BindLuaFunc_0_const<pb::Descriptor const, std::string const&, &pb::Descriptor::full_name>::value },
	{ "index", BindLuaFunc_0_const<pb::Descriptor const, int, &pb::Descriptor::index>::value },
	{ "file", BindLuaFunc_0_const<pb::Descriptor const, pb::FileDescriptor const*, &pb::Descriptor::file>::value },
	{ "containing_type", BindLuaFunc_0_const<pb::Descriptor const, pb::Descriptor const*, &pb::Descriptor::containing_type>::value },
	{ "DebugString", BindLuaFunc_0_const<pb::Descriptor const, std::string, &pb::Descriptor::DebugString>::value },
	{ "field_count", BindLuaFunc_0_const<pb::Descriptor const, int, &pb::Descriptor::field_count>::value },
	{ "field", BindLuaFunc_1_const<pb::Descriptor const, pb::FieldDescriptor const*, int, &pb::Descriptor::field>::value },
	{ "FindFieldByName", BindLuaFunc_1_const<pb::Descriptor const, pb::FieldDescriptor const*, std::string const&, &pb::Descriptor::FindFieldByName>::value },
	{ "nested_type_count", BindLuaFunc_0_const<pb::Descriptor const, int, &pb::Descriptor::nested_type_count>::value },
	{ "nested_type", BindLuaFunc_1_const<pb::Descriptor const, pb::Descriptor const*, int, &pb::Descriptor::nested_type>::value },
	{ "FindNestedTypeByName", BindLuaFunc_1_const<pb::Descriptor const, pb::Descriptor const*, std::string const&, &pb::Descriptor::FindNestedTypeByName>::value },
	{ "enum_type_count", BindLuaFunc_0_const<pb::Descriptor const, int, &pb::Descriptor::enum_type_count>::value },
	{ "enum_type", BindLuaFunc_1_const<pb::Descriptor const, pb::EnumDescriptor const*, int, &pb::Descriptor::enum_type>::value },
	{ "FindEnumTypeByName", BindLuaFunc_1_const<pb::Descriptor const, pb::EnumDescriptor const*, std::string const&, &pb::Descriptor::FindEnumTypeByName>::value },
	{ "FindEnumValueByName", BindLuaFunc_1_const<pb::Descriptor const, pb::EnumValueDescriptor const*, std::string const&, &pb::Descriptor::FindEnumValueByName>::value },

	{ "unknow_field_count", Descriptor_unknow_field_count },
	{ "unknow_field", Descriptor_unknow_field },
	{ NULL, NULL },
};

//
// DynamicProtobuf.EnumDescriptor
//
static const struct luaL_Reg EnumDescriptor_funcs[] =
{
	{ "name", BindLuaFunc_0_const<pb::EnumDescriptor const, std::string const&, &pb::EnumDescriptor::name>::value },
	{ "full_name", BindLuaFunc_0_const<pb::EnumDescriptor const, std::string const&, &pb::EnumDescriptor::full_name>::value },
	{ "index", BindLuaFunc_0_const<pb::EnumDescriptor const, int, &pb::EnumDescriptor::index>::value },
	{ "file", BindLuaFunc_0_const<pb::EnumDescriptor const, pb::FileDescriptor const*, &pb::EnumDescriptor::file>::value },
	{ "value_count", BindLuaFunc_0_const<pb::EnumDescriptor const, int, &pb::EnumDescriptor::value_count>::value },
	{ "value", BindLuaFunc_1_const<pb::EnumDescriptor const, pb::EnumValueDescriptor const*, int, &pb::EnumDescriptor::value>::value },
	{ "FindValueByName", BindLuaFunc_1_const<pb::EnumDescriptor const, pb::EnumValueDescriptor const*, std::string const&, &pb::EnumDescriptor::FindValueByName>::value },
	{ "FindValueByNumber", BindLuaFunc_1_const<pb::EnumDescriptor const, pb::EnumValueDescriptor const*, int, &pb::EnumDescriptor::FindValueByNumber>::value },
	{ "containing_type", BindLuaFunc_0_const<pb::EnumDescriptor const, pb::Descriptor const*, &pb::EnumDescriptor::containing_type>::value },
	{ "DebugString", BindLuaFunc_0_const<pb::EnumDescriptor const, std::string, &pb::EnumDescriptor::DebugString>::value },
	{ NULL, NULL },
};

//
// DynamicProtobuf.EnumValueDescriptor
//
static const struct luaL_Reg EnumValueDescriptor_funcs[] =
{
	{ "name", BindLuaFunc_0_const<pb::EnumValueDescriptor const, std::string const&, &pb::EnumValueDescriptor::name>::value },
	{ "index", BindLuaFunc_0_const<pb::EnumValueDescriptor const, int, &pb::EnumValueDescriptor::index>::value },
	{ "number", BindLuaFunc_0_const<pb::EnumValueDescriptor const, int, &pb::EnumValueDescriptor::number>::value },
	{ "full_name", BindLuaFunc_0_const<pb::EnumValueDescriptor const, std::string const&, &pb::EnumValueDescriptor::full_name>::value },
	{ "type", BindLuaFunc_0_const<pb::EnumValueDescriptor const, pb::EnumDescriptor const*, &pb::EnumValueDescriptor::type>::value },
	{ "DebugString", BindLuaFunc_0_const<pb::EnumValueDescriptor const, std::string, &pb::EnumValueDescriptor::DebugString>::value },
	{ NULL, NULL },
};

//
// DynamicProtobuf.FieldDescriptor
//
static int FieldDescriptor_is_message(lua_State* L)
{
	const pb::FieldDescriptor* self = checkClassInstancePtr<pb::FieldDescriptor const>(L, 1);
	bool ret = self->cpp_type() == pb::FieldDescriptor::CPPTYPE_MESSAGE;
	lua_pushboolean(L, ret ? 1 : 0);
	return 1;
}

static int FieldDescriptor_GetDefaultValue(lua_State* L)
{
	const pb::FieldDescriptor* self = checkClassInstancePtr<pb::FieldDescriptor const>(L, 1);
	if (!self->has_default_value())
	{
		luaL_error(L, "field '%s' has no default value", self->full_name().c_str());
		return 0;
	}

	switch (self->cpp_type())
	{
	case pb::FieldDescriptor::CPPTYPE_INT32:
	{
		pb::int32 v = self->default_value_int32();
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_INT64:
	{
		pb::int64 v = self->default_value_int64();
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT32:
	{
		pb::uint32 v = self->default_value_uint32();
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT64:
	{
		pb::uint64 v = self->default_value_uint64();
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_FLOAT:
	{
		float v = self->default_value_float();
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_DOUBLE:
	{
		double v = self->default_value_double();
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_BOOL:
	{
		bool v = self->default_value_bool();
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_ENUM:
	{
		const pb::EnumValueDescriptor* pEnumValueDescriptor = self->default_value_enum();
		PushValue(L, pEnumValueDescriptor->number());
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_STRING:
	{
		std::string const& v = self->default_value_string();
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_MESSAGE:
	{
		luaL_error(L, "invalid GetDefaultValue on message field '%s'", self->full_name().c_str());
		break;
	}
	default:
	{
		luaL_error(L, "invalid cpp type");
		break;
	}
	}
	return 1;
}

static const struct luaL_Reg FieldDescriptor_funcs[] =
{
	{ "name", BindLuaFunc_0_const<pb::FieldDescriptor const, std::string const&, &pb::FieldDescriptor::name>::value },
	{ "full_name", BindLuaFunc_0_const<pb::FieldDescriptor const, std::string const&, &pb::FieldDescriptor::full_name>::value },
	{ "file", BindLuaFunc_0_const<pb::FieldDescriptor const, pb::FileDescriptor const*, &pb::FieldDescriptor::file>::value },
	{ "type", BindLuaFunc_0_const<pb::FieldDescriptor const, pb::FieldDescriptor::Type, &pb::FieldDescriptor::type>::value },
	{ "type_name", BindLuaFunc_0_const<pb::FieldDescriptor const, const char*, &pb::FieldDescriptor::type_name>::value },
	{ "cpp_type", BindLuaFunc_0_const<pb::FieldDescriptor const, pb::FieldDescriptor::CppType, &pb::FieldDescriptor::cpp_type>::value },
	{ "cpp_type_name", BindLuaFunc_0_const<pb::FieldDescriptor const, const char*, &pb::FieldDescriptor::cpp_type_name>::value },
	{ "label", BindLuaFunc_0_const<pb::FieldDescriptor const, pb::FieldDescriptor::Label, &pb::FieldDescriptor::label>::value },
	{ "is_required", BindLuaFunc_0_const<pb::FieldDescriptor const, bool, &pb::FieldDescriptor::is_required>::value },
	{ "is_optional", BindLuaFunc_0_const<pb::FieldDescriptor const, bool, &pb::FieldDescriptor::is_optional>::value },
	{ "is_repeated", BindLuaFunc_0_const<pb::FieldDescriptor const, bool, &pb::FieldDescriptor::is_repeated>::value },
	{ "is_map", BindLuaFunc_0_const<pb::FieldDescriptor const, bool, &pb::FieldDescriptor::is_map>::value },
	{ "is_packable", BindLuaFunc_0_const<pb::FieldDescriptor const, bool, &pb::FieldDescriptor::is_packable>::value },
	{ "is_packed", BindLuaFunc_0_const<pb::FieldDescriptor const, bool, &pb::FieldDescriptor::is_packed>::value },
	{ "index", BindLuaFunc_0_const<pb::FieldDescriptor const, int, &pb::FieldDescriptor::index>::value },
	{ "has_default_value", BindLuaFunc_0_const<pb::FieldDescriptor const, bool, &pb::FieldDescriptor::has_default_value>::value },
	{ "default_value_int32", BindLuaFunc_0_const<pb::FieldDescriptor const, int, &pb::FieldDescriptor::default_value_int32>::value },
	{ "default_value_int64", BindLuaFunc_0_const<pb::FieldDescriptor const, pb::int64, &pb::FieldDescriptor::default_value_int64>::value },
	{ "default_value_uint32", BindLuaFunc_0_const<pb::FieldDescriptor const, pb::uint32, &pb::FieldDescriptor::default_value_uint32>::value },
	{ "default_value_uint64", BindLuaFunc_0_const<pb::FieldDescriptor const, pb::uint64, &pb::FieldDescriptor::default_value_uint64>::value },
	{ "default_value_float", BindLuaFunc_0_const<pb::FieldDescriptor const, float, &pb::FieldDescriptor::default_value_float>::value },
	{ "default_value_double", BindLuaFunc_0_const<pb::FieldDescriptor const, double, &pb::FieldDescriptor::default_value_double>::value },
	{ "default_value_bool", BindLuaFunc_0_const<pb::FieldDescriptor const, bool, &pb::FieldDescriptor::default_value_bool>::value },
	{ "default_value_enum", BindLuaFunc_0_const<pb::FieldDescriptor const, const pb::EnumValueDescriptor*, &pb::FieldDescriptor::default_value_enum>::value },
	{ "default_value_string", BindLuaFunc_0_const<pb::FieldDescriptor const, std::string const&, &pb::FieldDescriptor::default_value_string>::value },
	{ "containing_type", BindLuaFunc_0_const<pb::FieldDescriptor const, const pb::Descriptor*, &pb::FieldDescriptor::containing_type>::value },
	{ "message_type", BindLuaFunc_0_const<pb::FieldDescriptor const, const pb::Descriptor*, &pb::FieldDescriptor::message_type>::value },
	{ "enum_type", BindLuaFunc_0_const<pb::FieldDescriptor const, const pb::EnumDescriptor*, &pb::FieldDescriptor::enum_type>::value },
	{ "DebugString", BindLuaFunc_0_const<pb::FieldDescriptor const, std::string, &pb::FieldDescriptor::DebugString>::value },

	//新增
	{ "is_message", FieldDescriptor_is_message },
	{ "GetDefaultValue", FieldDescriptor_GetDefaultValue },

	{ NULL, NULL },
};

static const struct luaL_Reg UnknownField_funcs[] =
{
	{ "varint", BindLuaFunc_0_const<pb::UnknownField const, pb::uint64, &pb::UnknownField::varint>::value },
	{ "fixed32", BindLuaFunc_0_const<pb::UnknownField const, pb::uint32, &pb::UnknownField::fixed32>::value },
	{ "fixed64", BindLuaFunc_0_const<pb::UnknownField const, pb::uint64, &pb::UnknownField::fixed64>::value },
	{ "length_delimited", BindLuaFunc_0_const<pb::UnknownField const, std::string const&, &pb::UnknownField::length_delimited>::value },
	{ "type", BindLuaFunc_0_const<pb::UnknownField const,pb::UnknownField::Type, &pb::UnknownField::type>::value },
	{ "number", BindLuaFunc_0_const<pb::UnknownField const, int, &pb::UnknownField::number>::value },

	{ NULL, NULL },
};

//
// DynamicProtobuf.Message
//

//内部调用 Array 版
template <bool (pb::MessageLite::*parseFunc)(const void*, int)>
static int Message_ParseFromString_X(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	size_t dataLen;
	char const* data = luaL_checklstring(L, 2, &dataLen);
	bool ret = (self->*parseFunc)(data, (int)dataLen);
	lua_pushboolean(L, ret);
	return 1;
}

static int Message_ParseFromString(lua_State* L)
{
	return Message_ParseFromString_X<&pb::Message::ParseFromArray>(L);
}

static int Message_ParsePartialFromString(lua_State* L)
{
	return Message_ParseFromString_X<&pb::Message::ParsePartialFromArray>(L);
}

//复用 buffer
static void* s_buffer;
static size_t s_buffer_size;

//内部调用 Array 版, return 1: 序列化数据，失败时为 nil
template <bool (pb::MessageLite::*serializeFunc)(void*, int) const>
static int Message_SerializeToString_X(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	int size = self->ByteSize();

	size_t allocBufferSize = size > 0 ? size : 1;
	if (allocBufferSize > s_buffer_size)
	{
		s_buffer = realloc(s_buffer, allocBufferSize);
		s_buffer_size = allocBufferSize;
	}

	if (!s_buffer)
	{
		lua_pushnil(L);
		return 1;
	}

	try
	{
		bool ret = (self->*serializeFunc)(s_buffer, size);
		if (!ret)
		{
			lua_pushnil(L);
			return 1;
		}

		lua_pushlstring(L, (const char*)s_buffer, size);
		return 1;
	}
	catch (pb::FatalException e)
	{
		luaL_error(L, "%s:%d, %s", e.filename(), e.line(), e.message().c_str());
		return 0;
	}
}

static int Message_SerializeToString(lua_State* L)
{
	return Message_SerializeToString_X<&pb::Message::SerializeToArray>(L);
}

static int Message_SerializePartialToString(lua_State* L)
{
	return Message_SerializeToString_X<&pb::Message::SerializePartialToArray>(L);
}

static int Message_SpaceUsed(lua_State* L)
{
	const pb::Message* self = checkClassInstancePtr<pb::Message const>(L, 1);
	lua_pushinteger(L, self->GetReflection()->SpaceUsed(*self));
	return 1;
}

static int Message_HasField(lua_State* L)
{
	const pb::Message* self = checkClassInstancePtr<pb::Message const>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	lua_pushboolean(L, self->GetReflection()->HasField(*self, field) ? 1 : 0);
	return 1;
}

static int Message_FieldSize(lua_State* L)
{
	const pb::Message* self = checkClassInstancePtr<pb::Message const>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);

	if (!field->is_repeated())
	{
		luaL_error(L, "invalid FieldSize on non repeated field '%s'", field->full_name().c_str());
		return 0;
	}

	lua_pushinteger(L, self->GetReflection()->FieldSize(*self, field));
	return 1;
}

static int Message_ClearField(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	self->GetReflection()->ClearField(self, field);
	return 0;
}

static int Message_RemoveLast(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	self->GetReflection()->RemoveLast(self, field);
	return 0;
}

static int Message_ReleaseLast(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	pb::Message* message = self->GetReflection()->ReleaseLast(self, field);
	pushClassInstancePtr(L, message);
	return 1;
}

static int Message_Swap(lua_State* L)
{
	pb::Message* message1 = checkClassInstancePtr<pb::Message>(L, 1);
	pb::Message* message2 = checkClassInstancePtr<pb::Message>(L, 2);
	message1->GetReflection()->Swap(message1, message2);
	return 0;
}

static int Message_SwapElements(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	int index1 = luaL_checkinteger(L, 3);
	int index2 = luaL_checkinteger(L, 4);
	self->GetReflection()->SwapElements(self, field, index1, index2);
	return 0;
}

static int Message_ListFields(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	std::vector<const pb::FieldDescriptor*> output;
	self->GetReflection()->ListFields(*self, &output);

	lua_newtable(L);
	for (int i=0; i<(int)output.size(); ++i)
	{
		pushClassInstancePtr(L, output[i]);
		lua_rawseti(L, -2, i+1);
	}
	return 1;
}

static int Message_ReleaseMessage(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	pb::Message* message = self->GetReflection()->ReleaseMessage(self, field);
	pushClassInstancePtr(L, message);
	return 1;
}

/*
	释放 Message 对象
	param 1: Message (lightuserdata)
*/
static int Message_Delete(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	delete self;
	return 0;
}

std::string s_Message_Get_inner_buffer_string;
/*
	读取 Message 对象上的非 repeated 字段
	param 1: Message (lightuserdata)
	param 2: FieldDescriptor (lightuserdata)，需为 singular
	return 1: 字段值, enum 返回 EnumValueDescriptor
*/
static int Message_Get(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	if (field->is_repeated())
	{
		luaL_error(L, "invalid Get on repeated field '%s'", field->full_name().c_str());
		return 0;
	}
	
	switch (field->cpp_type())
	{
	case pb::FieldDescriptor::CPPTYPE_INT32:
	{
		pb::int32 v = self->GetReflection()->GetInt32(*self, field);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_INT64:
	{
		pb::int64 v = self->GetReflection()->GetInt64(*self, field);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT32:
	{
		pb::uint32 v = self->GetReflection()->GetUInt32(*self, field);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT64:
	{
		pb::uint64 v = self->GetReflection()->GetUInt64(*self, field);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_FLOAT:
	{
		float v = self->GetReflection()->GetFloat(*self, field);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_DOUBLE:
	{
		double v = self->GetReflection()->GetDouble(*self, field);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_BOOL:
	{
		bool v = self->GetReflection()->GetBool(*self, field);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_STRING:
	{
		std::string& v = s_Message_Get_inner_buffer_string;
		v = self->GetReflection()->GetStringReference(*self, field, &v);	//这个写法是 GetStringReference 推荐的
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_ENUM:
	{
		const pb::EnumValueDescriptor* pEnumValueDescriptor = self->GetReflection()->GetEnum(*self, field);
		PushValue(L, pEnumValueDescriptor->number());
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_MESSAGE:
	{
		pb::Message* v = self->GetReflection()->MutableMessage(self, field);
		PushValue(L, v);
		break;
	}
	default:
	{
		luaL_error(L, "invalid cpp type");
		break;
	}
	}
	return 1;
}

static std::string s_Message_Set_inner_buffer_string;
/*
	设置 Message 对象上的非 repeated 字段
	param 1: Message (lightuserdata)
	param 2: FieldDescriptor (lightuserdata)，需为 singular
	param 3: 字段值
*/
static int Message_Set(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	if (field->is_repeated())
	{
		luaL_error(L, "invalid Set on repeated field '%s'", field->full_name().c_str());
		return 0;
	}
	
	int valueIndex = 3;
	switch (field->cpp_type())
	{
	case pb::FieldDescriptor::CPPTYPE_INT32:
	{
		self->GetReflection()->SetInt32(self, field, CheckValue<pb::int32>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_INT64:
	{
		self->GetReflection()->SetInt64(self, field, CheckValue<pb::int64>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT32:
	{
		self->GetReflection()->SetUInt32(self, field, CheckValue<pb::uint32>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT64:
	{
		self->GetReflection()->SetUInt64(self, field, CheckValue<pb::uint64>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_FLOAT:
	{
		self->GetReflection()->SetFloat(self, field, CheckValue<float>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_DOUBLE:
	{
		self->GetReflection()->SetDouble(self, field, CheckValue<double>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_BOOL:
	{
		self->GetReflection()->SetBool(self, field, CheckValue<bool>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_STRING:
	{
		std::string& v = s_Message_Get_inner_buffer_string;
		size_t len;
		const char* str = luaL_checklstring(L, valueIndex, &len);
		v.assign(str, len);
		self->GetReflection()->SetString(self, field, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_ENUM:
	{
		int v = luaL_checkinteger(L, valueIndex);
		const pb::EnumValueDescriptor* pEnumValueDescriptor = field->enum_type()->FindValueByNumber(v);
		if (!pEnumValueDescriptor)
		{
			luaL_error(L, "enum number '%d' not found on enum '%s'", v, field->enum_type()->full_name().c_str());
			return 0;
		}
		self->GetReflection()->SetEnum(self, field, pEnumValueDescriptor);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_MESSAGE:
	{
		luaL_error(L, "invalid Set on message field '%s'", field->full_name().c_str());
		break;
	}
	default:
	{
		luaL_error(L, "invalid cpp type");
		break;
	}
	}
	return 0;
}

/*
	读取一个 repeated 字段值
	param 1: Message (lightuserdata)
	param 2: FieldDescriptor (lightuserdata)，需为 repeated
	param 3: index
	return 1: 字段值
*/
static int Message_GetRepeated(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	if (!field->is_repeated())
	{
		luaL_error(L, "invalid GetRepeated on non repeated field '%s'", field->full_name().c_str());
		return 0;
	}
	int index = luaL_checkinteger(L, 3);
	if (index < 0 || index >= self->GetReflection()->FieldSize(*self, field))
	{
		luaL_error(L, "bad argument #%d to 'GetRepeated' (invalid index '%d', when max is '%d')", 3, index, self->GetReflection()->FieldSize(*self, field));
		return 0;
	}

	switch (field->cpp_type())
	{
	case pb::FieldDescriptor::CPPTYPE_INT32:
	{
		pb::int32 v = self->GetReflection()->GetRepeatedInt32(*self, field, index);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_INT64:
	{
		pb::int64 v = self->GetReflection()->GetRepeatedInt64(*self, field, index);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT32:
	{
		pb::uint32 v = self->GetReflection()->GetRepeatedUInt32(*self, field, index);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT64:
	{
		pb::uint64 v = self->GetReflection()->GetRepeatedUInt64(*self, field, index);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_FLOAT:
	{
		float v = self->GetReflection()->GetRepeatedFloat(*self, field, index);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_DOUBLE:
	{
		double v = self->GetReflection()->GetRepeatedDouble(*self, field, index);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_BOOL:
	{
		bool v = self->GetReflection()->GetRepeatedBool(*self, field, index);
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_STRING:
	{
		std::string& v = s_Message_Get_inner_buffer_string;
		v = self->GetReflection()->GetRepeatedStringReference(*self, field, index, &v);	//这个写法是 GetStringReference 推荐的
		PushValue(L, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_ENUM:
	{
		const pb::EnumValueDescriptor* pEnumValueDescriptor = self->GetReflection()->GetRepeatedEnum(*self, field, index);
		PushValue(L, pEnumValueDescriptor->number());
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_MESSAGE:
	{
		pb::Message* v = self->GetReflection()->MutableRepeatedMessage(self, field, index);
		PushValue(L, v);
		break;
	}
	default:
	{
		luaL_error(L, "invalid cpp type");
		break;
	}
	}
	return 1;
}

/*
	设置一个 repeated 字段值
	param 1: Message (lightuserdata)
	param 2: FieldDescriptor (lightuserdata)，需为 repeated
	param 3: index
	param 4: 字段值
*/
static int Message_SetRepeated(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	if (!field->is_repeated())
	{
		luaL_error(L, "invalid SetRepeated on non repeated field '%s'", field->full_name().c_str());
		return 0;
	}
	int index = luaL_checkinteger(L, 3);
	if (index < 0 || index >= self->GetReflection()->FieldSize(*self, field))
	{
		luaL_argerror(L, 3, "invalid index");
		return 0;
	}

	int valueIndex = 4;
	switch (field->cpp_type())
	{
	case pb::FieldDescriptor::CPPTYPE_INT32:
	{
		self->GetReflection()->SetRepeatedInt32(self, field, index, CheckValue<pb::int32>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_INT64:
	{
		self->GetReflection()->SetRepeatedInt64(self, field, index, CheckValue<pb::int64>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT32:
	{
		self->GetReflection()->SetRepeatedUInt32(self, field, index, CheckValue<pb::uint32>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT64:
	{
		self->GetReflection()->SetRepeatedUInt64(self, field, index, CheckValue<pb::uint64>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_FLOAT:
	{
		self->GetReflection()->SetRepeatedFloat(self, field, index, CheckValue<float>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_DOUBLE:
	{
		self->GetReflection()->SetRepeatedDouble(self, field, index, CheckValue<double>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_BOOL:
	{
		self->GetReflection()->SetRepeatedBool(self, field, index, CheckValue<bool>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_STRING:
	{
		std::string& v = s_Message_Get_inner_buffer_string;
		size_t len;
		const char* str = luaL_checklstring(L, valueIndex, &len);
		v.assign(str, len);
		self->GetReflection()->SetRepeatedString(self, field, index, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_ENUM:
	{
		int v = luaL_checkinteger(L, valueIndex);
		const pb::EnumValueDescriptor* pEnumValueDescriptor = field->enum_type()->FindValueByNumber(v);
		if (!pEnumValueDescriptor)
		{
			luaL_error(L, "enum number '%d' not found on enum '%s'", v, pEnumValueDescriptor->full_name().c_str());
			return 0;
		}
		self->GetReflection()->SetRepeatedEnum(self, field, index, pEnumValueDescriptor);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_MESSAGE:
	{
		luaL_error(L, "invalid SetRepeated on message field '%s'", field->full_name().c_str());
		break;
	}
	default:
	{
		luaL_error(L, "invalid cpp type");
		break;
	}
	}
	return 0;
}

/*
	增加一个非 Message 的 repeated 字段值 (类似 Python 版的 append)
	param 1: Message (lightuserdata)
	param 2: FieldDescriptor (lightuserdata)，需为 singular
*/
static int Message_Append(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	if (!field->is_repeated())
	{
		luaL_error(L, "invalid Append on non repeated field '%s'", field->full_name().c_str());
		return 0;
	}
	
	int valueIndex = 3;
	switch (field->cpp_type())
	{
	case pb::FieldDescriptor::CPPTYPE_INT32:
	{
		self->GetReflection()->AddInt32(self, field, CheckValue<pb::int32>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_INT64:
	{
		self->GetReflection()->AddInt64(self, field, CheckValue<pb::int64>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT32:
	{
		self->GetReflection()->AddUInt32(self, field, CheckValue<pb::uint32>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_UINT64:
	{
		self->GetReflection()->AddUInt64(self, field, CheckValue<pb::uint64>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_FLOAT:
	{
		self->GetReflection()->AddFloat(self, field, CheckValue<float>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_DOUBLE:
	{
		self->GetReflection()->AddDouble(self, field, CheckValue<double>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_BOOL:
	{
		self->GetReflection()->AddBool(self, field, CheckValue<bool>::value(L, valueIndex));
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_STRING:
	{
		std::string& v = s_Message_Get_inner_buffer_string;
		size_t len;
		const char* str = luaL_checklstring(L, valueIndex, &len);
		v.assign(str, len);
		self->GetReflection()->AddString(self, field, v);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_ENUM:
	{
		int v = luaL_checkinteger(L, valueIndex);
		const pb::EnumValueDescriptor* pEnumValueDescriptor = field->enum_type()->FindValueByNumber(v);
		if (!pEnumValueDescriptor)
		{
			luaL_error(L, "enum number '%d' not found on enum '%s'", v, pEnumValueDescriptor->full_name().c_str());
			return 0;
		}
		self->GetReflection()->AddEnum(self, field, pEnumValueDescriptor);
		break;
	}
	case pb::FieldDescriptor::CPPTYPE_MESSAGE:
	{
		luaL_error(L, "invalid Append on message field '%s'", field->full_name().c_str());
		break;
	}
	default:
	{
		luaL_error(L, "invalid cpp type");
		break;
	}
	}
	return 0;
}

/*
	增加一个 Message 的 repeated 字段值，设置为默认值 (类似 Python 版的 add)
	param 1: Message (lightuserdata)
	param 2: FieldDescriptor (lightuserdata)，需为 singular
	param 3: 字段值
*/
static int Message_Add(lua_State* L)
{
	pb::Message* self = checkClassInstancePtr<pb::Message>(L, 1);
	const pb::FieldDescriptor* field = checkClassInstancePtr<pb::FieldDescriptor const>(L, 2);
	if (!field->is_repeated())
	{
		luaL_error(L, "invalid Append on non repeated field '%s'", field->full_name().c_str());
		return 0;
	}
	
	switch (field->cpp_type())
	{
	case pb::FieldDescriptor::CPPTYPE_MESSAGE:
	{
		pb::Message* message = self->GetReflection()->AddMessage(self, field, NULL);
		pushClassInstancePtr(L, message);
		break;
	}
	default:
	{
		luaL_error(L, "invalid Add on non message field '%s'", field->full_name().c_str());
		break;
	}
	}
	return 1;
}

static const struct luaL_Reg Message_funcs[] =
{
	//原始方法
	{ "New", BindLuaFunc_0_const<pb::Message const, pb::Message*, &pb::Message::New>::value },
	{ "CopyFrom", BindLuaFunc_1<pb::Message, void, pb::Message const&, &pb::Message::CopyFrom>::value },
	{ "MergeFrom", BindLuaFunc_1<pb::Message, void, pb::Message const&, &pb::Message::MergeFrom>::value },
	{ "InitializationErrorString", BindLuaFunc_0_const<pb::Message const, std::string, &pb::Message::InitializationErrorString>::value },
	{ "DiscardUnknownFields", BindLuaFunc_0<pb::Message, void, &pb::Message::DiscardUnknownFields>::value },
	{ "SpaceUsed", BindLuaFunc_0_const<pb::Message const, int, &pb::Message::SpaceUsed>::value },
	{ "DebugString", BindLuaFunc_0_const<pb::Message const, std::string, &pb::Message::DebugString>::value },
	{ "ShortDebugString", BindLuaFunc_0_const<pb::Message const, std::string, &pb::Message::ShortDebugString>::value },
	{ "Utf8DebugString", BindLuaFunc_0_const<pb::Message const, std::string, &pb::Message::Utf8DebugString>::value },
	{ "GetTypeName", BindLuaFunc_0_const<pb::Message const, std::string, &pb::Message::GetTypeName>::value },
	{ "Clear", BindLuaFunc_0<pb::Message, void, &pb::Message::Clear>::value },
	{ "IsInitialized", BindLuaFunc_0_const<pb::Message const, bool, &pb::Message::IsInitialized>::value },
	{ "CheckTypeAndMergeFrom", BindLuaFunc_1<pb::Message, void, const pb::MessageLite&, &pb::Message::CheckTypeAndMergeFrom>::value },
	{ "ByteSize", BindLuaFunc_0_const<pb::Message const, size_t, &pb::Message::ByteSizeLong>::value },
	{ "GetDescriptor", BindLuaFunc_0_const<pb::Message const, pb::Descriptor const*, &pb::Message::GetDescriptor>::value },
	{ "UnpackTo", BindLuaFunc_1_const<pb::Any const, bool, pb::Message*, &pb::Any::UnpackTo>::value },

	//适配修改
	{ "ParseFromString", Message_ParseFromString },
	{ "ParsePartialFromString", Message_ParsePartialFromString },
	{ "SerializeToString", Message_SerializeToString },
	{ "SerializePartialToString", Message_SerializePartialToString },

	//Reflection 方法
	{ "SpaceUsed", Message_SpaceUsed },
	{ "HasField", Message_HasField },
	{ "FieldSize", Message_FieldSize },
	{ "ClearField", Message_ClearField },
	{ "RemoveLast", Message_RemoveLast },
	{ "ReleaseLast", Message_ReleaseLast },
	{ "Swap", Message_Swap },
	{ "SwapElements", Message_SwapElements },
	{ "ListFields", Message_ListFields },
	{ "ReleaseMessage", Message_ReleaseMessage },

	//新增方法
	{ "Delete", Message_Delete },
	{ "Get", Message_Get },
	{ "Set", Message_Set },
	{ "GetRepeated", Message_GetRepeated },
	{ "SetRepeated", Message_SetRepeated },
	{ "Append", Message_Append },
	{ "Add", Message_Add },

	{ NULL, NULL },
};

//
// DynamicProtobuf
//

static pb::DescriptorPool* s_pDescriptorPool;
static pb::DescriptorPool& descriptorPool()
{
	if (!s_pDescriptorPool)
		s_pDescriptorPool = new pb::DescriptorPool();
	return *s_pDescriptorPool;
}

static pb::DynamicMessageFactory* s_pDynamicMessageFactory;
static pb::DynamicMessageFactory& dynamicMessageFactory()
{
	if (!s_pDynamicMessageFactory)
		s_pDynamicMessageFactory = new pb::DynamicMessageFactory();
	return *s_pDynamicMessageFactory;
}

/*
	清空全局数据，主要是 FileDescriptor 内部数据
*/
static int ClearGlobalData(lua_State *L)
{
	delete s_pDynamicMessageFactory;
	s_pDynamicMessageFactory = NULL;

	//需后删 s_pDescriptorPool，不然可能崩溃。没看出原因
	delete s_pDescriptorPool;
	s_pDescriptorPool = NULL;

	return 0;
}
void af_splitpath(const char* s, char* path, char* name, char* ext)
{
	if (path) memset(path, 0, strlen(path));
	if (name) memset(name, 0, strlen(name));
	if (ext) memset(ext, 0, strlen(ext));

	size_t n = strlen(s);
	char* p = (char*)s;
	p += n;
	bool skiped = false;
	while (p != s && *p != '.') {
		if (*p == '/' || *p == '\\') {
			skiped = true;
			break;
		}
		p--;
	}
	char pwholename[256] = { 0 };
	if (*p != '.' || skiped) {
		strcpy(pwholename, s);
	}
	else {
		if (ext) strcpy(ext, p);
		memcpy(pwholename, s, p - s);
	}

	p = pwholename;
	p += n;
	while (p != pwholename && *p != '/' && *p != '\\') p--;
	if (*p == '/' || *p == '\\') {
		p++;
		if (name) strcpy(name, p);
		if (path) memcpy(path, pwholename, p - pwholename - 1);
	}
	else {
		if (name) strcpy(name, pwholename);
	}
}

class MyReaderBuffer
{
	int readFunc;
	MyReaderBuffer()
	{
		readFunc = LUA_NOREF;
	}
	~MyReaderBuffer()
	{
		if (gL)
		{
			UnBindRef(gL);
		}
		readFunc = LUA_NOREF;
	}
public:
	static MyReaderBuffer& get()
	{
		static MyReaderBuffer inst;
		return inst;
	}
	bool ReadBuffer(const char* filename, std::vector<unsigned char>& buffer, size_t& size)
	{
		if (readFunc != LUA_NOREF && gL)
		{
			int nTop = lua_gettop(gL);
			lua_rawgeti(gL, LUA_REGISTRYINDEX, readFunc);
			lua_pushstring(gL, filename);
			int ret = lua_pcall(gL, 1, 2, 0);
			if (0 != ret)
			{
				lua_error(gL);
				lua_settop(gL, nTop);
				return false;
			}
			bool ok = false;
			if (lua_isstring(gL, -2))
			{
				size_t sz;
				const char* str = lua_tolstring(gL, -2, &sz);
				buffer.resize(sz);
				memcpy(&buffer[0], (const unsigned char*)str, sz);
				size = (size_t)lua_tointeger(gL, -1);
				ok = true;
			}
			lua_settop(gL, nTop);
			return ok;
		}
		else
		{
			FILE* fp = fopen(filename, "rb");
			if (!fp) return false;

			fseek(fp, 0, SEEK_END);
			unsigned long len = (unsigned long)ftell(fp);
			size = len;
			if (len == 0) {
				fclose(fp);
				return true;
			}
			buffer.resize(len);
			fseek(fp, 0, SEEK_SET);
			fread(&buffer[0], sizeof(unsigned char), size, fp);
			fclose(fp);
			return true;
		}
	}
	void BindRef(lua_State * L)
	{
		UnBindRef(L);
		readFunc = luaL_ref(L, LUA_REGISTRYINDEX);
	}
	void UnBindRef(lua_State * L)
	{
		if (readFunc != LUA_NOREF)
		{
			lua_unref(L, readFunc);
			readFunc = LUA_NOREF;
		}
	}
};
class DynamicProtoImportErrorCollector : public pb::io::ErrorCollector
{
	std::string fileName;
public:
	//virtual void AddError(const std::string& filename, int line, int column, const std::string& message) override
	//{
	//	std::cout << "Error:" << filename << "@" << line << ":" << column << ", err:" << message.c_str() << std::endl;
	//}
	virtual void AddError(int line, pb::io::ColumnNumber column, const std::string& message)
	{
		std::cout << "\nError: " << fileName.c_str() << "@" << line << ":" << column << ", err:" << message.c_str();
	}
public:
	DynamicProtoImportErrorCollector(const std::string& filename): fileName(filename) {}
};
/*
	从文件加载pb,自动加载依赖
	成功返回true,失败返回
*/
static bool _LoadFileDescriptor(char const* pFileName, std::vector<const pb::FileDescriptor*>& fd, std::string& err)
{
	for (auto& p : fd)
	{
		if (p->name() == pFileName)
		{
			return true;
		}
	}
	std::cout << "\nLoad FileDescriptor " << pFileName;
	size_t size = 0;
	std::vector<unsigned char> buffer;

	if(!MyReaderBuffer::get().ReadBuffer(pFileName, buffer, size))
	{
		err = std::string("ReadBuffer ") + pFileName;
		return false;
	}
	pb::FileDescriptorProto file_descriptor_proto;
	const char* pBuffer = (const char*)&buffer[0];
	std::istream* pStrStream = new std::istringstream(std::string(pBuffer, pBuffer+size));
	pb::io::IstreamInputStream* pInputStream = new pb::io::IstreamInputStream(pStrStream);
	DynamicProtoImportErrorCollector* pErrorCollector = new DynamicProtoImportErrorCollector(pFileName);
	pb::io::Tokenizer* pTokenizer = new pb::io::Tokenizer(pInputStream, pErrorCollector);
	pb::compiler::Parser parser;
	if (!parser.Parse(pTokenizer, &file_descriptor_proto))
	{
		delete pTokenizer;
		delete pInputStream;
		delete pStrStream;
		delete pErrorCollector;
		err = std::string("Parse ") + pFileName;
		return false;
	}
	else
	{
		delete pTokenizer;
		delete pInputStream;
		delete pStrStream;
		delete pErrorCollector;

		//加载依赖
		int dependency_count_ = file_descriptor_proto.dependency_size();
		for (int i = 0; i < dependency_count_; ++i)
		{
			const std::string dependency = file_descriptor_proto.dependency(i);

			char s_path[256] = { 0 };
			char s_ext[10] = { 0 };
			af_splitpath(pFileName, s_path, NULL, s_ext);

			char s_dependcyname[100] = { 0 };
			char s_path2[100] = { 0 };
			af_splitpath(dependency.c_str(), s_path2, s_dependcyname, NULL);
			std::string dependency_proto;
			if ( s_path2[0] != 0x0)
				dependency_proto = std::string(s_path) + "/" + std::string(s_path2) + "/" + std::string(s_dependcyname) + s_ext;
			else
				dependency_proto = std::string(s_path) + "/" + std::string(s_dependcyname) + s_ext;

			//这里设置名字，是为了FindFile能够找到
			file_descriptor_proto.set_dependency(i, dependency_proto.c_str());
			if (!_LoadFileDescriptor(dependency_proto.c_str(), fd, err))
			{
				return false;
			}
		}
		
		file_descriptor_proto.set_name(pFileName);

		const pb::FileDescriptor* pMainFileDescriptor = descriptorPool().BuildFile(file_descriptor_proto);
		if (pMainFileDescriptor)
		{
			fd.push_back(pMainFileDescriptor);
			return true;
		}
		err = std::string("BuildFile ") + pFileName;
		return false;
	}
}

/*
	param 1: FileDescriptor 描述文件内容 (pb 序列化数据)
	return 1: FileDescriptor 对象数组 (lightuserdata)
*/
static int LoadFileDescriptor(lua_State *L)
{
	const char* pFileName = luaL_checkstring(L, 1);

	std::string err;
	std::vector<const pb::FileDescriptor*> fileDescriptors;
	bool succ = _LoadFileDescriptor(pFileName, fileDescriptors, err);
	if (!succ){
		luaL_error(L, "LoadFileDescriptor Failed :%s, When %s", pFileName, err.c_str());
		return 0;
	}

	lua_newtable(L);
	std::vector<const pb::FileDescriptor*>::const_iterator it = fileDescriptors.begin();
	for (int i = 1; it != fileDescriptors.end(); ++it, ++i)
	{
		lua_pushnumber(L, i);
		pushClassInstancePtr(L, *it);
		lua_settable(L, -3);
	}
	return 1;
}

/*
	param 1: Descriptor 对象 (lightuserdata)
	return 1: Message Prototype 对象 (lightuserdata)
*/
static int GetMessagePrototype(lua_State *L)
{
	pb::Descriptor* pDescriptor = checkClassInstancePtr<pb::Descriptor>(L, 1);
	pb::Message const* pProtoMessage = dynamicMessageFactory().GetPrototype(pDescriptor);
	pushClassInstancePtr(L, pProtoMessage);
	return 1;
}

static int SetReadCallback(lua_State *L)
{
	MyReaderBuffer::get().BindRef(L);
	return 0;
}

static const struct luaL_Reg DynamicProtobuf_funcs[] = {
	{ "ClearGlobalData", ClearGlobalData },
	{ "LoadFileDescriptor", LoadFileDescriptor },
	{ "GetMessagePrototype", GetMessagePrototype },
	{ "SetReadCallback", SetReadCallback },
	{ NULL, NULL },
};

extern "C"
{
	LUALIB_API int luaopen_protos(lua_State *L)
	{
		gL = L;
		luaL_register(L, "DynamicProtobuf.FileDescriptor", FileDescriptor_funcs);
		luaL_register(L, "DynamicProtobuf.Descriptor", Descriptor_funcs);
		luaL_register(L, "DynamicProtobuf.EnumDescriptor", EnumDescriptor_funcs);
		luaL_register(L, "DynamicProtobuf.EnumValueDescriptor", EnumValueDescriptor_funcs);
		luaL_register(L, "DynamicProtobuf.FieldDescriptor", FieldDescriptor_funcs);
		luaL_register(L, "DynamicProtobuf.UnknownField", UnknownField_funcs);
		luaL_register(L, "DynamicProtobuf.Message", Message_funcs);

		luaL_register(L, "DynamicProtobuf", DynamicProtobuf_funcs);
		return 1;
	}
	LUALIB_API void unload_protos(lua_State * L)
	{
		gL = NULL;
	}
};
