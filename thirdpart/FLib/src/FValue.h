#ifndef _FLIB_VALUE_H__
#define _FLIB_VALUE_H__
#pragma once
#include "FType.h"

typedef enum _flib_value_type_t {
  /**
   * @const VALUE_TYPE_INVALID
   * 无效类型。
   */
  VALUE_TYPE_INVALID = 0,
  /**
   * @const VALUE_TYPE_BOOL
   * BOOL类型。
   */
  VALUE_TYPE_BOOL,
  /**
   * @const VALUE_TYPE_INT8
   * int8_t类型。
   */
  VALUE_TYPE_INT8,
  /**
   * @const VALUE_TYPE_UINT8
   * uint8_t类型。
   */
  VALUE_TYPE_UINT8,
  /**
   * @const VALUE_TYPE_INT16
   * int16_t类型。
   */
  VALUE_TYPE_INT16,
  /**
   * @const VALUE_TYPE_UINT16
   * uint16_t类型。
   */
  VALUE_TYPE_UINT16,
  /**
   * @const VALUE_TYPE_INT32
   * int32_t类型。
   */
  VALUE_TYPE_INT32,
  /**
   * @const VALUE_TYPE_UINT32
   * uint32_t类型。
   */
  VALUE_TYPE_UINT32,
  /**
   * @const VALUE_TYPE_INT64
   * int64_t类型。
   */
  VALUE_TYPE_INT64,
  /**
   * @const VALUE_TYPE_UINT64
   * uint64_t类型。
   */
  VALUE_TYPE_UINT64,
  /**
   * @const VALUE_TYPE_FLOAT
   * float_t类型。
   */
  VALUE_TYPE_FLOAT,
  /**
   * @const VALUE_TYPE_DOUBLE
   * double类型。
   */
  VALUE_TYPE_DOUBLE,
  /**
   * @const VALUE_TYPE_STRING
   * char*类型。
   */
  VALUE_TYPE_STRING,
  /**
   * @const VALUE_TYPE_WSTRING
   * wchar_t*类型。
   */
  VALUE_TYPE_WSTRING,
  /**
   * @const VALUE_TYPE_POINTER
   * void*类型。
   */
  VALUE_TYPE_POINTER,
  /**
   * @const VALUE_TYPE_OBJECT
   * object_t*类型。
   */
  VALUE_TYPE_OBJECT,
  /**
   * @const VALUE_TYPE_ENUM
   * object_t*类型。
   */
  VALUE_TYPE_ENUM
} flib_value_type_t;

typedef struct _flib_object_t {
  char* name;
  void* object;
  size_t ref_count;
  _flib_object_t(void* o, const char* n);
  _flib_object_t(_flib_object_t& other);
  _flib_object_t& operator=(_flib_object_t& other);
  ~_flib_object_t();
}flib_object_t;

typedef struct _flib_enum_t {
    int32 value;
    char* name;
    size_t ref_count;
	_flib_enum_t(int32 v, const char* n);
	_flib_enum_t(_flib_enum_t& other);
	_flib_enum_t& operator=(_flib_enum_t& other);
	~_flib_enum_t();
}flib_enum_t;

class F_DLL_API FValue
{
    flib_value_type_t type ;
    uchar free_handle ;
    union 
    {
        int8 i8;
        uint8 u8;
        int16 i16;
        uint16 u16;
        int32 i32;
        uint32 u32;
        int64 i64;
        uint64 u64;
        float f;
        double f64;
        bool b;
        const char* str;
        const wchar_t* wstr;
        void* ptr;
        flib_object_t* object;
        flib_enum_t* e;
    } value;
	void reset();
public:
	FValue();
	~FValue();
public:
	FValue* clone();
	FValue& copy(const FValue& ths);
public:
    template<typename T> void set(T);
	void set(bool b);
	void set(int8 i8);
	void set(uint8 u8);
	void set(int16 i16);
	void set(uint16 u16);
	void set(int32 i32);
	void set(uint32 u32);
	void set(int64 i64);
	void set(uint64 u64);
	void set(float f);
	void set(double d);
	void set(const char* str);
	void set(const wchar_t* wstr);
	void set(void* ptr);
	void set(flib_object_t* o);
	void set(flib_enum_t* e);
public:
	flib_value_type_t get_type() const;
	bool get_bool() const;
	int8 get_int8() const;
	uint8 get_uint8() const;
	int16 get_int16() const;
	uint16 get_uint16() const;
	int32 get_int32() const;
	uint32 get_uint32() const;
	int64 get_int64() const;
	uint64 get_uint64() const;
	float get_float() const;
	double get_double() const;
	const char* get_string() const;
	const wchar_t* get_wstring() const;
	void* get_pointer() const;
	flib_object_t* get_object() const;
	flib_enum_t* get_enum() const;
};

#endif//_FLIB_VALUE_H__