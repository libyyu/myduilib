#define FLIB_DLL
#include "FValue.h"
#include "FMemory.h"
_FStdBegin
_flib_object_t::_flib_object_t(void* o, const char* n) :ref_count(0), name(0)
{
	ref_count++;
	object = o;
	char* s = NULL;
	if (n)
	{
		size_t len = strlen(n);
		s = (char*)FLIB_ALLOC(len + 1);
		memcpy(s, n, len);
		s[len] = '\0';
	}
	name = s;
}
_flib_object_t::_flib_object_t(_flib_object_t& other) :ref_count(0), name(0)
{
	ref_count++;
	other.ref_count++;
	object = other.object;
	if (name) FLIB_FREE((void*)name);
	name = other.name;
}
_flib_object_t& _flib_object_t::operator=(_flib_object_t& other)
{
	ref_count++;
	other.ref_count++;
	object = other.object;
	if (name) FLIB_FREE((void*)name);
	name = other.name;
	return *this;
}
_flib_object_t::~_flib_object_t()
{
	ref_count--;
	if (ref_count < 1)
	{
		if (name) FLIB_FREE((void*)name);
		name = NULL;

	}
}

_flib_enum_t::_flib_enum_t(int32 v, const char* n) :ref_count(0), name(0)
{
	ref_count++;
	value = v;
	char* s = NULL;
	if (n)
	{
		size_t len = strlen(n);
		s = (char*)FLIB_ALLOC(len + 1);
		memcpy(s, n, len);
		s[len] = '\0';
	}
	name = s;
}
_flib_enum_t::_flib_enum_t(_flib_enum_t& other) :ref_count(0), name(0)
{
	ref_count++;
	other.ref_count++;
	value = other.value;
	if (name) FLIB_FREE((void*)name);
	name = other.name;
}
_flib_enum_t& _flib_enum_t::operator=(_flib_enum_t& other)
{
	ref_count++;
	other.ref_count++;
	value = other.value;
	if (name) FLIB_FREE((void*)name);
	name = other.name;
	return *this;
}
_flib_enum_t::~_flib_enum_t()
{
	ref_count--;
	if (ref_count < 1)
	{
		if (name) FLIB_FREE((void*)name);
		name = NULL;
	}
}

void FValue::reset()
{
	if (free_handle)
	{
		switch (type)
		{
		case VALUE_TYPE_STRING:
		{
			FLIB_FREE((void*)value.str);
			value.str = NULL;
			break;
		}
		case VALUE_TYPE_WSTRING:
		{
			FLIB_FREE((void*)value.wstr);
			value.wstr = NULL;
			break;
		}
		case VALUE_TYPE_OBJECT:
		{
			if (value.object)
			{
				delete value.object;
				value.object = NULL;
			}
			break;
		}
		case VALUE_TYPE_ENUM:
		{
			if (value.e)
			{
				delete value.e;
				value.e = NULL;
			}
			break;
		}
		default:
			break;
		}
	}
	memset(&value, 0x00, sizeof(value));
	type = VALUE_TYPE_INVALID;
	free_handle = 0;
}

FValue::FValue()
{
	type = VALUE_TYPE_INVALID;
	free_handle = 0;
	memset(&value, 0x00, sizeof(value));
}

FValue::~FValue()
{
	this->reset();
}

FValue* FValue::clone()
{
	FValue* value = new FValue();
	value->copy(*this);
	return value;
}
FValue& FValue::copy(const FValue& ths)
{
	reset();
	free_handle = ths.free_handle;
	type = ths.type;
	switch (ths.type)
	{
	case VALUE_TYPE_STRING:
	{
		char* s = NULL;
		if (ths.value.str)
		{
			size_t len = strlen(ths.value.str);
			s = (char*)FLIB_ALLOC(len + 1);
			memcpy(s, ths.value.str, len);
			s[len] = '\0';
		}
		value.str = s;
		free_handle = value.str != NULL ? 1 : 0;
		break;
	}
	case VALUE_TYPE_WSTRING:
	{
		size_t size = 0;
		wchar_t* new_str = NULL;
		if (ths.value.wstr)
		{
			size = wcslen(ths.value.wstr) + 1;
			new_str = (wchar_t*)FLIB_ALLOC(size * sizeof(wchar_t));
			memcpy(new_str, ths.value.wstr, size * sizeof(wchar_t));
			new_str[size] = L'0';
		}
		value.wstr = new_str;
		free_handle = value.wstr != NULL ? 1 : 0;
		break;
	}
	case VALUE_TYPE_OBJECT:
	{
		value.object = new flib_object_t(*(ths.value.object));
		free_handle = value.object != NULL ? 1 : 0;
		break;
	}
	case VALUE_TYPE_ENUM:
	{
		value.e = new flib_enum_t(*(ths.value.e));
		free_handle = value.e != NULL ? 1 : 0;
		break;
	}
	case VALUE_TYPE_POINTER:
		value.ptr = ths.value.ptr;
		break;
	case VALUE_TYPE_BOOL:
		value.b = ths.value.b;
		break;
	case VALUE_TYPE_INT8:
		value.i8 = ths.value.i8;
		break;
	case VALUE_TYPE_UINT8:
		value.u8 = ths.value.u8;
		break;
	case VALUE_TYPE_INT16:
		value.i16 = ths.value.i16;
		break;
	case VALUE_TYPE_UINT16:
		value.u16 = ths.value.u16;
		break;
	case VALUE_TYPE_INT32:
		value.i32 = ths.value.i32;
		break;
	case VALUE_TYPE_UINT32:
		value.u32 = ths.value.u32;
		break;
	case VALUE_TYPE_INT64:
		value.i64 = ths.value.i64;
		break;
	case VALUE_TYPE_UINT64:
		value.u64 = ths.value.u64;
		break;
	case VALUE_TYPE_FLOAT:
		value.f = ths.value.f;
		break;
	case VALUE_TYPE_DOUBLE:
		value.f64 = ths.value.f64;
		break;
	default:
		break;
	}
	return *this;
}

void FValue::set(bool b)
{
	reset();
	value.b = b;
	type = VALUE_TYPE_BOOL;
	free_handle = 0;
}
void FValue::set(int8 i8)
{
	reset();
	value.i8 = i8;
	type = VALUE_TYPE_INT8;
	free_handle = 0;
}
void FValue::set(uint8 u8)
{
	reset();
	value.u8 = u8;
	type = VALUE_TYPE_UINT8;
	free_handle = 0;
}
void FValue::set(int16 i16)
{
	reset();
	value.i16 = i16;
	type = VALUE_TYPE_INT16;
	free_handle = 0;
}
void FValue::set(uint16 u16)
{
	reset();
	value.u16 = u16;
	type = VALUE_TYPE_UINT16;
	free_handle = 0;
}
void FValue::set(int32 i32)
{
	reset();
	value.i32 = i32;
	type = VALUE_TYPE_INT32;
	free_handle = 0;
}
void FValue::set(uint32 u32)
{
	reset();
	value.u32 = u32;
	type = VALUE_TYPE_UINT32;
	free_handle = 0;
}
void FValue::set(int64 i64)
{
	reset();
	value.i64 = i64;
	type = VALUE_TYPE_INT64;
	free_handle = 0;
}
void FValue::set(uint64 u64)
{
	reset();
	value.u64 = u64;
	type = VALUE_TYPE_UINT64;
	free_handle = 0;
}
void FValue::set(float f)
{
	reset();
	value.f = f;
	type = VALUE_TYPE_FLOAT;
	free_handle = 0;
}
void FValue::set(double d)
{
	reset();
	value.f64 = d;
	type = VALUE_TYPE_DOUBLE;
	free_handle = 0;
}
void FValue::set(const char* str)
{
	reset();
	if (value.str) FLIB_FREE((void*)value.str);
	char* s = NULL;
	if (str)
	{
		size_t len = strlen(str);
		s = (char*)FLIB_ALLOC(len + 1);
		memcpy(s, str, len);
		s[len] = '\0';
	}
	value.str = s;
	free_handle = value.str != NULL ? 1 : 0;
	type = VALUE_TYPE_STRING;
}
void FValue::set(const wchar_t* wstr)
{
	reset();
	size_t size = 0;
	wchar_t* new_str = NULL;
	if (wstr)
	{
		size = wcslen(wstr) + 1;
		new_str = (wchar_t*)FLIB_ALLOC(size * sizeof(wchar_t));
		memcpy(new_str, wstr, size * sizeof(wchar_t));
		new_str[size] = L'0';
	}
	value.wstr = new_str;
	free_handle = value.wstr != NULL ? 1 : 0;
	type = VALUE_TYPE_WSTRING;
}
void FValue::set(void* ptr)
{
	reset();
	value.ptr = ptr;
	type = VALUE_TYPE_POINTER;
	free_handle = 0;
}
void FValue::set(flib_object_t* o)
{
	reset();
	value.object = new flib_object_t(*o);
	type = VALUE_TYPE_OBJECT;
	free_handle = 1;
}
void FValue::set(flib_enum_t* e)
{
	reset();
	value.e = new flib_enum_t(*e);
	type = VALUE_TYPE_ENUM;
	free_handle = 1;
}

flib_value_type_t FValue::get_type() const
{
	return type;
}
bool FValue::get_bool() const
{
	assert(type == VALUE_TYPE_BOOL);
	return value.b;
}
int8 FValue::get_int8() const
{
	assert(type == VALUE_TYPE_INT8);
	return value.i8;
}
uint8 FValue::get_uint8() const
{
	assert(type == VALUE_TYPE_UINT8);
	return value.u8;
}
int16 FValue::get_int16() const
{
	assert(type == VALUE_TYPE_INT16);
	return value.i16;
}
uint16 FValue::get_uint16() const
{
	assert(type == VALUE_TYPE_UINT16);
	return value.u16;
}
int32 FValue::get_int32() const
{
	assert(type == VALUE_TYPE_INT32);
	return value.i32;
}
uint32 FValue::get_uint32() const
{
	assert(type == VALUE_TYPE_UINT32);
	return value.u32;
}
int64 FValue::get_int64() const
{
	assert(type == VALUE_TYPE_INT64);
	return value.i64;
}
uint64 FValue::get_uint64() const
{
	assert(type == VALUE_TYPE_UINT64);
	return value.u64;
}
float FValue::get_float() const
{
	assert(type == VALUE_TYPE_FLOAT);
	return value.f;
}
double FValue::get_double() const
{
	assert(type == VALUE_TYPE_DOUBLE);
	return value.f64;
}
const char* FValue::get_string() const
{
	assert(type == VALUE_TYPE_STRING);
	return value.str;
}
const wchar_t* FValue::get_wstring() const
{
	assert(type == VALUE_TYPE_WSTRING);
	return value.wstr;
}
void* FValue::get_pointer() const
{
	assert(type == VALUE_TYPE_POINTER);
	return value.ptr;
}
flib_object_t* FValue::get_object() const
{
	assert(type == VALUE_TYPE_OBJECT);
	return value.object;
}
flib_enum_t* FValue::get_enum() const
{
	assert(type == VALUE_TYPE_ENUM);
	return value.e;
}

_FStdEnd