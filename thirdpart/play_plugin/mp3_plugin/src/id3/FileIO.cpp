#include "All.h"
#include "FileIO.h"
#include "CharacterHelper.h"
#include <cassert>
#include <string>
#ifdef _MSC_VER
#include <sys/types.h>  
#include <sys/stat.h>
#include <direct.h>
#else
#include <fcntl.h>   
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#endif // _MSC_VER

namespace em_mp3_tag
{
file_io::file_io()
{
#ifdef _MSC_VER
	mh_file = INVALID_HANDLE_VALUE;
#else
	mh_file = -1;
#endif // _MSC_VER

    memset(ms_file_name, 0x0, MAX_PATH);
    mb_read_only = false;

	mn_seek_offset = INVALID_OFFSET;
	mn_buf_size = 0;
	mn_buf_offset = INVALID_OFFSET;
	mn_file_size = 0;
	mp_buf = NULL;
}

file_io::~file_io()
{
    close();
	delete[] mp_buf;
}

int file_io::open(const char* p_name, bool b_readonly)
{
	assert(p_name != NULL);
	assert(strlen(p_name) < MAX_PATH);

    close();
	mb_read_only = b_readonly;
#ifdef _MSC_VER
	if (mb_read_only)
		mh_file = ::CreateFileA(p_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		mh_file = ::CreateFileA(p_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (mh_file == INVALID_HANDLE_VALUE) 
    {
		return -1;
	}
#else
	if (mb_read_only)
		mh_file = open(p_name, O_RDONLY);
	else
		mh_file = open(p_name, O_RDWR);
	if (mh_file < 0)
	{
		return -1;
	}
#endif

	strcpy(ms_file_name, p_name);		//保存文件名
	str_utf16* u16name = GetUTF16FromUTF8((const str_utf8*)p_name);
	wcscpy(ms_file_name_wchar, u16name);
	delete[] u16name;
	mn_seek_offset = 0;

    return 0;
}

int file_io::open(const wchar_t * p_name, bool b_readonly)
{
	assert(p_name != NULL);
	assert(wcslen(p_name) < MAX_PATH);

	close();
	mb_read_only = b_readonly;

	str_utf8* u8name = GetUTF8FromUTF16((const str_utf16*)p_name);
	std::string u8strname((const char*)u8name);
	delete[] u8name;

#ifdef _MSC_VER
	if (mb_read_only)
		mh_file = ::CreateFileW(p_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	else
		mh_file = ::CreateFileW(p_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (mh_file == INVALID_HANDLE_VALUE) 
	{
		return -1;
	}
#else
	if (mb_read_only)
		mh_file = open((const char*)u8name, O_RDONLY);
	else
		mh_file = open((const char*)u8name, O_RDWR);
	if (mh_file < 0)
	{
		return -1;
	}
#endif

	wcscpy(ms_file_name_wchar, p_name);		//保存文件名
	strcpy(ms_file_name, u8strname.c_str());

	mn_seek_offset = 0;

	return 0;
}

int file_io::close()
{
    SAFE_FILE_CLOSE(mh_file);
	_reset_cache();
    return 0;
}
    
int file_io::read(void* p_buffer, unsigned int n_bytes_2_read, unsigned int* p_bytes_read)
{
	assert(p_buffer != NULL);
	assert(mn_seek_offset != INVALID_OFFSET);
	if (mn_seek_offset + n_bytes_2_read > get_size())
		n_bytes_2_read = get_size() - mn_seek_offset;
	if (mn_seek_offset >= mn_buf_offset && mn_seek_offset + n_bytes_2_read <= mn_buf_offset + mn_buf_size)
	{
		//在缓存范围内
		memcpy(p_buffer, mp_buf + mn_seek_offset - mn_buf_offset , n_bytes_2_read);
		*p_bytes_read = n_bytes_2_read;
	}
	else
	{
		*p_bytes_read = _fill_buf(mn_seek_offset, n_bytes_2_read);			//will change mn_seek_offset
		assert(*p_bytes_read >= n_bytes_2_read);
		assert(mn_seek_offset == mn_buf_offset);
		memcpy(p_buffer, mp_buf, n_bytes_2_read);
		*p_bytes_read = n_bytes_2_read;
	}
	return 0;
}

int file_io::write(const void* p_buffer, unsigned int n_bytes_2_write, unsigned int* p_bytes_written)
{
	_seek(mn_seek_offset);
#ifdef _MSC_VER
	bool b_result = (bool)::WriteFile(mh_file, p_buffer, n_bytes_2_write, (unsigned long*)p_bytes_written, NULL);
#else
	int ret = ::write(mh_file, p_buffer, n_bytes_2_write);
	if (ret > 0)* p_bytes_written = ret;
	bool b_result = ret > 0;
#endif // _MSC_VER
	_reset_cache();
    if ((b_result == false) || (*p_bytes_written != n_bytes_2_write))
        return ERROR_IO_WRITE;
    else
        return 0;
}
    
int file_io::seek(int n_distance, unsigned int n_move_mode)
{
	switch (n_move_mode)
	{
	case FILE_BEGIN :
		assert(n_distance >= 0);
		mn_seek_offset = n_distance;
		break;
	case FILE_CURRENT :
		assert(mn_seek_offset != INVALID_OFFSET);
		mn_seek_offset += n_distance;
		break;
	case FILE_END :
		mn_seek_offset = get_size() + n_distance;
		break;
	}
    return 0;
}
    
int file_io::set_EOF()
{
	_seek(mn_seek_offset);
#ifdef _MSC_VER
	bool b_result = (bool)::SetEndOfFile(mh_file);			//增长或截断文件,物理操作,直接影响硬盘文件
#else
	bool b_result = lseek(mh_file, 0L, FILE_END) < 0;
#endif // _MSC_VER
	_reset_cache();
	return b_result == false ? -1 : 0;
}

int file_io::get_position()
{
	if (mn_seek_offset == INVALID_OFFSET)
#ifdef _MSC_VER
		mn_seek_offset = ::SetFilePointer(mh_file, 0, NULL, FILE_CURRENT);
#else
		mn_seek_offset = lseek(mh_file, 0, FILE_CURRENT);
#endif // _MSC_VER
	
	return static_cast<int>(mn_seek_offset);
}

int file_io::get_size()
{
	if (mn_file_size == 0)
#ifdef _MSC_VER
		mn_file_size = ::GetFileSize(mh_file, NULL);
#else
	{
		long offset = lseek(mh_file, 0, FILE_CURRENT);
		mn_file_size = lseek(mh_file, 0L, FILE_END);
		::lseek(mh_file, offset, FILE_BEGIN);
	}
#endif
	return static_cast<int>(mn_file_size);
}

int file_io::get_name(char* p_buffer)
{
    strcpy(p_buffer, ms_file_name);
    return 0;
}

int file_io::get_name(wchar_t* p_buffer)
{
	wcscpy(p_buffer, ms_file_name_wchar);
	return 0;
}

int file_io::create(const char* p_name)
{
	close();

	//创建文件
#ifdef _MSC_VER
	mh_file = ::CreateFileA(p_name, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (mh_file == INVALID_HANDLE_VALUE) 
		return -1;
#else
	mh_file = ::open(p_name, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
	if (mh_file < 0)
		return -1;
#endif
	mb_read_only = false;
	strcpy(ms_file_name, p_name);
	str_utf16* u16name = GetUTF16FromUTF8((const str_utf8*)p_name);
	wcscpy(ms_file_name_wchar, u16name);
	delete[] u16name;
	mn_seek_offset = 0;
	return 0;
}
int file_io::create(const wchar_t* p_name)
{
	close();

	str_utf8* u8name = GetUTF8FromUTF16((const str_utf16*)p_name);
	std::string u8strname((const char*)u8name);
	delete[] u8name;

	//创建文件
#ifdef _MSC_VER
	mh_file = ::CreateFileW(p_name, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (mh_file == INVALID_HANDLE_VALUE) 
		return -1;
#else
	mh_file = ::open((const char*)u8name, O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
	if (mh_file < 0)
		return -1;
#endif
	mb_read_only = false;
	wcscpy(ms_file_name_wchar, p_name);
	strcpy(ms_file_name, u8strname.c_str());
	mn_seek_offset = 0;
	return 0;
}

int file_io::destroy()
{
    close();
	_reset_cache();
	//删除文件
#ifdef _MSC_VER
	return ::DeleteFileA(ms_file_name) ? 0 : -1;
#else
	return remove(ms_file_name);
#endif
}

int file_io::attach_header(unsigned char* p_buf, unsigned int n_len)
{
	int n_result = 0;
	assert(p_buf != NULL && n_len > 0);
	unsigned int n_before_len = get_size();
	unsigned int n_after_len = n_before_len + n_len;
	n_result = seek(n_after_len, FILE_BEGIN);
	assert(n_result == 0);
	n_result = set_EOF();
	assert(n_result == 0);
	unsigned char s_buf[DATA_COPY_BUF];
	unsigned int n_bytes_2_read = 0;
	unsigned int n_bytes_readed = 0;
	unsigned int n_bytes_written = 0;
	unsigned int n_write_begin = n_after_len;
	while (n_before_len > 0)
	{
		n_bytes_2_read = MY_MIN(DATA_COPY_BUF, n_before_len);
		n_before_len -= n_bytes_2_read;
		n_result = seek(n_before_len, FILE_BEGIN);
		assert(n_result == 0);
		n_result = read(s_buf, n_bytes_2_read, &n_bytes_readed);
		assert(n_result == 0 && n_bytes_readed == n_bytes_2_read);
		n_write_begin -= n_bytes_readed;
		n_result = seek(n_write_begin, FILE_BEGIN);
		assert(n_result == 0);
		n_result = write(s_buf, n_bytes_readed, &n_bytes_written);
		assert(n_result == 0 && n_bytes_written == n_bytes_readed);
	}
	assert(n_before_len == 0 && n_write_begin == n_len);
	//写入新增的header
	n_result = seek(0, FILE_BEGIN);
	assert(n_result == 0);
	n_result = write(p_buf, n_len, &n_bytes_written);
	assert(n_result == 0 && n_bytes_written == n_len);
#ifdef _MSC_VER
	return ::FlushFileBuffers(mh_file) ? 0 : -1;
#else
	return 0;
#endif
}

int file_io::attach_tail(unsigned char* p_buf, unsigned int n_len)
{
	int n_result = 0;
	assert(p_buf != NULL && n_len > 0 && n_len < INT_MAX);
	unsigned int n_before_len = get_size();
	unsigned int n_after_len = n_before_len + n_len;
	n_result = seek((int)n_len, FILE_END);
	assert(n_result == 0);
	n_result = set_EOF();
	assert(n_result == 0);
	n_result = seek(-n_len, FILE_END);
	assert(n_result == 0);
	unsigned int n_bytes_written = 0;
	n_result = write(p_buf, n_len, &n_bytes_written);
	assert(n_result == 0 && n_bytes_written == n_len);
#ifdef _MSC_VER
	return ::FlushFileBuffers(mh_file) ? 0 : -1;
#else
	return 0;
#endif
}

int file_io::cut_header(unsigned int n_len)
{
	int n_result = 0;
	assert(n_len > 0);
	unsigned int n_before_len = get_size();
	assert(n_before_len >= n_len);
	n_result = seek(n_len, FILE_BEGIN);
	assert(n_result == 0);
	unsigned int n_remain_bytes = n_before_len - n_len;

	unsigned char s_buf[DATA_COPY_BUF];
	unsigned int n_bytes_2_read = 0;
	unsigned int n_bytes_readed = 0;
	unsigned int n_bytes_written = 0;
	unsigned int n_write_begin = 0;
	while (n_remain_bytes > 0)
	{
		n_bytes_2_read = MY_MIN(DATA_COPY_BUF, n_remain_bytes);
		n_result = seek(n_before_len - n_remain_bytes, FILE_BEGIN);
		assert(n_result == 0);
		n_result = read(s_buf, n_bytes_2_read, &n_bytes_readed);
		assert(n_result == 0 && n_bytes_readed == n_bytes_2_read);
		n_result = seek(n_write_begin, FILE_BEGIN);
		assert(n_result == 0);
		n_result = write(s_buf, n_bytes_readed, &n_bytes_written);
		assert(n_result == 0 && n_bytes_written == n_bytes_readed);
		n_write_begin += n_bytes_readed;
		n_remain_bytes -= n_bytes_readed;
	}
	assert(n_remain_bytes == 0 && n_write_begin + n_len == n_before_len);
	
	n_result = seek(-n_len, FILE_END);
	assert(n_result == 0);
	n_result = set_EOF();
	assert(n_result == 0);
#ifdef _MSC_VER
	return ::FlushFileBuffers(mh_file) ? 0 : -1;
#else
	return 0;
#endif
}

int file_io::cut_tail(unsigned int n_len)
{
	int n_result = 0;
	assert(n_len > 0);
	unsigned int n_before_len = get_size();
	assert(n_before_len >= n_len);
	n_result = seek(-n_len, FILE_END);
	assert(n_result == 0);
	n_result = set_EOF();
	assert(n_result == 0);
#ifdef _MSC_VER
	return ::FlushFileBuffers(mh_file) ? 0 : -1;
#else
	return 0;
#endif
}

void file_io::_reset_cache()
{
	if (mp_buf != NULL)
	{
		delete[] mp_buf;
		mp_buf = NULL;
	}
	mn_buf_offset = INVALID_OFFSET;
	mn_buf_size = 0;
	mn_seek_offset = INVALID_OFFSET;
	mn_file_size = 0;
}

bool file_io::_real_read(void* p_buffer, unsigned int n_bytes_2_read, unsigned int* p_bytes_read)
{
#ifdef _MSC_VER
	unsigned int n_total_bytes_read = 0;
	int n_bytes_left = n_bytes_2_read;
	bool b_result = true;
	unsigned char* p_buf = (unsigned char*)p_buffer;

	*p_bytes_read = 1;
	//这里的while为了解决什么?IO堵塞?
	while ((n_bytes_left > 0) && (*p_bytes_read > 0) && b_result)		//缓冲区仍有剩余 && 上次读>0 && 上次读正常
	{
		b_result = (bool)::ReadFile(mh_file, &p_buf[n_bytes_2_read - n_bytes_left], n_bytes_left, (unsigned long*)p_bytes_read, NULL);
		if (b_result == true)
		{
			n_bytes_left -= *p_bytes_read;
			n_total_bytes_read += *p_bytes_read;
		}
	}
	*p_bytes_read = n_total_bytes_read;
	return b_result == true;
#else
	int ret = ::read(mh_file, p_buffer, n_bytes_2_read);
	if (ret < 0) return false;
	*p_bytes_read = ret;
	return true;
#endif
}

bool file_io::_seek(unsigned int n_offset)
{
#ifdef _MSC_VER
	::SetFilePointer(mh_file, n_offset, NULL, FILE_BEGIN);
#else
	::lseek(mh_file, n_offset, FILE_BEGIN);
#endif
	mn_seek_offset = n_offset;
	return true;
}

unsigned int file_io::_fill_buf(unsigned int n_offset, unsigned int n_len)
{
	bool b_result(false);
	assert(n_offset + n_len <= get_size());
	if (mp_buf == NULL)
	{
		mn_buf_size = MY_MAX(DEF_BUF_SIZE, n_len);
		mp_buf = new char[mn_buf_size];
	}
	else
	{
		if (n_len > mn_buf_size)
		{
			_reset_cache();
			assert(n_len > DEF_BUF_SIZE);
			mn_buf_size = n_len;
			mp_buf = new char[mn_buf_size];
		}
	}
	b_result = _seek(n_offset);
	assert(b_result);
	unsigned int n_bytes_read = 0;
	b_result = _real_read(mp_buf, mn_buf_size, &n_bytes_read);
	assert(b_result);
	mn_buf_offset = n_offset;
	return n_bytes_read;
}

}		//end namespace em_mp3_tag
