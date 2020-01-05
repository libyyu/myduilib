#include "stdafx.h"
#include "Convert.h"
namespace FConvert
{
	//�ٷֺű���
	//http://zh.wikipedia.org/zh-cn/%E7%99%BE%E5%88%86%E5%8F%B7%E7%BC%96%E7%A0%81

	BOOL mci::UrlEncode(const char* szSrc, char* pBuf, int cbBufLen, BOOL bUpperCase)
	{
		if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
			return FALSE;

		size_t len_ascii = strlen(szSrc);
		if(len_ascii == 0)
		{
			pBuf[0] = 0;
			return TRUE;
		}

		//��ת����UTF-8
		char baseChar = bUpperCase ? 'A' : 'a';
		int cchWideChar = MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, NULL, 0);
		LPWSTR pUnicode = (LPWSTR)malloc((cchWideChar + 1) * sizeof(WCHAR));
		if(pUnicode == NULL)
			return FALSE;
		MultiByteToWideChar(CP_ACP, 0, szSrc, len_ascii, pUnicode, cchWideChar + 1);

		int cbUTF8 = WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, NULL, 0, NULL, NULL);
		LPSTR pUTF8 = (LPSTR)malloc((cbUTF8 + 1) * sizeof(CHAR));
		if(pUTF8 == NULL)
		{
			free(pUnicode);
			return FALSE;
		}
		WideCharToMultiByte(CP_UTF8, 0, pUnicode, cchWideChar, pUTF8, cbUTF8 + 1, NULL, NULL);
		pUTF8[cbUTF8] = '\0';

		unsigned char c;
		int cbDest = 0; //�ۼ�
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
				//��黺������С�Ƿ��ã�
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
		return TRUE;
	}
	//�������utf-8����
	BOOL mci::UrlDecode(const char* szSrc, char* pBuf, int cbBufLen)
	{
		if(szSrc == NULL || pBuf == NULL || cbBufLen <= 0)
			return FALSE;

		size_t len_ascii = strlen(szSrc);
		if(len_ascii == 0)
		{
			pBuf[0] = 0;
			return TRUE;
		}

		char *pUTF8 = (char*)malloc(len_ascii + 1);
		if(pUTF8 == NULL)
			return FALSE;

		int cbDest = 0; //�ۼ�
		unsigned char *pSrc = (unsigned char*)szSrc;
		unsigned char *pDest = (unsigned char*)pUTF8;
		while(*pSrc)
		{
			if(*pSrc == '%')
			{
				*pDest = 0;
				//��λ
				if(pSrc[1] >= 'A' && pSrc[1] <= 'F')
					*pDest += (pSrc[1] - 'A' + 10) * 0x10;
				else if(pSrc[1] >= 'a' && pSrc[1] <= 'f')
					*pDest += (pSrc[1] - 'a' + 10) * 0x10;
				else
					*pDest += (pSrc[1] - '0') * 0x10;

				//��λ
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
			return FALSE;
		}
		MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)pUTF8, cbDest, pUnicode, cchWideChar);
		WideCharToMultiByte(CP_ACP, 0, pUnicode, cchWideChar, pBuf, cbBufLen, NULL, NULL);
		free(pUTF8);
		free(pUnicode);
		return TRUE;
	}

	CDuiString mci::Htmlspecialchars(LPCTSTR pBuf)
	{
		CDuiString sResult;

		LPCTSTR pHeader = pBuf;
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

	BOOL mci::IsValidEmailChar(TCHAR ch)
	{
		//26��Сд��ĸ
		if( (ch>=97) && (ch<=122) ) 
			return TRUE;

		//26����д��ĸ
		if( (ch>=65) && (ch<=90) )
			return TRUE;

		//0~9
		if((ch>=48) && (ch<=57)) 
			return TRUE;

		//_-.@
		if( ch==95 || ch==45 || ch==46 || ch==64 ) 
			return TRUE;
		return FALSE;
	}

	BOOL mci::IsValidEmail(CDuiString strEmail)
	{
		//a@b.c
		if(strEmail.GetLength()<5)
			return FALSE;
		//1. ���ַ���������ĸ
		TCHAR ch = strEmail[0];
		if( ((ch>=97) && (ch<=122)) || ((ch>=65) && (ch<=90)) )
		{
			int atCount =0;
			int atPos = 0;
			int dotCount = 0;
			//0�Ѿ��жϹ��ˣ���1��ʼ
			for(int i=1;i<strEmail.GetLength();i++) 
			{
				ch = strEmail[i];
				if(IsValidEmailChar(ch))
				{
					//"@"
					if(ch==64)
					{
						atCount ++;
						atPos = i;
					}//@���ź��"."��
					else if( (atCount>0) && (ch==46) )
						dotCount ++;
				}else
					return FALSE;
			}
			//6. ��β�������ַ���@�����ߡ�.��
			if( ch == 46 ) return FALSE;
			//2. �������һ������ֻ��һ�����š�@��
			//3. @������������һ�������������š�.��
			if( (atCount!=1) || (dotCount<1) || (dotCount>3) )
				return FALSE;
			//5. ��������֡�@.������.@
			if( strEmail.Find(_T("@."))>0 || strEmail.Find(_T(".@"))>0 )
				return FALSE;
			return TRUE;
		}
		return FALSE;
	}

	CDuiString mci::GetModulePath()
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
		return CDuiString(s_buf);
	}

	void  mci::GetModulePath(CDuiString  &Path)
	{

		TCHAR s_buf[MAX_PATH];
		s_buf[0] = 0x0;
		DWORD n_result = ::GetModuleFileName(NULL, s_buf, sizeof(TCHAR) * MAX_PATH);
		TCHAR	s_drive[MAX_PATH];
		s_drive[0] = 0x0;
		TCHAR s_dir[MAX_PATH];
		s_dir[0] = 0x0;
		/*errno_t n_err_no =*/ 
		TCHAR s_filename[MAX_PATH];
		s_filename[0] = 0x0;
		TCHAR s_ext[MAX_PATH];
		s_ext[0] = 0x0;
		//	_tsplitpath(s_buf, s_drive, s_dir, NULL, NULL);
		_tsplitpath_s(s_buf, s_drive,MAX_PATH, s_dir, MAX_PATH,s_filename,MAX_PATH,s_ext,MAX_PATH);
		//assert(n_err_no == 0);
		_tcscpy_s(s_buf, s_drive);
		_tcscat_s(s_buf, s_dir);
		Path = s_buf;
	}

	std::string mci::get_module_path()
	{
		CDuiString s_result = GetModulePath();
		return TO_STDSTRING(s_result);
	}

	int mci::file_last_modified(const char* p_file_name)
	{
		if (p_file_name == NULL || strlen(p_file_name) == 0)
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
	
	int mci::file_last_modified(const wchar_t * p_file_name)
	{
		if (p_file_name == NULL || wcslen(p_file_name) == 0)
			return 0;
		//assert(sizeof(time_t) == sizeof(unsigned int));
		struct _stat32 file_info;
		if (_wstat32(p_file_name, &file_info) == 0)
		{
			return file_info.st_mtime;
		}
		else
			return 0;
	}

	bool mci::file_exist(const char * p_file_name)
	{
		if (p_file_name != NULL && strlen(p_file_name) > 0)
		{
			struct stat stat_info;
			return stat(p_file_name, &stat_info) == 0;
		}
		else
			return false;
	}

	bool mci::file_exist(const wchar_t* p_file_name)
	{
		if (p_file_name != NULL && wcslen(p_file_name) > 0)
		{
			struct _stat32 stat_info;
			return _wstat32(p_file_name, &stat_info) == 0;
		}
		else
			return false;
	}

	bool mci::is_dir_exist(const char* p_dir_name)
	{
		if (p_dir_name == NULL || strlen(p_dir_name) == 0)
			return false;
		bool b_result = false;
		WIN32_FIND_DATAA wfd;
		memset(&wfd, 0x0, sizeof(wfd));
		HANDLE hFind = FindFirstFileA(p_dir_name, &wfd);
		if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			b_result = true;
		}
		if (hFind != INVALID_HANDLE_VALUE)
			FindClose(hFind);
		return b_result;
	}

	bool mci::is_dir_exist(const wchar_t* p_dir_name)
	{
		bool b_result = false;
		/*
		if (p_dir_name != NULL && wcslen(p_dir_name) > 0)
		{
		namespace fs = boost::filesystem;
		//·���Ŀ���ֲ
		fs::wpath full_path(fs::initial_path<fs::wpath>());
		full_path = fs::system_complete(fs::wpath(p_dir_name, fs::native));
		b_result = fs::exists(full_path);
		}
		return b_result;
		*/
		WIN32_FIND_DATAW wfd;
		memset(&wfd, 0x0, sizeof(wfd));
		HANDLE hFind = FindFirstFileW(p_dir_name, &wfd);
		if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			b_result = true;   
		}
		if (hFind != INVALID_HANDLE_VALUE)
			FindClose(hFind);
		return b_result;
	}

	bool mci::delete_file(const char* p_file_name)
	{
		assert(p_file_name != NULL);
		return ::remove(p_file_name) == 0;
	}
	
	bool mci::delete_file(const wchar_t* p_file_name)
	{
		assert(p_file_name != NULL);
		return ::_wremove(p_file_name) == 0;
	}

	bool mci::delete_file_undo(const TCHAR* p_file_name, bool b_file_only)
	{
		bool b_result = false;
		int n_result = 0;
		TCHAR s_buf[MAX_PATH + 2];
		memset(s_buf, 0x0, sizeof(TCHAR) * (MAX_PATH + 2));
		if (p_file_name == NULL || _tcslen(p_file_name) > MAX_PATH)
			return false;
		_tcscpy_s(s_buf, MAX_PATH, p_file_name);
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

	bool mci::delete_dir(const TCHAR* p_dir_name)
	{
		assert(p_dir_name != NULL);
		bool b_result = false;
		unsigned int n_len = _tcslen(p_dir_name);
		TCHAR* p_buf = new TCHAR[n_len + 2];
		memset(p_buf, 0x0, sizeof(TCHAR) * (n_len + 2));
		SHFILEOPSTRUCT file_op;
		ZeroMemory((void*)&file_op, sizeof(SHFILEOPSTRUCT));
		_tcscpy_s(p_buf, n_len + 1, p_dir_name);

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

	bool mci::make_dir(const LPCTSTR p_dir_name,LPSECURITY_ATTRIBUTES lpSecurityAttributes /* = NULL */)
	{
		assert(p_dir_name);
		if(is_dir_exist(p_dir_name))
			return true;
		SECURITY_ATTRIBUTES sa;
		SECURITY_DESCRIPTOR sd;

		InitializeSecurityDescriptor(&sd,SECURITY_DESCRIPTOR_REVISION);
		SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.bInheritHandle = TRUE;
		sa.lpSecurityDescriptor = &sd;
		bool ret = CreateDirectory(p_dir_name, &sa);
		assert(ret&&p_dir_name);
		return true;
	}

	bool mci::is_directory(const char* p_file_name)
	{
		assert(p_file_name != NULL);
		struct stat stat_info;
		int n_result = stat(p_file_name, &stat_info);
		if (n_result == -1)
			return false;
		return (stat_info.st_mode & S_IFDIR) == S_IFDIR;
	}
	
	bool mci::is_directory(const wchar_t* p_file_name)
	{
		assert(p_file_name != NULL);
		struct _stat32 stat_info;
		int n_result = _wstat32(p_file_name, &stat_info);
		if (n_result == -1)
			return false;
		return (stat_info.st_mode & S_IFDIR) == S_IFDIR;
	}

	CDuiString mci::rip_file_name(const CDuiString& s_path_file, bool b_suffix)
	{
		CDuiString s_file_name;
		int n_pos = s_path_file.ReverseFind(_T('\\'));
		if (n_pos > 0)
		{
			s_file_name = s_path_file.Right(s_path_file.GetLength() - n_pos - 1);
			if (!b_suffix)
			{
				n_pos = s_file_name.ReverseFind(_T('.'));
				if (n_pos > 0)
					s_file_name = s_file_name.Left(n_pos);
			}
		}
		return s_file_name;
	}
	
	CDuiString mci::rip_file_name(const TCHAR* s_filename)
	{
		CDuiString s_file_name = _T("");
		int ilen = _tcslen(s_filename);
		const  TCHAR* p = s_filename+ilen-1;
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
			s_file_name += s_filename[i];
		}
		return s_file_name;
	}
	
	CDuiString mci::rip_file_ext(const CDuiString& s_path_file, bool b_lower)
	{
		CDuiString s_ext;
		if (!s_path_file.IsEmpty())
		{
			TCHAR s_buf[MAX_PATH];
			s_buf[0] = 0x0;
			//			_wsplitpath(s_path_file.GetString(), NULL, NULL, NULL, s_buf);
			_splitpath_s(s_path_file,NULL,NULL,NULL,NULL,NULL,NULL,s_buf,MAX_PATH);
			if (_tcslen(s_buf) > 0)
			{
				s_ext = s_buf;
				if (b_lower)
					s_ext.MakeLower();
				else
					s_ext.MakeUpper();
			}
		}
		return s_ext;
	}

	std::vector<MYSTDSTRING> mci::get_dir_stack(const MYSTDSTRING& s_file_name, bool b_reverse)
	{
		std::vector<MYSTDSTRING> list;
		MYSTDSTRING s_item;
		TCHAR* w_buf = _tcsdup(s_file_name.c_str());
		if (s_file_name.empty() || w_buf == NULL)
			return list;
		TCHAR* w_token = NULL;
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
		std::vector<MYSTDSTRING> reverse_list;
		reverse_list.reserve(list.size());
		for (unsigned int i = 0; i < list.size(); ++i)
		{
			s_item = list[list.size() - i - 1];
			reverse_list.push_back(s_item);
		}
		return reverse_list;
	}

	bool mci::is_file(const char* p_file_name)
	{
		assert(p_file_name != NULL);
		struct stat stat_info;
		int n_result = stat(p_file_name, &stat_info);
		if (n_result == -1)
			return false;
		return !((stat_info.st_mode & S_IFDIR) == S_IFDIR);

	}

	bool mci::move_file(const TCHAR* p_src_file, const TCHAR* p_dest_file)
	{
		assert(p_src_file != NULL);
		assert(p_dest_file != NULL);
		return ::MoveFile(p_src_file, p_dest_file) == TRUE;
	}

	CDuiString mci::rip_path(const CDuiString& s_file_name)
	{
		CDuiString s_path;
		int n_pos = s_file_name.ReverseFind(_T('\\'));
		if (n_pos >= 0)
			s_path = s_file_name.Left(n_pos);
		return s_path;
	}

	/*�ļ��Ƿ�ֻ��״̬*/
	bool mci::is_read_only(const char* p_file_name)
	{
		DWORD state =GetFileAttributesA(p_file_name);
		return (state &FILE_ATTRIBUTE_READONLY)?true:false;
	}

	bool mci::is_read_only(const wchar_t* p_file_name)
	{
		DWORD state =GetFileAttributesW(p_file_name);
		return (state &FILE_ATTRIBUTE_READONLY)?true:false;
	}
	/*ȥ�����ļ���ֻ��״̬*/
	bool mci::remove_read_only(const char* p_file_name)
	{
		DWORD Mask=(0xFFFFFFFF^FILE_ATTRIBUTE_READONLY);
		DWORD state =GetFileAttributesA(p_file_name);
		state =state&Mask;
		return (bool)SetFileAttributesA(p_file_name,state);	
	}
	
	bool mci::remove_read_only(const wchar_t* p_file_name)
	{
		DWORD Mask=(0xFFFFFFFF^FILE_ATTRIBUTE_READONLY);
		DWORD state =GetFileAttributesW(p_file_name);
		state =state&Mask;
		return (bool)SetFileAttributesW(p_file_name,state);	
	}
	
	bool mci::copy_file(const TCHAR* p_source_file, const TCHAR* p_dest_file, bool b_force)
	{
		return ::CopyFile(p_source_file, p_dest_file, b_force ? FALSE : TRUE) == TRUE;
	}

	bool mci::create_file(const TCHAR* p_file_name, bool b_force, unsigned int n_len)
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

	unsigned int mci::get_file_size(const char* p_file_name)
	{
		assert(p_file_name != NULL);
		struct stat stat_info;
		if (stat(p_file_name, &stat_info) == 0)
			return stat_info.st_size;
		return 0;	
	}

	unsigned int mci::get_file_size(const wchar_t* p_file_name)
	{
		assert(p_file_name != NULL);
		struct _stat32 stat_info;
		if (_wstat32(p_file_name, &stat_info) == 0)
			return stat_info.st_size;
		return 0;	
	}


	bool mci::confirm_dir(const char* p_dir_name, const char c_splitter)
	{
		assert(p_dir_name != NULL);
		std::string s_dir = p_dir_name;
		if (*s_dir.rbegin() == c_splitter)
			s_dir = s_dir.substr(0, s_dir.size() - 1);
		std::string::size_type n_pos = s_dir.find(c_splitter);
		if (n_pos == std::string::npos)
			return false;
		std::string s_confirm;
		while (true)
		{
			n_pos = s_dir.find(c_splitter, n_pos + 1);
			if (n_pos == std::string::npos)
			{
				s_confirm = s_dir;
				_mkdir(s_confirm.c_str());
				break;
			}
			s_confirm = s_dir.substr(0, n_pos);
			_mkdir(s_confirm.c_str());
			if (n_pos >= s_dir.size())
				break;
		}
		return is_dir_exist(p_dir_name);
	}
	
	bool mci::confirm_dir_2(const char* p_file_name, const char c_splitter)
	{
		assert(p_file_name != NULL);
		std::string s_file = p_file_name;
		std::string::size_type n_pos = s_file.rfind(c_splitter);
		if (n_pos == std::string::npos)
			return false;
		std::string s_dir = s_file.substr(0, n_pos);
		return confirm_dir(s_dir.c_str(), c_splitter);
	}

	bool mci::confirm_dir(const wchar_t* p_dir_name, const wchar_t c_splitter)
	{
		assert(p_dir_name != NULL);
		std::wstring s_dir = p_dir_name;
		if (*s_dir.rbegin() == c_splitter)
			s_dir = s_dir.substr(0, s_dir.size() - 1);
		std::wstring::size_type n_pos = s_dir.find(c_splitter);
		if (n_pos == std::wstring::npos)
			return false;
		std::wstring s_confirm;
		while (true)
		{
			n_pos = s_dir.find(c_splitter, n_pos + 1);
			if (n_pos == std::wstring::npos)
			{
				s_confirm = s_dir;
				_wmkdir(s_confirm.c_str());
				break;
			}
			s_confirm = s_dir.substr(0, n_pos);
			_wmkdir(s_confirm.c_str());
			if (n_pos >= s_dir.size())
				break;
		}
		return is_dir_exist(p_dir_name);
	}

	bool mci::confirm_dir_2(const wchar_t* p_file_name, const wchar_t c_splitter)
	{
		assert(p_file_name != NULL);
		std::wstring s_file = p_file_name;
		std::wstring::size_type n_pos = s_file.rfind(c_splitter);
		if (n_pos == std::wstring::npos)
			return false;
		std::wstring s_dir = s_file.substr(0, n_pos);
		return confirm_dir(s_dir.c_str(), c_splitter);
	}

	void mci::format_time_old(int n_gmt_time, const char* p_format, char* p_output)
	{
		p_output[0] = 0x0;
		assert(sizeof(time_t) == sizeof(int));
		struct tm* p_time = NULL;//localtime((const time_t*)&n_gmt_time);
		localtime_s(p_time,(const time_t*)&n_gmt_time);
		if (p_time != NULL)
		{
			assert(p_format != NULL);
			assert(strlen(p_format) <= MAX_TIME_BUF / 2);
			char s_format[MAX_TIME_BUF];
			strftime(s_format, MAX_TIME_BUF - 1, p_format, p_time);
			strcpy_s(p_output, MAX_TIME_BUF - 1, s_format);
		}
		return;
	}

	void mci::format_time_old(int n_gmt_time, const wchar_t* p_format, wchar_t* p_output)
	{
		assert(sizeof(time_t) == sizeof(int));
		struct tm* p_time = NULL;//localtime((const time_t*)&n_gmt_time);
		localtime_s(p_time,(const time_t*)&n_gmt_time);
		assert(p_format != NULL);
		assert(wcslen(p_format) <= MAX_TIME_BUF / 2);
		wchar_t s_format[MAX_TIME_BUF];
		wcsftime(s_format, MAX_TIME_BUF - 1, p_format, p_time);
		wcscpy_s(p_output, MAX_TIME_BUF - 1, s_format);
		return;
	}

	void mci::format_time_new(int n_gmt_time, const char* p_format, char* p_output)
	{
		assert(p_output != NULL);
		//const unsigned short MAX_TIME_BUF = 128;
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
	}
	
	void mci::format_time(int n_gmt_time, const char* p_format, char* p_output)
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

	void mci::format_time(int n_gmt_time, const wchar_t* p_format, wchar_t* p_output)
	{
		return format_time_old(n_gmt_time, p_format, p_output);
		/*
		assert(p_output != NULL);
		//static const unsigned short MAX_TIME_BUF = 128;
		assert(p_format != NULL);
		assert(wcslen(p_format) <= MAX_TIME_BUF / 2);
		assert(sizeof(time_t) == sizeof(int));
		struct tm gmt_time;
		errno_t n_result = gmtime_s(&gmt_time, (time_t*)&n_gmt_time);
		assert(n_result == 0);
		wchar_t s_format[MAX_TIME_BUF];
		wcsftime(s_format, MAX_TIME_BUF, p_format, &gmt_time);
		wcscpy_s(p_output, MAX_TIME_BUF, s_format);
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

	void mci::format_currenttime(char* ptime,int len)
	{
		time_t t = time(0); 
		struct tm tt;
		localtime_s(&tt,&t);
		strftime( ptime, sizeof(ptime), "[%Y-%m-%d %H:%M:%S]", &tt); 
	}

	CDuiString mci::format_currenttime()
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
	unsigned int mci::check_char(const unsigned char* p_input, unsigned int n_len, const unsigned char c_find)
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

	unsigned int mci::check_char(const  wchar_t* p_input, unsigned int n_len, const  wchar_t c_find)
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

	__int64 mci::get_disk_space(const TCHAR* p_disk_name)
	{
		__int64 i64_free_bytes_2_caller = 0;
		__int64 i64_total_bytes = 0;
		__int64 i64_free_bytes = 0;
		if (_tcscmp(p_disk_name, _T("A:")) == 0 || _tcscmp(p_disk_name, _T("B:")) == 0)
			return 0;

		UINT nError   = ::SetErrorMode(SEM_FAILCRITICALERRORS); //�������δ�������߲��ɶ� 
		//msdn The system does not display the critical-error-handler message box
		BOOL b_result = ::GetDiskFreeSpaceEx(p_disk_name,  (PULARGE_INTEGER)&i64_free_bytes_2_caller,
			(PULARGE_INTEGER)&i64_total_bytes,  (PULARGE_INTEGER)&i64_free_bytes);
		SetErrorMode(nError);
		return i64_free_bytes_2_caller;
	}

	CDuiString mci::get_max_space_disk()
	{
		DWORD n_drivers = ::GetLogicalDrives();
		static const unsigned short MAX_DISK = 26;
		CDuiString s_driver;
		__int64 i64_size = 0;
		for (unsigned short i = 0; i < MAX_DISK; ++i)
		{
			//�жϵ�ǰλ�Ƿ���������   
			if ((n_drivers & (1 << i)) != 0)   
			{
				CDuiString s_name = (TCHAR)('A' + i);
				s_name += _T(":");
				UINT n_type = ::GetDriveType(s_name);
				if (n_type == DRIVE_FIXED/* || n_type == DRIVE_REMOVABLE*/)
				{
					if (s_driver.IsEmpty())
					{
						s_driver = s_name;
						assert(i64_size == 0);
						i64_size = get_disk_space(s_name);
					}
					else
					{
						__int64 i64_disk_size = get_disk_space(s_name);
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

	CDuiString mci::get_have_dir_disk(const TCHAR* p_dir)
	{
		DWORD n_drivers = ::GetLogicalDrives();
		static const unsigned short MAX_DISK = 26;
		CDuiString s_driver;
		vector<CDuiString>	sub_dir_list;
		CDuiString s_dir = p_dir;
		s_dir.Replace(_T("\/"),_T("\\"));
		int index = 0;
		CDuiString s_sub_dir;
		for(index = 0; index <= s_dir.GetLength(); index++)
		{
			if( index == s_dir.GetLength())
			{
				if(s_sub_dir.GetLength() > 0)
				{
					sub_dir_list.push_back(s_sub_dir);
					s_sub_dir=_T("");
				}
			}
			else if(s_dir[index] == '\\')
			{
				if(s_sub_dir.GetLength() > 0)
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
			//�жϵ�ǰλ�Ƿ���������   
			if ((n_drivers & (1 << i)) != 0)   
			{
				CDuiString s_name = (TCHAR)('A' + i);
				s_name += _T(":");
				UINT n_type = ::GetDriveType(s_name);
				if (n_type == DRIVE_FIXED/* || n_type == DRIVE_REMOVABLE*/)
				{
					vector<CDuiString>	::iterator iter ;
					int n_count = 0;
					CDuiString s_path = s_name;
					int n_time = 0;
					for(iter =sub_dir_list.begin();iter !=  sub_dir_list.end();iter++ )
					{
						s_path += _T("\\");
						s_path +=(*iter) ;

						if(file_exist(s_path))
						{
							n_count ++;
							n_time = file_last_modified(s_path);
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

	std::string mci::binary_2_hex(const unsigned char* p_data, unsigned int n_len)
	{
		assert(p_data != NULL && n_len > 0);

		char* p_output = new char[n_len * 2 + 1];
		ZeroMemory(p_output, n_len * 2 + 1);
		char* p_pos = p_output;
		for (unsigned int i = 0; i < n_len; ++i)
		{
			sprintf_s(p_pos, 3, "%02x", p_data[i]);
			p_pos += 2;
		}
		std::string s_result = p_output;
		delete[] p_output;
		return s_result;
	}

	CDuiString mci::_binary_2_hex(const unsigned char* p_data, unsigned int n_len)
	{
		assert(p_data != NULL && n_len > 0);
		TCHAR* p_output = new TCHAR[n_len * 2 + 1];
		ZeroMemory(p_output, sizeof(TCHAR) * (n_len * 2 + 1));
		TCHAR* p_pos = p_output;
		for (unsigned int i = 0; i < n_len; ++i)
		{
			_stprintf_s(p_pos, 3, _T("%02x"), p_data[i]);
			p_pos += 2;
		}
		CDuiString s_result = p_output;
		delete[] p_output;
		return s_result;
	}

	void mci::confirm_splash(CDuiString& s_input, bool b_tail/*=true*/)
	{
		int n_size = s_input.GetLength();
		if (n_size == 0)
		{
			s_input += _T('\\');
		}
		else
		{
			if (b_tail)		//ȷ��β����
			{
				if (n_size > 0 && s_input[n_size - 1] != _T('\\') && s_input[n_size - 1] != _T('/'))
				{
					s_input += _T('\\');
				}
			}
			else			//ȷ��ͷ����
			{
				if (n_size > 0 && s_input[0] != _T('\\') && s_input[0] != _T('/'))
				{
					s_input = _T('\\') + s_input;
				}
			}
		}
		return;
	}

	void mci::confirm_n_splash(CDuiString& s_input, bool b_tail/*=true*/)
	{
		int n_size = s_input.GetLength();
		if (b_tail)				//ȷ��β��û��
		{
			if (n_size > 0 && (s_input[n_size - 1] == _T('\\') || s_input[n_size - 1] == _T('/')))
			{
				s_input = s_input.Left(n_size - 1);
			}
		}
		else						//ȷ��ͷ��û��
		{
			if (n_size > 0 && (s_input[0] == _T('\\') || s_input[0] == _T('/')))	
			{
				s_input = s_input.Right(n_size - 1);
			}
		}
		return;
	}

	CDuiString mci::link_dir_splash(const CDuiString& s_base, const CDuiString& s_sub)
	{
		CDuiString s_result = s_base;
		confirm_splash(s_result);
		CDuiString s_link = s_sub;
		confirm_n_splash(s_link, false);
		s_result += s_link;
		return s_result;
	}

	CDuiString mci::fs_replace(const CDuiString& s_input, const TCHAR c_new)
	{
		const TCHAR REPLACE_CHARS[] = _T("\/\\\:\*\"\?\>\<\|");

		CDuiString s_output = s_input;
		unsigned int n_count = sizeof(REPLACE_CHARS) / sizeof(REPLACE_CHARS[0]);
		for (unsigned int i = 0; i < n_count; ++i)
		{
			const TCHAR c_old = REPLACE_CHARS[i];
			s_output.Replace(CDuiString(c_old), CDuiString(c_new));
		}
		int n_len = s_output.GetLength();

		//�Էǿ���ASCII�ַ��Ĵ���
		static const unsigned short MIN_ASCII_VALID = 30;
		for (int i = 0; i < n_len; ++i)
		{
			TCHAR s_value = s_output[i];
			unsigned short n_value = s_value;
			if (n_value < MIN_ASCII_VALID)
			{
				s_output.SetAt(i, c_new);
			}
		}

		if (n_len > 0)														//�Ե�ŵ����⴦��
		{
			if (s_output[0] == _T('.'))									//���׵��
			{
				s_output.SetAt(0, c_new);
			}
			if (s_output[n_len - 1] == _T('.'))						//��β���
			{		
				s_output.SetAt(n_len - 1, c_new);
			}
		}
		return s_output;
	}

	void mci::shift_html_item(MYSTDSTRING& s_data)
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

	void mci::erase_html_item(MYSTDSTRING& s_data)
	{
		static const TCHAR LEFT_ARROW = _T('<');
		static const TCHAR RIGHT_ARROW = _T('>');
		//std::wstring s_src = s_data;
		MYSTDSTRING::size_type n_pos = 0;
		int n_count = 0;
		while (true)
		{
			n_count = 0;
			n_pos = s_data.find(LEFT_ARROW, 0);		//���ҵ�һ��<
			if (n_pos == MYSTDSTRING::npos)
				break;
			MYSTDSTRING::size_type n_left_pos = n_pos;
			++n_count;
			++n_pos;
			while (n_pos < s_data.size())
			{
				if (s_data[n_pos] == LEFT_ARROW)
					++n_count;
				else if (s_data[n_pos] == RIGHT_ARROW)
					--n_count;
				if (n_count == 0)		//һ�ν���
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

	//////////////////////////////////////////////////////////
	CDuiString mci::ToString(char* cValue)
	{
		//����char *�����С�����ֽ�Ϊ��λ��һ������ռ�����ֽ�
		// 		int charLen = strlen(cValue);
		// 		//������ֽ��ַ��Ĵ�С�����ַ����㡣
		// 		int len = MultiByteToWideChar(CP_ACP,0,cValue,charLen,NULL,0);
		// 		//Ϊ���ֽ��ַ���������ռ䣬�����СΪ���ֽڼ���Ķ��ֽ��ַ���С
		// 		wchar_t *buf = new wchar_t[len + 1];
		// 		//���ֽڱ���ת���ɿ��ֽڱ���
		// 		MultiByteToWideChar(CP_ACP,0,cValue,charLen,buf,len);
		// 		buf[len] = '\0'; //����ַ�����β��ע�ⲻ��len+1
		// 		//��TCHAR����ת��ΪCString
		// 		CString pWideChar;
		// 		pWideChar.Append(buf);
		// 		//ɾ��������
		// 		delete []buf;
		// 		return pWideChar;
		//USES_CONVERSION;
		//return A2T(cValue);

		return TO_DUISTRING(cValue);
	}

	CDuiString mci::ToString(const char* cValue)
	{
		return TO_DUISTRING(cValue);
	}
	
	CDuiString mci::ToString(BYTE btValue)
	{	  
		CDuiString strValue;
		strValue.Format(_T("%d"), btValue);  
		return strValue;  
	}  
	
	CDuiString mci::ToString(int iValue)
	{  
		CDuiString strValue;
		strValue.Format(_T("%d"), iValue);  
		return strValue;  
	}  

	CDuiString mci::ToString(unsigned int iValue)
	{  
		CDuiString strValue;
		strValue.Format(_T("%u"), iValue);  
		return strValue;  
	}  
	
	CDuiString mci::ToString(long lValue)
	{  
		CDuiString strValue;
		strValue.Format(_T("%d"), lValue);  
		return strValue;  
	}  
	
	CDuiString mci::ToString(unsigned long lValue)
	{  
		CDuiString strValue;
		strValue.Format(_T("%u"), lValue);  
		return strValue;  
	}  

	CDuiString mci::ToString(__int64 i64Value)
	{  
		CDuiString strValue;
		strValue.Format(_T("%I64d"), i64Value);  
		return strValue;  
	}  
	
	CDuiString mci::ToString(unsigned __int64 i64Value)
	{  
		CDuiString strValue;
		strValue.Format(_T("%I64u"), i64Value);  
		return strValue;  
	}  

	CDuiString mci::ToString(float fltValue)
	{  
		CDuiString strValue;
		strValue.Format(_T("%f"), fltValue);  
		return strValue;  
	}  

	CDuiString mci::ToString(double dblValue)
	{  
		CDuiString strValue;
		strValue.Format(_T("%f"), dblValue);  
		return strValue;  
	}  
	// ʱ�������ʽ��2010-05-06 22:07:08   
	
	CDuiString mci::ToString(const COleDateTime& time,bool bdatetime/*=true*/)
	{  
		CDuiString strValue;
		if(bdatetime)
			strValue = time.Format(_T("%Y-%m-%d %H:%M:%S")); 
		else
			strValue = time.Format(_T("%Y-%m-%d")); 
		return strValue;  
	}  
	
	/*
	CString mci::ToString(const _variant_t& var)  
	{  
	CString strValue("");  
	switch (var.vt)  
	{  
	case VT_BSTR: //�ַ���   
	case VT_LPSTR:  
	case VT_LPWSTR:  
	strValue = (LPCTSTR)(_bstr_t)var;  
	break;  
	case VT_I1:   //�޷����ַ�   
	case VT_UI1:  
	strValue.Format(_T("%d"), var.bVal);  
	break;  
	case VT_I2:   //������   
	strValue.Format(_T("%d"), var.iVal);  
	break;  
	case VT_UI2:   //�޷��Ŷ�����   
	strValue.Format(_T("%u"), var.uiVal);  
	break;  
	case VT_INT: //����   
	strValue.Format(_T("%d"), var.intVal);  
	break;  
	case VT_I4:   //����   
	case VT_I8:   //������   
	strValue.Format(_T("%d"), var.lVal);  
	break;  
	case VT_UINT:   //�޷�������   
	strValue.Format(_T("%d"), var.uintVal);  
	break;  
	case VT_UI4:    //�޷�������   
	case VT_UI8:    //�޷��ų�����   
	strValue.Format(_T("%d"), var.ulVal);  
	break;  
	case VT_VOID:  
	strValue.Format(_T("%08x"), var.byref);  
	break;  
	case VT_R4:   //������   
	strValue.Format(_T("%f"), var.fltVal);  
	break;  
	case VT_R8:   //˫������   
	strValue.Format(_T("%f"), var.dblVal);  
	break;  
	case VT_DECIMAL: //С��   
	strValue.Format(_T("%f"), (double)var);  
	break;  
	case VT_CY:  
	{  
	CY cy = var.cyVal;  
	strValue = cy.Format();  
	}  
	break;  
	case VT_BLOB:  
	case VT_BLOB_OBJECT:  
	case 0x2011:  
	strValue = _T("[BLOB]");  
	break;  
	case VT_BOOL:   //������     
	strValue = var.boolVal ? _T("TRUE") : _T("FALSE");  	        break;  
	case VT_DATE: //������   
	{  
	DATE dt = var.date;  
	COleDateTime da = COleDateTime(dt);  
	strValue = da.Format(_T("%Y-%m-%d %H:%M:%S"));  
	}
	break;
	case VT_EMPTY:   //��   
	strValue = _T("");  
	break;  
	case VT_UNKNOWN:   //δ֪����   	    default:  
	strValue = _T("VT_UNKNOW");  
	break;  
	}
	return strValue;  
	}*/
	/*******************************************/
	std::string mci::ToCharArr(LPCTSTR lpszValue)
	{
		//CString Value(lpszValue);
		// 	//ע�⣺����n��len��ֵ��С��ͬ,n�ǰ��ַ�����ģ�len�ǰ��ֽڼ����
		// 	int n = Value.GetLength();
		// 	//��ȡ���ֽ��ַ��Ĵ�С����С�ǰ��ֽڼ����
		// 	int len = WideCharToMultiByte(CP_ACP,0,Value,Value.GetLength(),NULL,0,NULL,NULL);
		// 	//Ϊ���ֽ��ַ���������ռ䣬�����СΪ���ֽڼ���Ŀ��ֽ��ֽڴ�С
		// 	char * pBuff = new char[len+1];   //���ֽ�Ϊ��λ
		// 	//���ֽڱ���ת���ɶ��ֽڱ���
		// 	WideCharToMultiByte(CP_ACP,0,Value,Value.GetLength(),pBuff,len,NULL,NULL);
		// 	pBuff[len+1] = '\0';   //���ֽ��ַ���'\0'����
		// 	char* pRet = NULL;
		// //	strcpy(pRet,pBuff);
		// 	memcpy(pRet,pBuff,len);
		// 	delete pBuff;

		/*setlocale(LC_ALL,"chs");
		char* p = new char[MAX_PATH];
		size_t t;
		wcstombs_s(&t,p,MAX_PATH,Value,MAX_PATH);
		char* pRet = NULL;
		memcpy(pRet,p,t);
		delete p;
		return std::string(pRet);
		*///	return pRet;

		return TO_STDSTRING(lpszValue);
	}
	
	BYTE mci::ToByte(LPCTSTR lpszValue)  
	{  
		BYTE btValue;  
		btValue = (BYTE)_ttoi(lpszValue);  
		return btValue;  
	}  
	
	int mci::ToInt(LPCTSTR lpszValue)  
	{  
		int iValue;  
		iValue = _ttoi(lpszValue);  
		return iValue;  
	}  
	
	unsigned int mci::ToUInt(LPCTSTR lpszValue)  
	{  
		unsigned int iValue;  
		iValue = _ttoi(lpszValue);  
		return iValue;  
	}  
	
	long mci::ToLong(LPCTSTR lpszValue)  
	{  
		long lValue;  
		lValue = _ttol(lpszValue);  
		return lValue;  
	}  
	
	unsigned long mci::ToULong(LPCTSTR lpszValue)  
	{  
		unsigned long lValue;  
		lValue = _ttol(lpszValue);  
		return lValue;  
	}  
	
	__int64 mci::ToInt64(LPCTSTR lpszValue)  
	{  
		__int64 i64Value;  
		i64Value = _ttoi64(lpszValue);  
		return i64Value;  
	}  
	
	unsigned __int64 mci::ToUInt64(LPCTSTR lpszValue)  
	{  
		unsigned __int64 i64Value;  
		i64Value = _ttoi64(lpszValue);  
		return i64Value;  
	}  
	
	float mci::ToFloat(LPCTSTR lpszValue)  
	{  
		float fltValue;  
#ifdef _UNICODE   
		fltValue = (float)_wtof(lpszValue);  
#else   
		fltValue = (float)atof(lpszValue);  
#endif   
		return fltValue;  
	}  
	
	double mci::ToDouble(LPCTSTR lpszValue)  
	{  
		double dblValue;  
#ifdef _UNICODE   
		dblValue = _wtof(lpszValue);  
#else   
		dblValue = atof(lpszValue);  
#endif   
		return dblValue;  
	}  
	// ʱ���ʽ���ӣ�2010-05-06 22:07:08   
	//               2010 05 06 22 07 08   
	//               2010:05:06 22:07:08   
	//               2010-05-06-22-07-08   
	// ֻҪ��"- :"�ָ��ʱ���ʽ������   
	
	COleDateTime mci::ToTime(LPCTSTR lpszValue)  
	{  
		unsigned int iArray[6] = {2010, 5, 11, 12, 00, 00};  
		int nIndex = 0;  
#if _MSC_VER >= 1310  //VC6.0��֧��CStringAlt::Tokenize   
		CDuiString strTmp;  
		int curPos = 0;  
		CDuiString strValue(lpszValue);

		/*strTmp = strValue.Tokenize(_T("- :"),curPos);  
		while (strTmp != _T("") && nIndex <6)  
		{  
			iArray[nIndex++] = ToUInt(strTmp);  
			strTmp = strValue.Tokenize(_T("- :"), curPos);  
		};  */
#else   
		TCHAR tChar[MAX_PATH] = {0};  
		TCHAR tCharTmp[MAX_PATH] = {0};  
		TCHAR seps[]   = "- :";  
		TCHAR *next_token = NULL;  
		ASSERT(_tcslen(lpszValue) < MAX_PATH);  
		RtlCopyMemory(tChar, lpszValue, _tcslen(lpszValue) * sizeof(TCHAR));  
		next_token = _tcstok(tChar, seps);  
		while ( next_token != NULL && nIndex <6)  
		{  
			iArray[nIndex++] = _ttoi(next_token);  
			next_token = _tcstok( NULL, seps);  
		}  

#endif   
		COleDateTime time(iArray[0], iArray[1], iArray[2],  
			iArray[3], iArray[4], iArray[5]);  
		return time;  
	}  
	
	_variant_t mci::ToVariant(const COleDateTime& time)  
	{  
		_variant_t vtVal;        
		vtVal.vt = VT_DATE;  
		vtVal.date = time; 

		return vtVal;  
	}  
};
