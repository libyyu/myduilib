/*
一个简单的二维文本表格切分类
*/
#ifndef __FLIB_DATATABLE_H__
#define __FLIB_DATATABLE_H__
#pragma once
#include "FType.h"
#include <sstream>

_FStdBegin
class F_DLL_API FDataTable
{
	typedef std::vector<std::string> ROW_TYPE;
	typedef std::vector<ROW_TYPE> TABLE;
	TABLE _table;
public:
	FDataTable();
	FDataTable(FDataTable& ths);
	FDataTable& operator = (FDataTable& ths);
	FDataTable(TABLE& tb);
	FDataTable& operator = (TABLE& tb);
	~FDataTable();

	bool LoadFromFile(const std::string& file, bool u8Bom = true);

	bool LoadFromData(const std::string& data);

	size_t GetRowCount() const;
	size_t GetColCount(size_t row) const;
	std::string Unit(size_t row, size_t col) const;

	template<typename T>
	void Update(size_t row, size_t col, T value)
	{
		assert(row < GetRowCount());
		assert(col < GetColCount(row));
		std::stringstream ss;
		ss << value;
		_table[row][col] = ss.str();
	}

	template<typename T>
	bool Convert(const std::string & str, T& value)
	{
		std::stringstream ss;
		ss << str;
		if (!(ss >> value))
			return false;
		return true;
	}

	void Transform(size_t startrow, size_t startcolumn, size_t row_count, size_t col_count, FDataTable & new_table);

	bool GetDataRow(size_t row, size_t startcolumn) { return true; }
	template <typename TYPE1, typename ... ARG_TYPES>
	bool GetDataRow(size_t row, size_t startcolumn, TYPE1 * value, ARG_TYPES*... args)
	{
		if (_table.size() <= row) return false;
		if (_table[row].size() <= startcolumn) return false;

		Convert(_table[row][startcolumn], *value);
		return GetDataRow(row, startcolumn + 1, args...);
	}

	bool GetDataColomn(size_t col, size_t startrow) { return true; }
	template <typename TYPE1, typename ... ARG_TYPES>
	bool GetDataColomn(size_t col, size_t startrow, TYPE1 * value, ARG_TYPES*... args) 
	{ 
		if (_table.size() <= startrow) return false;
		if (_table[startrow].size() <= col) return false;

		Convert(_table[startrow][col], *value);
		return GetDataColomn(startrow+1, col, args...);
	}

	void Empty();
	bool IsEmpty() const;

	ROW_TYPE& GetRow(size_t row);

	const ROW_TYPE& GetRow(size_t row) const;

	ROW_TYPE& NewRow();
	
	ROW_TYPE& operator [](size_t index);

	virtual operator TABLE&();

	std::string ToString(const char* row_delimiter = "\r\n", const char* col_delimiter = ",");
	std::string ToString(const ROW_TYPE& row, const char* col_delimiter = ",");
protected:
	std::string EscapeString(const std::string& str);
	std::vector<std::string> Split(const std::string &str, const std::string &pattern);
private:
};

_FStdEnd
#endif//__FLIB_DATATABLE_H__

