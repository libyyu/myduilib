
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

#include "ATLComTime.h"
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
using namespace std;

namespace FConvert{
#define  MAX_TIME_BUF 128
	class mci{
	public:

		static BOOL UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase = TRUE);
		static BOOL UrlDecode(const char* szSrc, char* pBuf, int cbBufLen);

		static	CDuiString Htmlspecialchars(LPCTSTR pBuf);

		static	BOOL IsValidEmailChar(TCHAR ch);

		static	BOOL IsValidEmail(CDuiString strEmail);

		static	CDuiString GetModulePath();

		static	void  GetModulePath(CDuiString  &Path);

		static	std::string get_module_path();

		static	int file_last_modified(const char* p_file_name);

		static	int file_last_modified(const wchar_t * p_file_name);

		static	bool file_exist(const char * p_file_name);

		static	bool file_exist(const wchar_t* p_file_name);

		static	bool is_dir_exist(const char* p_dir_name);

		static	bool is_dir_exist(const wchar_t* p_dir_name);

		static	bool delete_file(const char* p_file_name);

		static	bool delete_file(const wchar_t* p_file_name);

		static	bool delete_file_undo(const TCHAR* p_file_name, bool b_file_only);

		static	bool delete_dir(const TCHAR* p_dir_name);

		static  bool make_dir(const LPCTSTR p_dir_name,LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL );

		static	bool is_directory(const char* p_file_name);

		static	bool is_directory(const wchar_t* p_file_name);

		static	CDuiString rip_file_name(const CDuiString& s_path_file, bool b_suffix);

		static CDuiString rip_file_name(const TCHAR* s_filename);

		static	CDuiString rip_file_ext(const CDuiString& s_path_file, bool b_lower);
#ifdef _UNICODE
		static	std::vector<std::wstring> get_dir_stack(const std::wstring& s_file_name, bool b_reverse);
#else
		static	std::vector<std::string> get_dir_stack(const std::string& s_file_name, bool b_reverse);
#endif
		static	bool is_file(const char* p_file_name);

		static	bool move_file(const TCHAR* p_src_file, const TCHAR* p_dest_file);

		static	CDuiString rip_path(const CDuiString& s_file_name);

		/*文件是否只读状态*/
		static	bool is_read_only(const char* p_file_name);

		static	bool is_read_only(const wchar_t* p_file_name);

		/*去掉的文件的只读状态*/
		static	bool remove_read_only(const char* p_file_name);	

		static	bool remove_read_only(const wchar_t* p_file_name);

		static	bool copy_file(const TCHAR* p_source_file, const TCHAR* p_dest_file, bool b_force);

		static	bool create_file(const TCHAR* p_file_name, bool b_force, unsigned int n_len);

		static	unsigned int get_file_size(const char* p_file_name);

		static	unsigned int get_file_size(const wchar_t* p_file_name);

		static	bool confirm_dir(const char* p_dir_name, const char c_splitter);

		static	bool confirm_dir_2(const char* p_file_name, const char c_splitter);

		static	bool confirm_dir(const wchar_t* p_dir_name, const wchar_t c_splitter);

		static	bool confirm_dir_2(const wchar_t* p_file_name, const wchar_t c_splitter);

		static	void format_time_old(int n_gmt_time, const char* p_format, char* p_output);

		static	void format_time_old(int n_gmt_time, const wchar_t* p_format, wchar_t* p_output);

		static	void format_time_new(int n_gmt_time, const char* p_format, char* p_output);

		static	void format_time(int n_gmt_time, const char* p_format, char* p_output);

		static	void format_time(int n_gmt_time, const wchar_t* p_format, wchar_t* p_output);

		static	void init_io();

		static	int local_time(int n_gmt_time=0);

		static	int now_time(int n_local_time=0);

		static	void init_time_zone();

		static	int time_diff(int n_from, int n_to);

		static void format_currenttime(char* ptime,int len);

		static CDuiString format_currenttime();

		static	unsigned int check_char(const unsigned char* p_input, unsigned int n_len, const unsigned char c_find);

		static	unsigned int check_char(const  wchar_t* p_input, unsigned int n_len, const  wchar_t c_find);

		static	__int64 get_disk_space(const TCHAR* p_disk_name);

		static	CDuiString get_max_space_disk();

		static	CDuiString get_have_dir_disk(const TCHAR* p_dir);

		static	std::string binary_2_hex(const unsigned char* p_data, unsigned int n_len);

		static	CDuiString _binary_2_hex(const unsigned char* p_data, unsigned int n_len);

		static	void confirm_splash(CDuiString& s_input, bool b_tail=true);

		static	void confirm_n_splash(CDuiString& s_input, bool b_tail=true);

		static	CDuiString link_dir_splash(const CDuiString& s_base, const CDuiString& s_sub);

		static	CDuiString fs_replace(const CDuiString& s_input, const TCHAR c_new);
#ifdef _UNICODE
		static	void shift_html_item(std::wstring& s_data);
		static	void erase_html_item(std::wstring& s_data);
#else
		static	void shift_html_item(std::string& s_data);
		static	void erase_html_item(std::string& s_data);
#endif
		//////////////////////////////////////////////////////////
		static	CDuiString ToString(char* cValue);

		static	CDuiString ToString(const char* cValue);

		static	CDuiString ToString(BYTE btValue)  ;

		static	CDuiString ToString(int iValue)  ;

		static	CDuiString ToString(unsigned int iValue)  ;

		static	CDuiString ToString(long lValue)  ;

		static	CDuiString ToString(unsigned long lValue)  ;

		static	CDuiString ToString(__int64 i64Value)  ;

		static	CDuiString ToString(unsigned __int64 i64Value)  ;

		static	CDuiString ToString(float fltValue)  ;

		static	CDuiString ToString(double dblValue)  ;

		// 时间输出格式：2010-05-06 22:07:08   
		static	CDuiString ToString(const COleDateTime& time,bool bdatetime/*=true*/)  ;

		/*
		static	CString ToString(const _variant_t& var)  ;
		*/



		/*******************************************/
		static std::string ToCharArr(LPCTSTR lpszValue);

		static	BYTE ToByte(LPCTSTR lpszValue)  ;

		static	int ToInt(LPCTSTR lpszValue)  ;

		static	unsigned int ToUInt(LPCTSTR lpszValue) ; 

		static	long ToLong(LPCTSTR lpszValue)  ;

		static	unsigned long ToULong(LPCTSTR lpszValue)  ;

		static	__int64 ToInt64(LPCTSTR lpszValue)  ;

		static	unsigned __int64 ToUInt64(LPCTSTR lpszValue)  ;

		static	float ToFloat(LPCTSTR lpszValue)  ;

		static	double ToDouble(LPCTSTR lpszValue)  ;

		// 时间格式例子：2010-05-06 22:07:08   
		//               2010 05 06 22 07 08   
		//               2010:05:06 22:07:08   
		//               2010-05-06-22-07-08   
		// 只要是"- :"分割的时间格式都符合   
		static	COleDateTime ToTime(LPCTSTR lpszValue)  ;


		static	_variant_t ToVariant(const COleDateTime& time)  ;
	};
};

#endif//__CONVERT_H