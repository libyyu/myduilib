#define FLIB_DLL
#include "FDataTable.h"

_FStdBegin


FDataTable::FDataTable() {}
FDataTable::FDataTable(FDataTable& ths)
{
	Empty();
	_table.swap(ths._table);
}
FDataTable& FDataTable::operator = (FDataTable& ths)
{
	Empty();
	_table.swap(ths._table);
	return *this;
}
FDataTable::FDataTable(FDataTable::TABLE& tb)
{
	Empty();
	_table.swap(tb);
}
FDataTable& FDataTable::operator = (FDataTable::TABLE& tb)
{
	Empty();
	_table.swap(tb);
	return *this;
}
FDataTable::~FDataTable()
{
	Empty();
}

bool FDataTable::LoadFromFile(const std::string& file, bool u8Bom/* = true*/)
{
	FILE* fp = fopen(file.c_str(), "r");
	if (!fp) return false;
	long dwPos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	long dwEndPos = ftell(fp);
	long len = dwEndPos - dwPos;
	fseek(fp, dwPos, SEEK_SET);

	if (u8Bom && len >= 3)
	{
		const unsigned char bomhead[] = { 0xEF, 0xBB, 0xBF };
		unsigned char bom[3] = { 0 };
		fread(bom, 1, sizeof(bom), fp);
		bool isBom = true;
		for (int i = 0; i < 3; ++i) {
			if (bom[i] != bomhead[i]) {
				isBom = false;
				break;
			}
		}

		if (!isBom) {
			fseek(fp, dwPos, SEEK_SET);
		}
		else {
			len -= 3;
		}
	}

	char* data = new char[len];
	memset(data, 0, len);
	size_t nread = fread(data, 1, len, fp);
	fclose(fp);

	data[nread] = 0x0;
	std::string u8(data);
	delete[] data;
	data = NULL;

	return LoadFromData(u8);
}

bool FDataTable::LoadFromData(const std::string& data)
{
	Empty();
	if (data.empty()) return false;
	size_t pos = 0;
	std::string grid;
	bool quotation_begin = false;
	bool quotation_end = true;
	ROW_TYPE* pRow = NULL;
	while (pos < data.length())
	{
		if (!pRow)
		{
			ROW_TYPE& row = NewRow();
			pRow = &row;
		}

		if (data[pos] == '\n' && quotation_end)
		{
			if (!grid.empty())
			{
				if (grid.back() == '\r')
				{
					grid.erase(grid.size() - 1);
				}
				pRow->push_back(grid);
				grid = "";
				quotation_begin = false;
			}

			ROW_TYPE& row = NewRow();
			pRow = &row;
		}
		else if (data[pos] == '\"')
		{
			if (!quotation_begin) { quotation_begin = true; quotation_end = false; }
			else
			{
				quotation_end = true;
			}
		}
		else if (data[pos] == ',' && quotation_end)
		{
			pRow->push_back(grid);
			grid = "";
			quotation_begin = false;
		}
		else
		{
			grid += data[pos];
		}
		pos++;
	}
	return true;
}

size_t FDataTable::GetRowCount() const { return _table.size(); }
size_t FDataTable::GetColCount(size_t row) const { return _table[row].size(); }
std::string FDataTable::Unit(size_t row, size_t col) const { return _table[row][col]; }

void FDataTable::Transform(size_t startrow, size_t startcolumn, size_t row_count, size_t col_count, FDataTable & new_table)
{
	new_table.Empty();
	for (size_t i = 0; i < _table.size(); i++)
	{
		ROW_TYPE & list = new_table.NewRow();
		size_t col_end = startcolumn + col_count;
		if (col_end > _table[i].size() || col_count == 0) col_end = _table[i].size();
		list.insert(list.begin(), col_end, std::string());
	}
}

void FDataTable::Empty()
{
	_table.clear();
}
bool FDataTable::IsEmpty() const
{
	return _table.empty();
}

FDataTable::ROW_TYPE& FDataTable::GetRow(size_t row)
{
	assert(0 <= row && row < GetRowCount());
	return _table[row];
}

const FDataTable::ROW_TYPE& FDataTable::GetRow(size_t row) const
{
	assert(0 <= row && row < GetRowCount());
	return _table[row];
}

FDataTable::ROW_TYPE& FDataTable::NewRow()
{
	_table.push_back(ROW_TYPE());
	return _table.back();
}

FDataTable::ROW_TYPE& FDataTable::operator [](size_t index)
{
	assert(index >= 0 && index < GetRowCount());
	return _table[index];
}
FDataTable::operator TABLE&()
{
	return _table;
}

std::string FDataTable::ToString(const char* row_delimiter/* = "\r\n"*/, const char* col_delimiter/* = ","*/)
{
	std::string str = "";
	for (size_t i = 0; i < _table.size(); ++i)
	{
		for (size_t j = 0; j < _table[i].size(); ++j)
		{
			str += EscapeString(_table[i][j]);
			if (j < _table[i].size() - 1)
				str += col_delimiter;
		}
		if (i < _table.size() - 1)
			str += row_delimiter;
	}
	return str;
}
std::string FDataTable::ToString(const FDataTable::ROW_TYPE& row, const char* col_delimiter/* = ","*/)
{
	std::string str = "";
	for (size_t i = 0; i < row.size(); ++i)
	{
		str += EscapeString(row[i]);
		if (i < row.size() - 1)
			str += col_delimiter;
	}
	return str;
}

std::string FDataTable::EscapeString(const std::string& str)
{
	if (str.find(',') != std::string::npos
		|| str.find(':') != std::string::npos
		|| str.find('\n') != std::string::npos)
	{
		return "\"" + str + "\"";
	}
	return str;
}
std::vector<std::string> FDataTable::Split(const std::string &str, const std::string &pattern)
{
	std::vector<std::string> resVec;
	if ("" == str) return resVec;

	std::string strs = str + pattern;
	size_t pos = strs.find(pattern);
	size_t size = strs.size();
	while (pos != std::string::npos)
	{
		std::string x = strs.substr(0, pos);
		resVec.push_back(x);
		strs = strs.substr(pos + 1, size);
		pos = strs.find(pattern);
	}

	return resVec;
}


_FStdEnd

