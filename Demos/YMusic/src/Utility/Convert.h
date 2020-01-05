
/***********************************************************
*	FileName：Convert.h
*  Author  ：DFn Li
*  Timer   : 2013/3/2 14:24
*  E-Mail  : libyyu@qq.com
*  Comment : deal with common type conversion
*  Version : 1.0.0.1
***********************************************************/
#ifndef __CONVERT_H
#define __CONVERT_H


#pragma once

// #include <atlbase.h>
// #include <atlstr.h>
// #include <atlconv.h>

//#include "ATLComTime.h"
#include <comutil.h>   
#include <comdef.h>  
#include <wchar.h> 
#include <cassert>
#include <sys/stat.h>
#include <locale.h>
#include <windows.h>
#include <direct.h>
#include <shellapi.h>
#include <vector>

namespace em_utility
{
#define  MAX_TIME_BUF 128
	class mci{
	public:
#ifdef _UNICODE 
		typedef wchar_t              chartype;
		typedef std::wstring         stringtype;
		typedef const chartype* pcchar_type;
#else
		typedef char                 chartype;
		typedef std::string          stringtype;
		typedef const chartype* pcchar_type;
#endif

		static bool UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, bool bUpperCase);
		static bool UrlDecode(const char* szSrc, char* pBuf, int cbBufLen);

		static	stringtype Htmlspecialchars(pcchar_type pBuf);

		static	bool IsValidEmailChar(chartype ch);

		static	bool IsValidEmail(pcchar_type strEmail);

		static	stringtype get_module_path();

		static	int file_last_modified(pcchar_type p_file_name);

		static	bool file_exist(pcchar_type p_file_name);

		static	bool is_dir_exist(pcchar_type p_dir_name);

		static	bool delete_file(pcchar_type p_file_name);

		static	bool delete_file_undo(pcchar_type p_file_name, bool b_file_only);

		static	bool delete_dir(pcchar_type p_dir_name);

		static  bool make_dir(pcchar_type p_dir_name,LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL );

		static	bool is_directory(pcchar_type p_file_name);

		static	stringtype rip_file_name(pcchar_type s_path_file, bool b_suffix);

		static stringtype rip_file_name(pcchar_type s_filename);

		static	stringtype rip_file_ext(pcchar_type s_path_file, bool b_lower);

		static	std::vector<stringtype> get_dir_stack(pcchar_type s_file_name, bool b_reverse);

		static	bool is_file(pcchar_type p_file_name);

		static	bool move_file(pcchar_type p_src_file, pcchar_type p_dest_file);

		static	stringtype rip_path(pcchar_type p_file_name);

		/*文件是否只读状态*/
		static	bool is_read_only(pcchar_type p_file_name);

		/*去掉的文件的只读状态*/
		static	bool remove_read_only(pcchar_type p_file_name);

		static	bool copy_file(pcchar_type p_source_file, pcchar_type p_dest_file, bool b_force);

		static	bool create_file(pcchar_type p_file_name, bool b_force, unsigned int n_len);

		static	unsigned int get_file_size(pcchar_type p_file_name);

		static	bool confirm_dir(pcchar_type p_dir_name, const chartype c_splitter);

		static	bool confirm_dir_2(pcchar_type p_file_name, const chartype c_splitter);

		static	void format_time_old(int n_gmt_time, pcchar_type p_format, chartype* p_output);

		static	void format_time_new(int n_gmt_time, pcchar_type p_format, chartype* p_output);

		static	void format_time(int n_gmt_time, pcchar_type p_format, chartype* p_output);

		static	void init_io();

		static	int local_time(int n_gmt_time=0);

		static	int now_time(int n_local_time=0);

		static	void init_time_zone();

		static	int time_diff(int n_from, int n_to);

		static void format_currenttime(chartype* ptime,int len);

		static stringtype format_currenttime();

		static	unsigned int check_char(pcchar_type p_input, unsigned int n_len, const  chartype c_find);

		static	__int64 get_disk_space(pcchar_type p_disk_name);

		static	stringtype get_max_space_disk();

		static	stringtype get_have_dir_disk(pcchar_type p_dir);

		static	stringtype binary_2_hex(pcchar_type p_data, unsigned int n_len);

		static	void confirm_splash(stringtype& s_input, bool b_tail=true);

		static	void confirm_n_splash(stringtype& s_input, bool b_tail=true);

		static	stringtype link_dir_splash(pcchar_type s_base, pcchar_type s_sub);

		static	stringtype fs_replace(pcchar_type s_input, const chartype c_new);

		static	void shift_html_item(stringtype& s_data);

		static	void erase_html_item(stringtype& s_data);

		static  stringtype CheckFilePathValid(pcchar_type lpszValue);

		static	void TrimLeftChar(stringtype& str, chartype c);
		static	void TrimLeft(stringtype& str);
		static	void TrimRightChar(stringtype& str, chartype c);
		static	void TrimRight(stringtype& str);
		static	void Trim(stringtype& str);
		static	void ToLower(stringtype& str);
		static	void ToUpper(stringtype& str);
		static	bool StartsWith(const stringtype& str, const stringtype& substr);
		static	bool EndsWith(const stringtype& str, const stringtype& substr);
		static	bool IsEqual(const stringtype& str1, const stringtype& str2);
		static	stringtype Root(const stringtype& path);
		static	bool createdirs(pcchar_type _dir);
		static	bool makeSurePath(stringtype& s);

#ifdef _WIN32
		static	std::wstring UTF8ToCString(const char* str);
		static	std::wstring AnsiToCString(const char* str);
		static	std::string UnicodeToUTF8(const wchar_t* str);
		static	std::string UnicodeToAnsi(const wchar_t* str);
		static	std::string UTF8ToAnsi(const char* str);
		static	std::string AnsiToUTF8(const char* str);
#endif

	};

	struct url_encoder
	{
	public:
		static unsigned char conv_2_hex(const unsigned char x)
		{
			return x > 9 ? x + 55 : x + 48;
		}
		static std::string url_encoding(const std::string& s_input);
		typedef unsigned char BYTE;

		inline static BYTE toHex(const BYTE& x)
		{
			return x > 9 ? x - 10 + 'A' : x + '0';
		}

		inline static BYTE fromHex(const BYTE& x)
		{
			return isdigit(x) ? x - '0' : x - 'A' + 10;
		}

		inline static std::string URLEncode(const std::string& sIn)
		{
			std::string sOut;
			for (size_t ix = 0; ix < sIn.size(); ix++)
			{
				BYTE buf[4];
				memset(buf, 0, 4);
				if (isalnum((BYTE)sIn[ix]))
				{
					buf[0] = sIn[ix];
				}
				else if (isspace((BYTE)sIn[ix])) //貌似把空格编码成%20或者+都可以
				{
					buf[0] = '+';
				}
				else
				{
					buf[0] = '%';
					buf[1] = toHex((BYTE)sIn[ix] >> 4);
					buf[2] = toHex((BYTE)sIn[ix] % 16);
				}
				sOut += (char*)buf;
			}
			return sOut;
		};

		inline static std::string URLDecode(const std::string& sIn)
		{
			std::string sOut;
			for (size_t ix = 0; ix < sIn.size(); ix++)
			{
				BYTE ch = 0;
				if (sIn[ix] == '%')
				{
					ch = (fromHex(sIn[ix + 1]) << 4);
					ch |= fromHex(sIn[ix + 2]);
					ix += 2;
				}
				else if (sIn[ix] == '+')
				{
					ch = ' ';
				}
				else
				{
					ch = sIn[ix];
				}
				sOut += (char)ch;
			}
			return sOut;
		}
	};
};

#endif//__CONVERT_H