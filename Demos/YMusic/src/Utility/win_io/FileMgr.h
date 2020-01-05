#pragma once


#include <set>
#include <vector>
#include <string>
//文件管理
class CFileMgr
{

	CFileMgr(void);
public:
	typedef TCHAR                char_type;
#ifdef _UNICODE 
	typedef std::wstring         string_type;
#else
	typedef std::string          string_type;
#endif
	static CFileMgr* Instance()
	{
		static CFileMgr mgr;
		return &mgr;
	}
	virtual ~CFileMgr(void);
	typedef std::vector<string_type>    filesArrayT;
	typedef filesArrayT::iterator       filesArrayIterT;
	typedef std::set<string_type>       fileExtArrayT;
	typedef fileExtArrayT::iterator     fileExtArrayIterT;
public:
	string_type GetModuleFilePath();
	bool file_exist(const char_type* s_file);
	string_type rip_file_name(const string_type& s_path_file);
	int OpenFile(HWND hWnd,filesArrayT& importfiles);
	int OpenFolder(HWND hWnd,filesArrayT& improtfiles);
	int FindImportFiles(const string_type& strPath,filesArrayT &importfiles);
	bool ReadFileAllBytes(const string_type& strPath, std::vector<BYTE>& bytes);
protected:
	fileExtArrayT m_FileExt;
};

