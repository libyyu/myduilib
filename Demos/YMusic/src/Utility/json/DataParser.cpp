#include "stdafx.h"
#include "DataParser.h"
#include "Utility/Convert.h"

template<typename T>
void CDataParser<T>::toWValue(value_type& v, std::wstring& o)
{
	o = em_utility::mci::UTF8ToCString(v.asCString());
}