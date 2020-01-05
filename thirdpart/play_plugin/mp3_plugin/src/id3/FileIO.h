#ifndef _fileio_h_
#define _fileio_h_

#include "IO.h"

#ifdef _MSC_VER
#include <windows.h>
#else
#define MAX_PATH 255
#define INVALID_HANDLE_VALUE -1
typedef void* HANDLE;
#endif

namespace em_mp3_tag
{

class file_io : public io_base
{
public:
    // construction / destruction
    file_io();
    ~file_io();
    // open / close
    int open(const char* p_name, bool b_readonly);
	int open(const wchar_t* p_name, bool b_readonly);

    int close();
    // read / write
    int read(void* p_buffer, unsigned int n_bytes_2_read, unsigned int* p_bytes_read);
    int write(const void* p_buffer, unsigned int n_bytes_2_write, unsigned int* p_bytes_written);
    // seek
    int seek(int n_distance, unsigned int n_move_mode);
    // other functions
    int set_EOF();
    // creation / destruction
	int create(const char* p_name);
	int create(const wchar_t* p_name);
	int destroy();
    // attributes
    int get_position();
    int get_size();
	int get_name(char* p_buffer);
	int get_name(wchar_t* p_buffer);
	//////////////////////////////////////////////////////////////////////////
	//在文件头部加入一段数据
	int attach_header(unsigned char* p_buf, unsigned int n_len);
	//在文件尾部加入一段数据
	int attach_tail(unsigned char* p_buf, unsigned int n_len);
	//在文件头部截除一段数据
	int cut_header(unsigned int n_len);
	//在文件尾部截除一段数据
	int cut_tail(unsigned int n_len);
private:
#ifdef _MSC_VER
    HANDLE	mh_file;
#else
	int     mh_file;
#endif
    char			ms_file_name[MAX_PATH];
	wchar_t			ms_file_name_wchar[MAX_PATH];
    bool			mb_read_only;
	
	static const unsigned int INVALID_OFFSET = 0xFFFFFFFF;
	unsigned int mn_seek_offset;
	unsigned int mn_buf_size;
	unsigned int mn_buf_offset;
	unsigned int mn_file_size;
	char* mp_buf;
	static const unsigned int DEF_BUF_SIZE = 1024 * 4;
	bool _seek(unsigned int n_offset);
	void _reset_cache();
	bool _real_read(void* p_buffer, unsigned int n_bytes_2_read, unsigned int* p_bytes_read);
	unsigned int _fill_buf(unsigned int n_offset, unsigned int n_len);
};

}			//end namespace em_mp3_tag

#endif //_fileio_h_
