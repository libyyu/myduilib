#include "All.h"
#include "ID3Genres.h"
#include "CharacterHelper.h"
#include "IO.h"
#include "ape_tag.h"
#include <cassert>
#ifdef _WIN32
#include <atlstr.h>
#endif
#include <boost/lexical_cast.hpp>
#include "misc.h"
#include <boost/algorithm/string/trim.hpp>

using namespace std;

namespace em_mp3_tag
{

/*****************************************************************************************
CAPETagField
*****************************************************************************************/
CAPETagField::CAPETagField(const str_utf16 * pFieldName, const void * pFieldValue, int nFieldBytes, int nFlags)
{
	// field name
	m_spFieldNameUTF16.Assign(new str_utf16 [wcslen(pFieldName) + 1], true);
	memcpy(m_spFieldNameUTF16, pFieldName, (wcslen(pFieldName) + 1) * sizeof(str_utf16));

	// data (we'll always allocate two extra bytes and memset to 0 so we're safely NULL terminated)
	m_nFieldValueBytes = max(nFieldBytes, 0);

	m_spFieldValue.Assign(new char [m_nFieldValueBytes + 2], true);
	memset(m_spFieldValue, 0x0, m_nFieldValueBytes + 2);
	if (m_nFieldValueBytes > 0)
		memcpy(m_spFieldValue, pFieldValue, m_nFieldValueBytes);

	// flags
	m_nFieldFlags = nFlags;		//这个flags是字段的flags
}

CAPETagField::~CAPETagField()
{
}

int CAPETagField::GetFieldSize()
{
	CSmartPtr<char> spFieldNameANSI(GetANSIFromUTF16(m_spFieldNameUTF16), true); 
	return static_cast<int>((strlen(spFieldNameANSI) + 1) + m_nFieldValueBytes + 4 + 4);			// 4 flags + 4 len
}

const str_utf16* CAPETagField::GetFieldName()
{
	return m_spFieldNameUTF16;
}

const char * CAPETagField::GetFieldValue()
{
	return m_spFieldValue;
}

int CAPETagField::GetFieldValueSize()
{
	return m_nFieldValueBytes;
}

int CAPETagField::GetFieldFlags()
{
	return m_nFieldFlags;
}

int CAPETagField::SaveField(char * pBuffer)
{
	*((int *) pBuffer) = m_nFieldValueBytes;		//先写4字节len
	pBuffer += 4;
	*((int *) pBuffer) = m_nFieldFlags;			//再写4字节flags
	pBuffer += 4;

	CSmartPtr<char> spFieldNameANSI((char *) GetANSIFromUTF16(m_spFieldNameUTF16), true); 
	strcpy(pBuffer, spFieldNameANSI);			//写字段名称
	pBuffer += strlen(spFieldNameANSI) + 1;

	memcpy(pBuffer, m_spFieldValue, m_nFieldValueBytes);	//写字段值

	return GetFieldSize();
}

/*****************************************************************************************
CAPETag
*****************************************************************************************/
CAPETag::CAPETag()
{
	mn_begin = 0;
	mn_length = 0;
	mn_fields = 0;
	mn_version = -1;
}

CAPETag::~CAPETag()
{
	ClearFields();
}
//////////////////////////////////////////////////////////////////////////
bool CAPETag::init(boost::shared_array<unsigned char> sp_buf, unsigned int n_begin, unsigned int n_length, APE_TAG_FOOTER* p_footer)
{
	assert(!msp_buf);
	msp_buf = sp_buf;
	mn_begin = n_begin;
	mn_length = n_length;
	return parse(p_footer);
}

bool CAPETag::parse(APE_TAG_FOOTER* p_footer)
{
	assert(msp_buf);
	assert(p_footer != NULL && p_footer->GetIsValid(false));

	// clean-up
	ClearFields();

	mn_version = p_footer->GetVersion();
	int n_field_bytes = p_footer->GetFieldBytes();
	assert(mn_length == p_footer->GetTotalTagBytes());

	char* p_buf = (char*)msp_buf.get();
	p_buf += p_footer->GetFieldsOffset();								//字段数组开始位置

	// parse out the raw fields
	int n_pos = 0;
	int n_fields = p_footer->GetNumberFields();
	for (int i = 0; i < n_fields; ++i)			//字段load循环
	{
		int n_remain = n_field_bytes - n_pos;			//剩余buf
		int n_handle_bytes = 0;
		if (n_remain <= 0)
			break;
		if (LoadField(&p_buf[n_pos], n_remain, &n_handle_bytes) != ERROR_SUCCESS)
		{
			// if LoadField(...) fails, it means that the tag is corrupt (accidently or intentionally)
			// we'll just bail out -- leaving the fields we've already set
			//assert(false);
			break;
		}
		n_pos += n_handle_bytes;
	}
	return true;
}

int CAPETag::get_length() const
{
	return mn_length;
}

int CAPETag::get_offset() const
{
	return mn_begin;
}

bool CAPETag::is_exist() const
{
	return msp_buf.operator bool();
}

bool CAPETag::is_append() const
{
	return true;
}

tag_base_data* CAPETag::get_base_tag()
{
	int n_result = 0;
	string s_value;
	tag_base_data* p_base_data = new tag_base_data;
	p_base_data->clear();
	
	p_base_data->s_title = GetFieldString(APE_TAG_FIELD_TITLE);
	p_base_data->s_artist = GetFieldString(APE_TAG_FIELD_ARTIST);
	p_base_data->s_album = GetFieldString(APE_TAG_FIELD_ALBUM);
	p_base_data->s_date = GetFieldString(APE_TAG_FIELD_YEAR);
	
	s_value = GetFieldString(APE_TAG_FIELD_TRACK);

	vector<string> items = misc::string_helper::split_string(s_value, "/");
	for (misc::string_helper::split_iter i = items.begin(); i != items.end(); ++i)
	{
		s_value = *i;
		boost::algorithm::trim(s_value);
		if (i - items.begin() == 0)
		{
			try
			{
				p_base_data->n_track = boost::lexical_cast<unsigned short>(s_value);
			}
			catch (...)
			{
				p_base_data->n_track = 0;	
			}
		}
		else if (i - items.begin() == 1)
		{
			unsigned int n_all = 0;
			try
			{
				n_all = boost::lexical_cast<unsigned int>(s_value);
			}
			catch (...)
			{
			}
		}
	}

	p_base_data->s_genre = GetFieldString(APE_TAG_FIELD_GENRE);
	return p_base_data;
}
tag_base_data_w* CAPETag::get_base_tag_wstring()
{
	int n_result = 0;
	string s_value;
	tag_base_data_w* p_base_data= new tag_base_data_w;
	p_base_data->clear();

	p_base_data->s_title = GetFieldWideString(APE_TAG_FIELD_TITLE);
	p_base_data->s_artist = GetFieldWideString(APE_TAG_FIELD_ARTIST);
	p_base_data->s_album = GetFieldWideString(APE_TAG_FIELD_ALBUM);
	p_base_data->s_date = GetFieldWideString(APE_TAG_FIELD_YEAR);

	s_value = GetFieldString(APE_TAG_FIELD_TRACK);

	vector<string> items = misc::string_helper::split_string(s_value, "/");
	for (misc::string_helper::split_iter i = items.begin(); i != items.end(); ++i)
	{
		s_value = *i;
		boost::algorithm::trim(s_value);
		if (i - items.begin() == 0)
		{
			try
			{
				p_base_data->n_track = boost::lexical_cast<unsigned short>(s_value);
			}
			catch (...)
			{
				p_base_data->n_track = 0;	
			}
		}
		else if (i - items.begin() == 1)
		{
			unsigned int n_all = 0;
			try
			{
				n_all = boost::lexical_cast<unsigned int>(s_value);
			}
			catch (...)
			{
			}
		}
	}

	p_base_data->s_genre = GetFieldWideString(APE_TAG_FIELD_GENRE);
	return p_base_data;
}

std::string CAPETag::get_tag_name()
{
	return const_enum_map::ma_tags[st_ape];
}

std::wstring CAPETag::get_tag_name_wstring()
{
	string str =const_enum_map::ma_tags[st_ape];
	return wstring(CA2W(str.c_str()).m_psz);
}

unsigned int CAPETag::get_tag_serial()
{
	return st_ape;
}

std::string CAPETag::GetFieldString(const str_utf16* p_field_name)
{
	std::string s_value;
	CAPETagField* p_field = GetTagField(p_field_name);
	if (p_field != NULL)
	{
		int n_val_len = p_field->GetFieldValueSize() + 2;
		boost::shared_array<str_utf16> sp_buf(new str_utf16[n_val_len]);
		memset(sp_buf.get(), 0x0, n_val_len);
		int n_size = n_val_len;
		int n_result = GetFieldString(p_field_name, sp_buf.get(), &n_size);
		assert(n_result == 0 && n_size > 0);
		boost::shared_array<str_ansi> sp_ansi(GetANSIFromUTF16(sp_buf.get()));
		s_value = sp_ansi.get();
	}
	return s_value;
}
//新加的返回宽字符串
std::wstring  CAPETag::GetFieldWideString(const str_utf16* p_field_name)
{
	std::wstring s_value;
	CAPETagField* p_field = GetTagField(p_field_name);
	if (p_field != NULL)
	{
		int n_val_len = p_field->GetFieldValueSize() + 2;
		boost::shared_array<str_utf16> sp_buf(new str_utf16[n_val_len]);
		memset(sp_buf.get(), 0x0, n_val_len);
		int n_size = n_val_len;
		int n_result = GetFieldString(p_field_name, sp_buf.get(), &n_size);
		assert(n_result == 0 && n_size > 0);
		
		s_value =sp_buf.get();
	}
	return s_value;
}
//////////////////////////////////////////////////////////////////////////
CAPETagField * CAPETag::GetTagField(int nIndex)
{
	assert(msp_buf);
	if ((nIndex >= 0) && (nIndex < mn_fields))
	{
		return m_aryFields[nIndex];
	}

	return NULL;
}

int CAPETag::Save(boost::shared_ptr<io_base> sp_file)
{
	if (mn_fields == 0) { return ERROR_SUCCESS; }
	int n_result = -1;

	// calculate the size of the whole tag
	int nFieldBytes = 0;
	for (int i = 0; i < mn_fields; ++i)
		nFieldBytes += m_aryFields[i]->GetFieldSize();

	// sort the fields
	SortFields();

	// build the footer
	APE_TAG_FOOTER APETagFooter(mn_fields, nFieldBytes);		//检查一般mp3文件的age tag是否包含header

	// make a buffer for the tag
	int nTotalTagBytes = APETagFooter.GetTotalTagBytes();
	boost::shared_array<char> sp_raw_tag(new char[nTotalTagBytes]);

	// save the fields
	int nLocation = 0;
	for (int i = 0; i < mn_fields; ++i)
		nLocation += m_aryFields[i]->SaveField(&sp_raw_tag[nLocation]);

	// add the footer to the buffer
	memcpy(&sp_raw_tag[nLocation], &APETagFooter, APE_TAG_FOOTER_BYTES);
	nLocation += APE_TAG_FOOTER_BYTES;

	// dump the tag to the I/O source
	n_result = append_2_file(sp_file, sp_raw_tag.get(), nTotalTagBytes);
	return n_result;
}

int CAPETag::append_2_file(boost::shared_ptr<io_base> sp_file, void* p_buffer, int n_bytes)
{
	unsigned int n_bytes_written = 0;
	sp_file->seek(0, FILE_END);
	int n_result = sp_file->write(p_buffer, n_bytes, &n_bytes_written);		//写入ape tag
	assert(n_result == 0 && n_bytes_written == n_bytes);
	return n_result;
}

int CAPETag::ClearFields()
{
	for (int i = 0; i < mn_fields; ++i)
	{
		SAFE_DELETE(m_aryFields[i])
	}
	mn_fields = 0;
	return ERROR_SUCCESS;
}

int CAPETag::GetTagFieldIndex(const str_utf16 * pFieldName)
{
	if (pFieldName == NULL) return -1;
	for (int i = 0; i < mn_fields; ++i)
	{
		if (_wcsicmp(m_aryFields[i]->GetFieldName(), pFieldName) == 0)
			return i;
	}
	return -1;

}

CAPETagField * CAPETag::GetTagField(const str_utf16 * pFieldName)
{
	int nIndex = GetTagFieldIndex(pFieldName);
	return (nIndex != -1) ? m_aryFields[nIndex] : NULL;
}

int CAPETag::GetFieldString(const str_utf16 * pFieldName, str_ansi * pBuffer, int * pBufferCharacters, BOOL bUTF8Encode)
{
	int nOriginalCharacters = *pBufferCharacters;
	if (nOriginalCharacters > MAX_FIELD_VALUE_SIZE)
		return ERROR_UNDEFINED;
	str_utf16 * pUTF16 = new str_utf16 [*pBufferCharacters + 1];
	pUTF16[0] = 0;

	int nRetVal = GetFieldString(pFieldName, pUTF16, pBufferCharacters);
	if (nRetVal == ERROR_SUCCESS)
	{
		CSmartPtr<str_ansi> spANSI(bUTF8Encode ? (str_ansi *) GetUTF8FromUTF16(pUTF16) : GetANSIFromUTF16(pUTF16), true);
		if (int(strlen(spANSI)) > nOriginalCharacters)
		{
			memset(pBuffer, 0, nOriginalCharacters * sizeof(str_ansi));
			*pBufferCharacters = 0;
			nRetVal = ERROR_UNDEFINED;
		}
		else
		{
			strcpy(pBuffer, spANSI);
			*pBufferCharacters = static_cast<int>(strlen(spANSI));
		}
	}

	delete [] pUTF16;

	return nRetVal;
}


int CAPETag::GetFieldString(const str_utf16 * pFieldName, str_utf16 * pBuffer, int * pBufferCharacters)
{
	int nRetVal = ERROR_UNDEFINED;

	if (*pBufferCharacters > 0)
	{
		CAPETagField * pAPETagField = GetTagField(pFieldName);
		if (pAPETagField == NULL)
		{
			// the field doesn't exist -- return an empty string
			memset(pBuffer, 0, *pBufferCharacters * sizeof(str_utf16));
			*pBufferCharacters = 0;
		}
		else if (pAPETagField->GetIsUTF8Text() || (mn_version < 2000))
		{
			// get the value in UTF-16 format
			CSmartPtr<str_utf16> spUTF16;
			if (mn_version >= 2000)
				spUTF16.Assign(GetUTF16FromUTF8((str_utf8 *) pAPETagField->GetFieldValue()), true);
			else
				spUTF16.Assign(GetUTF16FromANSI(pAPETagField->GetFieldValue()), true);

			// get the number of characters
			int nCharacters = static_cast<int>((wcslen(spUTF16) + 1));
			if (nCharacters > *pBufferCharacters)
			{
				// we'll fail here, because it's not clear what would get returned (null termination, size, etc.)
				// and we really don't want to cause buffer overruns on the client side
				*pBufferCharacters = nCharacters;
			}
			else
			{
				// just copy in
				*pBufferCharacters = nCharacters;
				memcpy(pBuffer, spUTF16.get_ptr(), *pBufferCharacters * sizeof(str_utf16));
				nRetVal = ERROR_SUCCESS;
			}
		}
		else
		{
			// memset the whole buffer to NULL (so everything left over is NULL terminated)
			memset(pBuffer, 0, *pBufferCharacters * sizeof(str_utf16));

			// do a binary dump (need to convert from wchar's to bytes)
			int nBufferBytes = (*pBufferCharacters - 1) * sizeof(str_utf16);
			nRetVal = GetFieldBinary(pFieldName, pBuffer, &nBufferBytes);
			*pBufferCharacters = (nBufferBytes / sizeof(str_utf16)) + 1;
		}
	}

	return nRetVal;
}

int CAPETag::GetFieldBinary(const str_utf16 * pFieldName, void * pBuffer, int * pBufferBytes)
{
	int nRetVal = ERROR_UNDEFINED;

	if (*pBufferBytes > 0)
	{
		CAPETagField * pAPETagField = GetTagField(pFieldName);
		if (pAPETagField == NULL)
		{
			memset(pBuffer, 0, *pBufferBytes);
			*pBufferBytes = 0;
		}
		else
		{
			if (pAPETagField->GetFieldValueSize() > *pBufferBytes)
			{
				// we'll fail here, because partial data may be worse than no data
				memset(pBuffer, 0, *pBufferBytes);
				*pBufferBytes = pAPETagField->GetFieldValueSize();
			}
			else
			{
				// memcpy
				*pBufferBytes = pAPETagField->GetFieldValueSize();
				memcpy(pBuffer, pAPETagField->GetFieldValue(), *pBufferBytes);
				nRetVal = ERROR_SUCCESS;
			}
		}
	}

	return nRetVal;
}

int CAPETag::LoadField(const char * pBuffer, int nMaximumBytes, int * pBytes)
{
	// set bytes to 0
	if (pBytes) *pBytes = 0;

	// size and flags
	int nLocation = 0;
	int nFieldValueSize = *((int *) &pBuffer[nLocation]);		//前4个字节为field长度
	if (nFieldValueSize <= 0 || nFieldValueSize > MAX_FIELD_VALUE_SIZE)
		return -1;
	nLocation += 4;
	int nFieldFlags = *((int *) &pBuffer[nLocation]);				//接着4个字节为flags
	nLocation += 4;

	// safety check (so we can't get buffer overflow attacked)
	int nMaximumRead = nMaximumBytes - 8 - nFieldValueSize;
	BOOL bSafe = TRUE;
	for (int z = 0; (z < nMaximumRead) && (bSafe == TRUE); z++)
	{
		int nCharacter = pBuffer[nLocation + z];
		if (nCharacter == 0)
			break;
		if ((nCharacter < 0x20) || (nCharacter > 0x7E))
			bSafe = FALSE;
	}
	if (bSafe == FALSE)
		return -1;

	// name
	int nNameCharacters = static_cast<int>(strlen(&pBuffer[nLocation]));
	if (nNameCharacters > MAX_FIELD_VALUE_SIZE)
		return -1;
	CSmartPtr<str_utf8> spNameUTF8(new str_utf8 [nNameCharacters + 1], true);
	memcpy(spNameUTF8, &pBuffer[nLocation], (nNameCharacters + 1) * sizeof(str_utf8));
	nLocation += nNameCharacters + 1;
	CSmartPtr<str_utf16> spNameUTF16(GetUTF16FromUTF8(spNameUTF8.get_ptr()), true);

	// value
	CSmartPtr<char> spFieldBuffer(new char [nFieldValueSize], true);
	memcpy(spFieldBuffer, &pBuffer[nLocation], nFieldValueSize);
	nLocation += nFieldValueSize;

	// update the bytes
	if (pBytes) *pBytes = nLocation;

	// set
	return SetFieldBinary(spNameUTF16.get_ptr(), spFieldBuffer, nFieldValueSize, nFieldFlags);
}

int CAPETag::SetFieldString(const str_utf16 * pFieldName, const str_utf16 * pFieldValue)
{
	// remove if empty
	if ((pFieldValue == NULL) || (wcslen(pFieldValue) <= 0))
		return RemoveField(pFieldName);

	// UTF-8 encode the value and call the UTF-8 SetField(...)
	CSmartPtr<str_utf8> spFieldValueUTF8(GetUTF8FromUTF16((str_utf16 *) pFieldValue), true);
	return SetFieldString(pFieldName, (const char *) spFieldValueUTF8.get_ptr(), true);
}

int CAPETag::SetFieldString(const str_utf16 * pFieldName, const char * pFieldValue, BOOL bAlreadyUTF8Encoded)
{
	// remove if empty
	if ((pFieldValue == NULL) || (strlen(pFieldValue) <= 0))
		return RemoveField(pFieldName);

	// get the length and call the binary SetField(...)
	if (bAlreadyUTF8Encoded == FALSE)
	{
		CSmartPtr<char> spUTF8((char *) GetUTF8FromANSI(pFieldValue), true);
		int nFieldBytes = static_cast<int>(strlen(spUTF8.get_ptr()));
		return SetFieldBinary(pFieldName, spUTF8.get_ptr(), nFieldBytes, TAG_FIELD_FLAG_DATA_TYPE_TEXT_UTF8);
	}
	else
	{
		int nFieldBytes = static_cast<int>(strlen(pFieldValue));
		return SetFieldBinary(pFieldName, pFieldValue, nFieldBytes, TAG_FIELD_FLAG_DATA_TYPE_TEXT_UTF8);
	}
}

int CAPETag::SetFieldBinary(const str_utf16 * pFieldName, const void * pFieldValue, int nFieldBytes, int nFieldFlags)
{
	if (pFieldName == NULL) return -1;

	// check to see if we're trying to remove the field (by setting it to NULL or an empty string)
	BOOL bRemoving = (pFieldValue == NULL) || (nFieldBytes <= 0);

	// get the index
	int nFieldIndex = GetTagFieldIndex(pFieldName);
	if (nFieldIndex != -1)
	{
		// existing field

		// fail if we're read-only (and not ignoring the read-only flag)
		if (m_aryFields[nFieldIndex]->GetIsReadOnly())
			return -1;

		// erase the existing field
		SAFE_DELETE(m_aryFields[nFieldIndex])

			if (bRemoving)
			{
				return RemoveField(nFieldIndex);
			}
	}
	else
	{
		if (bRemoving)
			return ERROR_SUCCESS;

		nFieldIndex = mn_fields;
		++mn_fields;
	}

	// create the field and add it to the field array
	m_aryFields[nFieldIndex] = new CAPETagField(pFieldName, pFieldValue, nFieldBytes, nFieldFlags);

	return ERROR_SUCCESS;
}

int CAPETag::RemoveField(int nIndex)
{
	if ((nIndex >= 0) && (nIndex < mn_fields))
	{
		SAFE_DELETE(m_aryFields[nIndex])
			memmove(&m_aryFields[nIndex], &m_aryFields[nIndex + 1], (256 - nIndex - 1) * sizeof(CAPETagField *));
		--mn_fields;
		return ERROR_SUCCESS;
	}

	return -1;
}

int CAPETag::RemoveField(const str_utf16 * pFieldName)
{
	return RemoveField(GetTagFieldIndex(pFieldName));
}

int CAPETag::SortFields()
{
	// sort the tag fields by size (so that the smallest fields are at the front of the tag)
	qsort(m_aryFields, mn_fields, sizeof(CAPETagField *), CompareFields);

	return ERROR_SUCCESS;
}

int CAPETag::CompareFields(const void * pA, const void * pB)
{
	CAPETagField * pFieldA = *((CAPETagField **) pA);
	CAPETagField * pFieldB = *((CAPETagField **) pB);

	return (pFieldA->GetFieldSize() - pFieldB->GetFieldSize());
}

}		//end namespace em_mp3_tag
