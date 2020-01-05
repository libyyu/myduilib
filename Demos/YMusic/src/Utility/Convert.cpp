#include "stdafx.h"
#include "Convert.h"
#include <time.h>
#include <algorithm>
#ifdef _WIN32
#include <atlstr.h>
#include <atlconv.h>
#endif

namespace em_utility
{
	//百分号编码
	//http://zh.wikipedia.org/zh-cn/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81
	bool mci::UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, bool bUpperCase)
	{
		if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
			return false;

		size_t len_ascii = strlen(szSrc);
		if(len_ascii == 0)
		{
			pBuf[0] = 0;
			return true;
		}

		//先转换到UTF-8
		char baseChar = bUpperCase ? 'A' : 'a';
		int cchWideChar = MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, NULL, 0);
		LPWSTR pUnicode = (LPWSTR)malloc((cchWideChar + 1) * sizeof(WCHAR));
		if(pUnicode == NULL)
			return false;
		MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, pUnicode, cchWideChar + 1);

		int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, NULL, 0, NULL, NULL);
		LPSTR pUTF8 = (LPSTR)malloc((cbUTF8 + 1) * sizeof(CHAR));
		if(pUTF8 == NULL)
		{
			free(pUnicode);
			return false;
		}
		WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, pUTF8, cbUTF8 + 1, NULL, NULL);
		pUTF8[cbUTF8] = '\0';

		unsigned char c;
		int cbDest = 0; //累加
		unsigned char *pSrc = (unsigned char*)pUTF8;
		unsigned char *pDest = (unsigned char*)pBuf;
		while(*pSrc && cbDest < cbBufLen - 1)
		{
			c = *pSrc;
			if(isalpha(c) || isdigit(c) || c == '-' || c == '.' || c == '~')
			{
				*pDest = c;
				++pDest;
				++cbDest;
			}
			else if(c == ' ')
			{
				*pDest = '+';
				++pDest;
				++cbDest;
			}
			else
			{
				//检查缓冲区大小是否够用？
				if(cbDest + 3 > cbBufLen - 1)
					break;
				pDest[0] = '%';
				pDest[1] = (c >= 0xA0) ? ((c >> 4) - 10 + baseChar) : ((c >> 4) + '0');
				pDest[2] = ((c & 0xF) >= 0xA)? ((c & 0xF) - 10 + baseChar) : ((c & 0xF) + '0');
				pDest += 3;
				cbDest += 3;
			}
			++pSrc;
		}
		//null-terminator
		*pDest = '\0';
		free(pUnicode);
		free(pUTF8);
		return true;
	}

	//解码后是utf-8编码
	bool mci::UrlDecode(const char* szSrc, char* pBuf, int cbBufLen)
	{
		if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
			return false;

		size_t len_ascii = strlen(szSrc);
		if(len_ascii == 0)
		{
			pBuf[0] = 0;
			return true;
		}

		char *pUTF8 = (char*)malloc(len_ascii + 1);
		if(pUTF8 == NULL)
			return false;

		int cbDest = 0; //累加
		unsigned char *pSrc = (unsigned char*)szSrc;
		unsigned char *pDest = (unsigned char*)pUTF8;
		while(*pSrc)
		{
			if(*pSrc == '%')
			{
				*pDest = 0;
				//高位
				if(pSrc[1] >= 'A' && pSrc[1] <= 'F')
					*pDest += (pSrc[1] - 'A' + 10) * 0x10;
				else if(pSrc[1] >= 'a' && pSrc[1] <= 'f')
					*pDest += (pSrc[1] - 'a' + 10) * 0x10;
				else
					*pDest += (pSrc[1] - '0') * 0x10;

				//低位
				if(pSrc[2] >= 'A' && pSrc[2] <= 'F')
					*pDest += (pSrc[2] - 'A' + 10);
				else if(pSrc[2] >= 'a' && pSrc[2] <= 'f')
					*pDest += (pSrc[2] - 'a' + 10);
				else
					*pDest += (pSrc[2] - '0');

				pSrc += 3;
			}
			else if(*pSrc == '+')
			{
				*pDest = ' ';
				++pSrc;
			}
			else
			{
				*pDest = *pSrc;
				++pSrc;
			}
			++pDest;
			++cbDest;
		}
		//null-terminator
		*pDest = '\0';
		++cbDest;

		int cchWideChar = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, NULL, 0);
		LPWSTR pUnicode = (LPWSTR)malloc(cchWideChar * sizeof(WCHAR));
		if(pUnicode == NULL)
		{
			free(pUTF8);
			return false;
		}
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, pUnicode, cchWideChar);
		WideCharToMultiByte(CP_ACP, 0, pUnicode, cchWideChar, pBuf, cbBufLen, NULL, NULL);
		free(pUTF8);
		free(pUnicode);
		return true;
	}
	
	mci::stringtype mci::Htmlspecialchars(mci::pcchar_type pBuf)
	{
		stringtype sResult;

		pcchar_type pHeader = pBuf;
		while(_T('\0') != *pHeader)
		{
			switch(*pHeader)
			{
			case _T('&'):
				sResult += _T("&amp;");
				break;
			case _T('"'):
				sResult += _T("\\&quot;");
				break;
			case _T('\''):
				sResult += _T("\\\'");
				break;
			case _T('<'):
				sResult += _T("&lt;");
				break;
			case _T('>'):
				sResult += _T("&gt;");
				break;
			default:
				sResult += *pHeader;
			}

			++pHeader;
		}

		return sResult;
	}

	bool mci::IsValidEmailChar(mci::chartype ch)
	{
		//26个小写字母
		if( (ch>=97) && (ch<=122) ) 
			return true;

		//26个大写字母
		if( (ch>=65) && (ch<=90) )
			return true;

		//0~9
		if((ch>=48) && (ch<=57)) 
			return true;

		//_-.@
		if( ch==95 || ch==45 || ch==46 || ch==64 ) 
			return true;
		return false;
	}

	bool mci::IsValidEmail(mci::pcchar_type strEmail)
	{
		//a@b.c
		if(!strEmail || _tcslen(strEmail)<5)
			return false;
		stringtype ss(strEmail);
		//1. 首字符必须用字母
		chartype ch = strEmail[0];
		if( ((ch>=97) && (ch<=122)) || ((ch>=65) && (ch<=90)) )
		{
			int atCount =0;
			int atPos = 0;
			int dotCount = 0;
			//0已经判断过了，从1开始
			for(size_t i=1;i< _tcslen(strEmail);i++)
			{
				ch = strEmail[i];
				if(IsValidEmailChar(ch))
				{
					//"@"
					if(ch==64)
					{
						atCount ++;
						atPos = i;
					}//@符号后的"."号
					else if( (atCount>0) && (ch==46) )
						dotCount ++;
				}else
					return false;
			}
			//6. 结尾不得是字符“@”或者“.”
			if( ch == 46 ) return false;
			//2. 必须包含一个并且只有一个符号“@”
			//3. @后必须包含至少一个至多三个符号“.”
			if( (atCount!=1) || (dotCount<1) || (dotCount>3) )
				return false;
			//5. 不允许出现“@.”或者.@
			
			if(ss.find(_T("@."))>0 || ss.find(_T(".@"))>0 )
				return false;
			return true;
		}
		return false;
	}

	mci::stringtype mci::get_module_path()
	{
		TCHAR s_buf[MAX_PATH];
		s_buf[0] = 0x0;
		DWORD n_result = ::GetModuleFileName(NULL, s_buf, sizeof(TCHAR) * MAX_PATH);
		TCHAR	s_drive[MAX_PATH];
		s_drive[0] = 0x0;
		TCHAR s_dir[MAX_PATH];
		s_dir[0] = 0x0;
		TCHAR s_filename[MAX_PATH];
		s_filename[0] = 0x0;
		TCHAR s_ext[MAX_PATH];
		s_ext[0] = 0x0;
		/*errno_t n_err_no =*/ 
		_tsplitpath_s(s_buf, s_drive,MAX_PATH, s_dir, MAX_PATH,s_filename,MAX_PATH,s_ext,MAX_PATH);
		//assert(n_err_no == 0);
		_tcscpy_s(s_buf, s_drive);
		_tcscat_s(s_buf, s_dir);
		return stringtype(s_buf);
	}

	int mci::file_last_modified(mci::pcchar_type p_file_name)
	{
		if (p_file_name == NULL || _tcslen(p_file_name) == 0)
			return 0;
		assert(sizeof(time_t) == sizeof(unsigned int));
		struct stat file_info;
		if (stat(p_file_name, &file_info) == 0)
		{
			return (int)file_info.st_mtime;
		}
		else
			return 0;
	}

	bool mci::file_exist(mci::pcchar_type p_file_name)
	{
		if (p_file_name != NULL && _tcslen(p_file_name) > 0)
		{
			struct stat stat_info;
			return stat(p_file_name, &stat_info) == 0;
		}
		else
			return false;
	}

	bool mci::is_dir_exist(mci::pcchar_type p_dir_name)
	{
		if (p_dir_name == NULL || _tcslen(p_dir_name) == 0)
			return false;
		bool b_result = false;
		WIN32_FIND_DATA wfd;
		memset(&wfd, 0x0, sizeof(wfd));
		stringtype p_dir = stringtype(p_dir_name);
		std::replace(p_dir.begin(),p_dir.end(),'/', '\\');
		HANDLE hFind = FindFirstFile(p_dir.c_str(), &wfd);
		if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			b_result = true;
		}
		if (hFind != INVALID_HANDLE_VALUE)
			FindClose(hFind);
		return b_result;
	}

	bool mci::delete_file(mci::pcchar_type p_file_name)
	{
		assert(p_file_name != NULL);
		return ::remove(p_file_name) == 0;
	}
	
	bool mci::delete_file_undo(mci::pcchar_type p_file_name, bool b_file_only)
	{
		bool b_result = false;
		int n_result = 0;
		TCHAR s_buf[MAX_PATH + 2] = { 0 };
		memset(s_buf, 0x0, sizeof(TCHAR) * (MAX_PATH + 2));
		if (p_file_name == NULL || _tcslen(p_file_name) > MAX_PATH)
			return false;
		_tcscpy(s_buf, p_file_name);
		s_buf[_tcslen(p_file_name)] = NULL;
		s_buf[_tcslen(p_file_name) + 1] = NULL;
		SHFILEOPSTRUCT FileOp = { 0 };
		//FileOp.hwnd = NULL;
		FileOp.wFunc = FO_DELETE;
		FileOp.pFrom = s_buf;
		//FileOp.pTo = _T("");
		FileOp.fFlags = FOF_ALLOWUNDO | FOF_SILENT | FOF_NOCONFIRMATION;
		if (b_file_only)
			FileOp.fFlags |= FOF_FILESONLY;
		//FileOp.hNameMappings = NULL;
		//FileOp.lpszProgressTitle = _T("");
		n_result = ::SHFileOperation(&FileOp);
		if (n_result != 0)
		{
			DWORD n_err = ::GetLastError();
			b_result = false;
		}
		b_result = n_result == 0;
		return b_result;
	}

	bool mci::delete_dir(mci::pcchar_type p_dir_name)
	{
		assert(p_dir_name != NULL);
		bool b_result = false;
		unsigned int n_len = _tcslen(p_dir_name);
		chartype* p_buf = new chartype[n_len + 2];
		memset(p_buf, 0x0, sizeof(chartype) * (n_len + 2));
		SHFILEOPSTRUCT file_op;
		ZeroMemory((void*)&file_op, sizeof(SHFILEOPSTRUCT));
		_tcscpy(p_buf, p_dir_name);

		file_op.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
		file_op.hNameMappings = NULL;
		file_op.hwnd = NULL;
		file_op.lpszProgressTitle = NULL;
		file_op.pFrom = p_buf;
		file_op.pTo = NULL;
		file_op.wFunc = FO_DELETE;
		b_result = SHFileOperation(&file_op) == 0;
		delete[] p_buf;
		return b_result; 
	}

	bool mci::make_dir(mci::pcchar_type p_dir_name,LPSECURITY_ATTRIBUTES lpSecurityAttributes /* = NULL */)
	{
		assert(p_dir_name);
		if(is_dir_exist(p_dir_name))
			return true;


		return confirm_dir(p_dir_name,_T('/'));
	}

	bool mci::is_directory(mci::pcchar_type p_file_name)
	{
		assert(p_file_name != NULL);
		struct stat stat_info;
		int n_result = stat(p_file_name, &stat_info);
		if (n_result == -1)
			return false;
		return (stat_info.st_mode & S_IFDIR) == S_IFDIR;
	}


	mci::stringtype mci::rip_file_name(mci::pcchar_type s_path_file, bool b_suffix)
	{
		stringtype tmp(s_path_file);
		stringtype s_file_name;
		int n_pos = tmp.rfind(_T('\\'));
		if (n_pos > 0)
		{
			s_file_name = tmp.substr(n_pos+1);
			if (!b_suffix)
			{
				n_pos = tmp.rfind(_T('.'));
				if (n_pos > 0)
					s_file_name = s_file_name.substr(0, n_pos);
			}
		}
		return s_file_name;
	}
	
	mci::stringtype mci::rip_file_name(mci::pcchar_type s_filename)
	{
		stringtype s_file_name = _T("");
		int ilen = _tcslen(s_filename);
		pcchar_type p = s_filename+ilen-1;
		int irpos = 0;
		while (p)
		{
			if(*p==_T('.'))
				break;
			irpos++;
			p = s_filename+ilen-irpos;
		}
		for (int i=0;i<ilen-irpos;++i)
		{
			s_file_name += (s_filename[i]);
		}
		return s_file_name;
	}
	
	mci::stringtype mci::rip_file_ext(mci::pcchar_type s_path_file, bool b_lower)
	{
		stringtype s_ext;
		if (s_path_file && _tcslen(s_path_file) >0)
		{
			TCHAR s_buf[MAX_PATH];
			s_buf[0] = 0x0;
			//			_wsplitpath(s_path_file.GetString(), NULL, NULL, NULL, s_buf);
			_tsplitpath_s(s_path_file,NULL,NULL,NULL,NULL,NULL,NULL,s_buf,MAX_PATH);
			if (_tcslen(s_buf) > 0)
			{
				s_ext = s_buf;
				if (b_lower)
					std::transform(s_ext.begin(), s_ext.end(), s_ext.begin(), tolower);
				else
					std::transform(s_ext.begin(), s_ext.end(), s_ext.begin(), toupper);
			}
		}
		return s_ext;
	}

	std::vector<mci::stringtype> mci::get_dir_stack(mci::pcchar_type s_file_name, bool b_reverse)
	{
		std::vector<stringtype> list;
		if (!s_file_name || _tcslen(s_file_name) == 0) return list;
		stringtype s_item;
		chartype* w_buf = _tcsdup(s_file_name);
		if (w_buf == NULL)
			return list;
		chartype* w_token = NULL;
		w_token = _tcstok_s(w_buf, _T("\\"), NULL);
		while (w_token != NULL)
		{
			s_item = w_token;
			//boost::to_lower(s_item);
			if (!s_item.empty())
				list.push_back(s_item);
			w_token = _tcstok_s(NULL, _T("\\"), NULL);
		}
		free(w_buf);
		if (!b_reverse)
			return list;
		std::vector<stringtype> reverse_list;
		reverse_list.reserve(list.size());
		for (unsigned int i = 0; i < list.size(); ++i)
		{
			s_item = list[list.size() - i - 1];
			reverse_list.push_back(s_item);
		}
		return reverse_list;
	}

	bool mci::is_file(mci::pcchar_type p_file_name)
	{
		assert(p_file_name != NULL);
		struct stat stat_info;
		int n_result = stat(p_file_name, &stat_info);
		if (n_result == -1)
			return false;
		return !((stat_info.st_mode & S_IFDIR) == S_IFDIR);

	}

	bool mci::move_file(mci::pcchar_type p_src_file, mci::pcchar_type p_dest_file)
	{
		assert(p_src_file != NULL);
		assert(p_dest_file != NULL);
		return ::MoveFile(p_src_file, p_dest_file) == TRUE;
	}

	mci::stringtype mci::rip_path(mci::pcchar_type s_file_name)
	{
		stringtype s_path, tmp(s_file_name);
		int n_pos = tmp.rfind('\\');
		if (n_pos >= 0)
			s_path = tmp.substr(0, n_pos);
		return s_path;
	}

	/*文件是否只读状态*/
	bool mci::is_read_only(mci::pcchar_type p_file_name)
	{
		DWORD state =GetFileAttributes(p_file_name);
		return (state &FILE_ATTRIBUTE_READONLY)?true:false;
	}

	/*去掉的文件的只读状态*/
	bool mci::remove_read_only(mci::pcchar_type p_file_name)
	{
		DWORD Mask=(0xFFFFFFFF^FILE_ATTRIBUTE_READONLY);
		DWORD state =GetFileAttributes(p_file_name);
		state =state&Mask;
		return (bool)SetFileAttributes(p_file_name,state);	
	}
		
	bool mci::copy_file(mci::pcchar_type p_source_file, mci::pcchar_type p_dest_file, bool b_force)
	{
		return ::CopyFile(p_source_file, p_dest_file, b_force ? FALSE : TRUE) == TRUE;
	}

	bool mci::create_file(mci::pcchar_type p_file_name, bool b_force, unsigned int n_len)
	{
		bool b_result(false);
		DWORD n_value = 0;
		if (b_force)
			n_value = CREATE_ALWAYS;
		else
			n_value = CREATE_NEW;
		HANDLE h_file= ::CreateFile(p_file_name,GENERIC_WRITE, 	FILE_SHARE_READ, NULL, n_value, FILE_ATTRIBUTE_NORMAL, NULL);
		if (h_file != INVALID_HANDLE_VALUE)
		{
			n_value = ::SetFilePointer(h_file, n_len, NULL, FILE_BEGIN);
			assert(n_value != INVALID_SET_FILE_POINTER);
			BOOL b_EOF_resutl = ::SetEndOfFile(h_file);
			//assert(b_EOF_resutl == TRUE);
			::CloseHandle(h_file);
			b_result = b_EOF_resutl == TRUE;
		}
		return b_result;
	}

	unsigned int mci::get_file_size(mci::pcchar_type p_file_name)
	{
		assert(p_file_name != NULL);
		struct stat stat_info;
		if (stat(p_file_name, &stat_info) == 0)
			return stat_info.st_size;
		return 0;	
	}

	bool mci::confirm_dir(mci::pcchar_type p_dir_name, const mci::chartype c_splitter)
	{
		assert(p_dir_name != NULL);
		stringtype s_dir = p_dir_name;
		if (*s_dir.rbegin() == c_splitter)
			s_dir = s_dir.substr(0, s_dir.size() - 1);
		stringtype::size_type n_pos = s_dir.find(c_splitter);
		if (n_pos == stringtype::npos)
			return false;
		stringtype s_confirm;
		while (true)
		{
			n_pos = s_dir.find(c_splitter, n_pos + 1);
			if (n_pos == stringtype::npos)
			{
				s_confirm = s_dir;
				if (!is_dir_exist(s_confirm.c_str()))
				{
					if (0 != _mkdir(s_confirm.c_str()))
						return false;
				}
				break;
			}
			s_confirm = s_dir.substr(0, n_pos);
			if (!is_dir_exist(s_confirm.c_str()))
			{
				int ret = _mkdir(s_confirm.c_str());
				if (0 != ret)
					return false;
			}
			if (n_pos >= s_dir.size())
				break;
		}
		return is_dir_exist(p_dir_name);
	}
	
	bool mci::confirm_dir_2(mci::pcchar_type p_file_name, const mci::chartype c_splitter)
	{
		assert(p_file_name != NULL);
		stringtype s_file = p_file_name;
		stringtype::size_type n_pos = s_file.rfind(c_splitter);
		if (n_pos == stringtype::npos)
			return false;
		std::string s_dir = s_file.substr(0, n_pos);
		return confirm_dir(s_dir.c_str(), c_splitter);
	}

	void mci::format_time_old(int n_gmt_time, mci::pcchar_type p_format, mci::chartype* p_output)
	{
		p_output[0] = 0x0;
		assert(sizeof(time_t) == sizeof(int));
		struct tm* p_time = NULL;//localtime((const time_t*)&n_gmt_time);
		localtime_s(p_time,(const time_t*)&n_gmt_time);
		if (p_time != NULL)
		{
			assert(p_format != NULL);
			assert(_tcslen(p_format) <= MAX_TIME_BUF / 2);
			chartype s_format[MAX_TIME_BUF];
			_tcsftime(s_format, MAX_TIME_BUF - 1, p_format, p_time);
			_tcsncpy(p_output, s_format, MAX_TIME_BUF - 1);
		}
		return;
	}

	void mci::format_time_new(int n_gmt_time, mci::pcchar_type p_format, mci::chartype* p_output)
	{
		assert(p_output != NULL);
		//const unsigned short MAX_TIME_BUF = 128;
		assert(p_format != NULL);
		assert(_tcslen(p_format) <= MAX_TIME_BUF / 2);
		assert(sizeof(time_t) == sizeof(int));
		struct tm gmt_time;
		errno_t n_result = gmtime_s(&gmt_time, (time_t*)&n_gmt_time);
		assert(n_result == 0);
		chartype s_format[MAX_TIME_BUF];
		_tcsftime(s_format, MAX_TIME_BUF, p_format, &gmt_time);
		_tcsncpy(p_output, s_format, MAX_TIME_BUF);
		return;
	}
	
	void mci::format_time(int n_gmt_time, mci::pcchar_type p_format, mci::chartype* p_output)
	{
		return format_time_old(n_gmt_time, p_format, p_output);
		/*
		assert(p_output != NULL);
		//static const unsigned short MAX_TIME_BUF = 128;
		assert(p_format != NULL);
		assert(strlen(p_format) <= MAX_TIME_BUF / 2);
		assert(sizeof(time_t) == sizeof(int));
		struct tm gmt_time;
		errno_t n_result = gmtime_s(&gmt_time, (time_t*)&n_gmt_time);
		assert(n_result == 0);
		char s_format[MAX_TIME_BUF];
		strftime(s_format, MAX_TIME_BUF, p_format, &gmt_time);
		strcpy_s(p_output, MAX_TIME_BUF, s_format);
		return;
		*/
	}

	void mci::init_io()
	{
		//bf_io_base::SetBigFileName("D:\\mp3.dat");
		//bf_io_base::SetBigFileSize(1024*1024*100);
		//hf_io_base::set_root("D:\\hi");
	}
	
	int mci::local_time(int n_gmt_time/*=0*/)
	{
		/*
		assert(sizeof(time_t) == sizeof(int));
		int n_timezone = timezone;
		if (n_gmt_time == 0)
		return (int)(now_time() + n_timezone);
		else
		return n_gmt_time + n_timezone;
		*/
		//		assert(sizeof(time_t) == sizeof(int));
		long n_timezone = 0;
		_get_timezone(&n_timezone);
		//int n_timezone = timezone;
		if (n_gmt_time == 0)
			return (int)time(NULL);
		else
			return n_gmt_time - n_timezone;
	}

	int mci::now_time(int n_local_time/*=0*/)
	{
		//		assert(sizeof(time_t) == sizeof(int));
		long n_timezone = 0;
		_get_timezone(&n_timezone);
		//int n_timezone = timezone;
		int n_local = 0;
		if (n_local_time == 0)
			n_local = local_time();
		else
			n_local = n_local_time;
		return n_local + n_timezone;
		/*
		assert(sizeof(time_t) == sizeof(int));
		return (int)time(NULL);
		*/
	}

	void mci::init_time_zone()
	{
		assert(sizeof(time_t) == sizeof(int));
		int n_gmt_now = now_time();
		char s_buf[256];
		memset(s_buf, 0x0, 256);
		long n_timezone = 0;
		_get_timezone(&n_timezone);
		format_time_old(n_gmt_now, "%Y.%m.%d - %H:%M:%S", s_buf);
		n_timezone = 0;
		_get_timezone(&n_timezone);
		return;
	}
	
	int mci::time_diff(int n_from, int n_to)
	{
		assert(sizeof(time_t) == sizeof(int));
		if (n_from == 0)
			return 0;
		assert(n_from > 0);
		if (n_to == 0)
			n_to = now_time();
		double n_diff = difftime(n_to, n_from);
		return static_cast<int>(n_diff);
	}

	void mci::format_currenttime(mci::chartype* ptime,int len)
	{
		time_t t = time(0); 
		struct tm tt;
		localtime_s(&tt,&t);
		_tcsftime( ptime, sizeof(ptime), _T("[%Y-%m-%d %H:%M:%S]"), &tt);
	}

	mci::stringtype mci::format_currenttime()
	{
		SYSTEMTIME time = {0};
		TCHAR szTime[MAX_PATH] = {0};
		::GetLocalTime( &time );
#if defined(UNDER_CE)
		_stprintf( szTime, _T("%04d-%02d-%02d %02d:%02d:%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
#else
		_stprintf_s( szTime, MAX_PATH, _T("%04d-%02d-%02d %02d:%02d:%02d"), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
#endif

		return szTime;
	}
	unsigned int mci::check_char(mci::pcchar_type p_input, unsigned int n_len, const mci::chartype c_find)
	{
		assert(p_input != NULL && n_len > 0);
		unsigned int i = 0;
		for (; i < n_len; ++i)
		{
			if (*(p_input +i) == c_find)
				break;
		}
		return i;
	}

	__int64 mci::get_disk_space(mci::pcchar_type p_disk_name)
	{
		__int64 i64_free_bytes_2_caller = 0;
		__int64 i64_total_bytes = 0;
		__int64 i64_free_bytes = 0;
		if (_tcscmp(p_disk_name, _T("A:")) == 0 || _tcscmp(p_disk_name, _T("B:")) == 0)
			return 0;

		UINT nError   = ::SetErrorMode(SEM_FAILCRITICALERRORS); //如果磁盘未就绪或者不可读 
		//msdn The system does not display the critical-error-handler message box
		BOOL b_result = ::GetDiskFreeSpaceEx(p_disk_name,  (PULARGE_INTEGER)&i64_free_bytes_2_caller,
			(PULARGE_INTEGER)&i64_total_bytes,  (PULARGE_INTEGER)&i64_free_bytes);
		SetErrorMode(nError);
		return i64_free_bytes_2_caller;
	}

	mci::stringtype mci::get_max_space_disk()
	{
		DWORD n_drivers = ::GetLogicalDrives();
		static const unsigned short MAX_DISK = 26;
		stringtype s_driver;
		__int64 i64_size = 0;
		for (unsigned short i = 0; i < MAX_DISK; ++i)
		{
			//判断当前位是否有驱动器   
			if ((n_drivers & (1 << i)) != 0)   
			{
				chartype chs = _T('A') + i;
				stringtype s_name; s_name += (chs);
				s_name += _T(":");
				UINT n_type = ::GetDriveType(s_name.c_str());
				if (n_type == DRIVE_FIXED/* || n_type == DRIVE_REMOVABLE*/)
				{
					if (s_driver.empty())
					{
						s_driver = s_name;
						assert(i64_size == 0);
						i64_size = get_disk_space(s_name.c_str());
					}
					else
					{
						__int64 i64_disk_size = get_disk_space(s_name.c_str());
						if (i64_disk_size > i64_size)
						{
							s_driver = s_name;
							i64_size = i64_disk_size;
						}
					}
				}
			}
		}
		return s_driver;
	}

	mci::stringtype mci::get_have_dir_disk(mci::pcchar_type p_dir)
	{
		DWORD n_drivers = ::GetLogicalDrives();
		static const unsigned short MAX_DISK = 26;
		stringtype s_driver;
		std::vector<stringtype>	sub_dir_list;
		stringtype s_dir = p_dir;
		std::replace(s_dir.begin(), s_dir.end(), _T('/'), _T('\\'));
		size_t index = 0;
		stringtype s_sub_dir;
		for(index = 0; index <= s_dir.length(); index++)
		{
			if( index == s_dir.length())
			{
				if(s_sub_dir.length() > 0)
				{
					sub_dir_list.push_back(s_sub_dir);
					s_sub_dir=_T("");
				}
			}
			else if(s_dir[index] == '\\')
			{
				if(s_sub_dir.length() > 0)
				{
					sub_dir_list.push_back(s_sub_dir);
					s_sub_dir=_T("");
				}
			}
			else
			{
				s_sub_dir += (s_dir[index]);
			}
		}
		int n_max_count=0;
		int n_max_time=0;

		__int64 i64_size = 0;
		for (unsigned short i = 0; i < MAX_DISK; ++i)
		{
			//判断当前位是否有驱动器   
			if ((n_drivers & (1 << i)) != 0)   
			{
				chartype chs = _T('A') + i;
				stringtype s_name; s_name += chs;
				s_name += _T(":");
				UINT n_type = ::GetDriveType(s_name.c_str());
				if (n_type == DRIVE_FIXED/* || n_type == DRIVE_REMOVABLE*/)
				{
					std::vector<stringtype>::iterator iter ;
					int n_count = 0;
					stringtype s_path = s_name;
					int n_time = 0;
					for(iter =sub_dir_list.begin();iter !=  sub_dir_list.end();iter++ )
					{
						s_path += _T("\\");
						s_path +=(*iter) ;

						if(file_exist(s_path.c_str()))
						{
							n_count ++;
							n_time = file_last_modified(s_path.c_str());
						}
					}

					if(n_count > n_max_count)
					{
						n_max_count = n_count;
						s_driver = s_name;
						n_max_time = n_time;
					}
					else if(n_count == n_max_count && (n_max_time < n_time) )
					{
						n_max_count = n_count;
						s_driver = s_name;
						n_max_time = n_time;
					}
				}
			}
		}
		return s_driver;
	}

	mci::stringtype mci::binary_2_hex(mci::pcchar_type p_data, unsigned int n_len)
	{
		assert(p_data != NULL && n_len > 0);

		chartype* p_output = new chartype[n_len * 2 + 1];
		ZeroMemory(p_output, n_len * 2 + 1);
		chartype* p_pos = p_output;
		for (unsigned int i = 0; i < n_len; ++i)
		{
			_stprintf_s(p_pos, 3, _T("%02x"), p_data[i]);
			p_pos += 2;
		}
		stringtype s_result = p_output;
		delete[] p_output;
		return s_result;
	}

	void mci::confirm_splash(mci::stringtype& s_input, bool b_tail/*=true*/)
	{
		int n_size = s_input.length();
		if (n_size == 0)
		{
			s_input += _T('\\');
		}
		else
		{
			if (b_tail)		//确保尾部有
			{
				if (n_size > 0 && s_input[n_size - 1] != _T('\\') && s_input[n_size - 1] != _T('/'))
				{
					s_input += _T('\\');
				}
			}
			else			//确保头部有
			{
				if (n_size > 0 && s_input[0] != _T('\\') && s_input[0] != _T('/'))
				{
					s_input = _T('\\') + s_input;
				}
			}
		}
		return;
	}

	void mci::confirm_n_splash(mci::stringtype& s_input, bool b_tail/*=true*/)
	{
		int n_size = s_input.length();
		if (b_tail)				//确保尾部没有
		{
			if (n_size > 0 && (s_input[n_size - 1] == _T('\\') || s_input[n_size - 1] == _T('/')))
			{
				s_input = s_input.substr(0, n_size - 1);
			}
		}
		else						//确保头部没有
		{
			if (n_size > 0 && (s_input[0] == _T('\\') || s_input[0] == _T('/')))	
			{
				s_input = s_input.substr(1);
			}
		}
		return;
	}

	mci::stringtype mci::link_dir_splash(mci::pcchar_type s_base, mci::pcchar_type s_sub)
	{
		stringtype s_result = s_base;
		confirm_splash(s_result);
		stringtype s_link = s_sub;
		confirm_n_splash(s_link, false);
		s_result += s_link;
		return s_result;
	}

	mci::stringtype mci::fs_replace(mci::pcchar_type s_input, const mci::chartype c_new)
	{
		const chartype REPLACE_CHARS[] = _T("\/\\\:\*\"\?\>\<\|");

		stringtype s_output = s_input;
		unsigned int n_count = sizeof(REPLACE_CHARS) / sizeof(REPLACE_CHARS[0]);
		for (unsigned int i = 0; i < n_count; ++i)
		{
			const chartype c_old = REPLACE_CHARS[i];
			std::replace(s_output.begin(), s_output.end(), c_old, c_new);
		}
		int n_len = s_output.length();

		//对非可视ASCII字符的处理
		static const unsigned short MIN_ASCII_VALID = 30;
		for (int i = 0; i < n_len; ++i)
		{
			chartype s_value = s_output[i];
			unsigned short n_value = s_value;
			if (n_value < MIN_ASCII_VALID)
			{
				s_output[i] = c_new;
			}
		}

		if (n_len > 0)														//对点号的特殊处理
		{
			if (s_output[0] == _T('.'))									//起首点号
			{
				s_output[0] = c_new;
			}
			if (s_output[n_len - 1] == _T('.'))						//结尾点号
			{	
				s_output[n_len - 1] = c_new;
			}
		}
		return s_output;
	}

	void mci::shift_html_item(mci::stringtype& s_data)
	{
		// 		static const std::wstring FIND_LIST[] = { (L"&amp;"), (L"&lt;"), (L"&gt;"), (L"&quot;"), (L"&apos;"), (L"&nbsp;")};
		// 		static const std::wstring REPLACE_LIST[] = { (L"&"), (L"<"), (L">"), (L"\""), (L"\'"), (L" ")};
		// 
		// 		unsigned int n_count = sizeof(FIND_LIST) / sizeof(FIND_LIST[0]);
		// 		for (unsigned int i = 0; i < n_count; ++i)
		// 		{
		// 			boost::algorithm::replace_all(s_data, FIND_LIST[i], REPLACE_LIST[i]);
		// 			s_data.replace()
		// 		}

		return;
	}

	void mci::erase_html_item(mci::stringtype& s_data)
	{
		static const chartype LEFT_ARROW = _T('<');
		static const chartype RIGHT_ARROW = _T('>');
		//std::wstring s_src = s_data;
		stringtype::size_type n_pos = 0;
		int n_count = 0;
		while (true)
		{
			n_count = 0;
			n_pos = s_data.find(LEFT_ARROW, 0);		//查找第一个<
			if (n_pos == stringtype::npos)
				break;
			stringtype::size_type n_left_pos = n_pos;
			++n_count;
			++n_pos;
			while (n_pos < s_data.size())
			{
				if (s_data[n_pos] == LEFT_ARROW)
					++n_count;
				else if (s_data[n_pos] == RIGHT_ARROW)
					--n_count;
				if (n_count == 0)		//一段结束
					break;
				++n_pos;
			}
			if (n_pos >= s_data.size())
				break;
			assert(n_pos > n_left_pos);
			s_data.erase(n_left_pos, n_pos - n_left_pos + 1);
		}
		return;
	}

	
	std::string url_encoder::url_encoding(const std::string& s_input)
	{
		std::string s_output;
		for (size_t i = 0; i < s_input.size(); ++i)
		{
			unsigned char buf[4];
			memset(buf, 0x0, 4);
			if (isalnum((unsigned char)s_input[i]))
			{
				buf[0] = s_input[i];
			}
			else if (isspace((unsigned char)s_input[i]))
			{
				buf[0] = '+';
			}
			else
			{
				buf[0] = '%';
				buf[1] = conv_2_hex((unsigned char)s_input[i] >> 4);
				buf[2] = conv_2_hex((unsigned char)s_input[i] % 16);
			}
			s_output += (char*)buf;
		}
		return s_output;	
	}
	mci::stringtype mci::CheckFilePathValid(mci::pcchar_type lpszValue)
	{
		TCHAR szInValid[] = _T("<>?*\"|\\/");
		stringtype szValue(lpszValue);
		for (int i=0;i<sizeof(szInValid)/sizeof(szInValid[0]);++i)
		{
			std::replace(szValue.begin(), szValue.end(), szInValid[i], _T(' '));
		}
		return szValue;
	}


	void mci::TrimLeftChar(mci::stringtype& str, chartype c)
	{
		str.erase(0, str.find_first_not_of(c));
	}
	void mci::TrimLeft(mci::stringtype& str)
	{
		for (mci::stringtype::iterator i = str.begin(); i != str.end(); ++i)
		{
			if (!isspace(*i))
			{
				str.erase(str.begin(), i);
				return;
			}
		}
	}
	void mci::TrimRightChar(mci::stringtype& str, chartype c)
	{
		str.erase(str.find_last_not_of(c) + 1);
	}
	void mci::TrimRight(mci::stringtype& str)
	{
		if (str.empty()) return;
		for (mci::stringtype::iterator i = str.end() - 1; i != str.begin(); --i)
		{
			if (!isspace(*i))
			{
				str.erase(i + 1, str.end());
				return;
			}
		}
	}
	void mci::Trim(mci::stringtype& str)
	{
		TrimLeft(str);
		TrimRight(str);
	}
	void mci::ToLower(mci::stringtype& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), tolower);
	}
	void mci::ToUpper(mci::stringtype& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), toupper);
	}
	bool mci::StartsWith(const mci::stringtype& str, const mci::stringtype& substr)
	{
		if (substr.length() > str.length()) return false;
		for (mci::stringtype::size_type i = 0; i < substr.length(); ++i)
		{
			if (substr[i] != str[i]) return false;
		}
		return true;
	}
	bool mci::EndsWith(const mci::stringtype& str, const mci::stringtype& substr)
	{
		if (substr.length() > str.length()) return false;
		for (mci::stringtype::size_type i = str.length() - 1, j = substr.length() - 1; j > 0; --i, --j)
		{
			if (substr[j] != str[i]) return false;
		}
		return true;
	}
	bool mci::IsEqual(const mci::stringtype& str1, const mci::stringtype& str2)
	{
		if (str1.length() != str2.length()) return false;
		mci::stringtype v1(str1);
		mci::stringtype v2(str2);
		ToLower(v1);
		ToLower(v2);
		return v1 == v2;
	}
	mci::stringtype mci::Root(const mci::stringtype& path)
	{
		size_t index = path.find_last_of(_T("/\\"));
		return (mci::stringtype::npos == index) ? mci::stringtype() : path.substr(0, index + 1);
	}
	bool mci::createdirs(pcchar_type _dir)
	{
		assert(_dir);
		if (!_dir) return false;
		mci::stringtype dir(_dir);
		Trim(dir);
		if (dir.empty()) return false;
		std::replace(dir.begin(), dir.end(), _T('\\'), _T('/'));
		while ('/' == *dir.rbegin())
			dir.erase(dir.end() - 1);

		mci::stringtype::size_type pos = dir.find(_T('/'));
		if (mci::stringtype::npos == pos)
			return false;

		mci::stringtype path;
		while (true)
		{
			pos = dir.find(_T('/'), pos + 1);
			if (pos == mci::stringtype::npos)
			{
				path = dir;
				if (!is_dir_exist(path.c_str()))
				{
#if _WIN32
					if (0 != _mkdir(path.c_str()))
						return false;
#else
					if (0 != mkdir(path.c_str(), S_IRWXU))
						return false;
#endif
				}
				break;
			}
			path = dir.substr(0, pos);
			if (!is_dir_exist(path.c_str()))
			{
#if _WIN32
				if (0 != _mkdir(path.c_str()))
					return false;
#else
				if (0 != mkdir(path.c_str(), S_IRWXU))
					return false;
#endif
			}
			if (pos >= dir.size())
				break;
		}
		return true;
	}
	bool mci::makeSurePath(mci::stringtype& s)
	{
		if (s.empty()) return false;
		std::replace(s.begin(), s.end(), _T('\\'), _T('/'));
		Trim(s);
		TrimLeftChar(s, _T('/'));
		if (s.empty()) return false;
		while (_T('/') != *s.rbegin()) s += _T('/');
		return !s.empty();
	}

#ifdef _WIN32
	std::wstring mci::UTF8ToCString(const char* str)
	{
		std::wstring rs;
		if (str && ::strlen(str) > 0)
		{
			CA2W input(str, CP_UTF8);
			rs = input;
		}
		return rs;
	}

	std::wstring mci::AnsiToCString(const char* str)
	{
		std::wstring rs;
		if (str && ::strlen(str) > 0)
		{
			CA2W input(str, CP_ACP);
			rs = input;
		}
		return rs;
	}

	std::string mci::UnicodeToUTF8(const wchar_t* str)
	{
		// 		typedef std::wstring_convert<std::codecvt_utf8<wchar_t> > w2utf8CoderT;
		// 		w2utf8CoderT coder;
		// 
		// 		std::wstring s(str);
		// 		return coder.to_bytes(s);

		std::string rs;
		if (str && ::wcslen(str) > 0)
		{
			CW2A input(str, CP_UTF8);
			rs = input;
		}
		return rs;
	}

	std::string mci::UnicodeToAnsi(const wchar_t* str)
	{
		std::string rs;
		if (str && ::wcslen(str) > 0)
		{
			CW2A input(str, CP_ACP);
			rs = input;
		}
		return rs;
	}

	std::string mci::UTF8ToAnsi(const char* str)
	{
#ifdef _UNICODE
		XString rs;
		if (str && ::strlen(str))
		{
			CString s_uni = UTF8ToCString(str);
			rs = UnicodeToAnsi(s_uni);
		}
		return rs;
#else
		return str;
#endif
	}

	std::string mci::AnsiToUTF8(const char* str)
	{
		std::string rs;
		if (str && (::strlen(str) > 0))
		{
			CA2W input(str, CP_ACP);
			rs = UnicodeToUTF8(input);
		}
		return rs;
	}
#endif
};
