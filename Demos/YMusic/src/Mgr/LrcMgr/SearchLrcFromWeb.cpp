#include "stdafx.h"
#include "SearchLrcFromWeb.h"
#include "ParseTrackFile.h"
#include "YMusic.h"
#include "YApp.h"
#include <WinInet.h>

CSearchLrcFromWeb::CSearchLrcFromWeb(void)
{
	m_strSearchPathLibyyu = _T("http://www.libyyu.com/lrcsearch?key=%s");
	m_strDownloadPathLibyyu = _T("http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?dl?Id=%d&Code=%s");
}


CSearchLrcFromWeb::~CSearchLrcFromWeb(void)
{

}

SongInfoVT CSearchLrcFromWeb::SearchLrc(spSongInfoT pSong)
{
	m_SongInfoV.clear();
	m_SongInfoV = SearchLrcFromLibyyu(pSong);
	return m_SongInfoV;
}

SongInfoVT CSearchLrcFromWeb::SearchLrc(const DuiLib::CDuiString& sSongName, const DuiLib::CDuiString& sArtistName)
{
	m_SongInfoV.clear();
	if (sSongName.IsEmpty() || sArtistName.IsEmpty())
		return m_SongInfoV;
	m_SongInfoV = SearchLrcFromLibyyu(sSongName,sArtistName);
	return m_SongInfoV;
}

DuiLib::CDuiString EncodeDuiString(const DuiLib::CDuiString& s_input)
{
	std::string skey = DuiLib::Convert::ToUTF8(s_input.GetData());
	skey = em_utility::url_encoder::URLEncode(skey);
	std::wstring wskey = DuiLib::Convert::UTF8ToUnicode(skey.c_str()).c_str();

	DuiLib::CDuiString s_out;
	s_out += wskey.c_str();
	return s_out;
}

SongInfoVT CSearchLrcFromWeb::SearchLrcFromLibyyu(spSongInfoT pSong)
{
	
	DuiLib::CDuiString strArtist = pSong->GetArtistName();
	DuiLib::CDuiString strTitle  = pSong->GetSongName();

	return SearchLrcFromLibyyu(strTitle, strArtist);
}

SongInfoVT CSearchLrcFromWeb::SearchLrcFromLibyyu(const DuiLib::CDuiString& sSongName, const DuiLib::CDuiString& sArtistName)
{
	DuiLib::CDuiString strArtist = sArtistName;
	DuiLib::CDuiString strTitle  = sSongName;

	strArtist.MakeLower();
	strArtist.Replace(_T(" "), _T(""));
	strArtist.Replace(_T("'"), _T(""));
	strTitle.MakeLower();
	strTitle.Replace(_T(" "), _T(""));
	strTitle.Replace(_T("'"), _T(""));
	if (strTitle == _T(""))
		return m_SongInfoV;

	DuiLib::CDuiString str_key;
	DuiLib::CDuiString str_artist;
	DuiLib::CDuiString str_title;

	if (strArtist != _T(""))
		str_artist = EncodeDuiString(strArtist);
	if (strTitle != _T(""))
		str_title = EncodeDuiString(strTitle);

	str_artist.MakeUpper();
	str_title.MakeUpper();
	str_key = str_title;
	if (!str_artist.IsEmpty())
		str_key = str_key + _T("%20") + str_artist;

	DuiLib::CDuiString str_search_path;
	str_search_path.Format(m_strSearchPathLibyyu.GetData(), str_key.GetData());
	DuiLib::CDuiString strLocalFile = DuiLib::Path::CombinePath(theFilePathMgr->GetDefaultCachePath(), _T("tracks.json"));
	
	bool bRet = DownloadLrcTrack(str_search_path, strLocalFile);
	//	assert(bRet);
	if (!bRet) return m_SongInfoV;

	fileinfo_container lrcTracks;
	CParseJsonFile::Parse(lrcTracks, strLocalFile);
	if (lrcTracks.size() <= 0)
		return m_SongInfoV;

	DuiLib::CDuiString strCode;
	DuiLib::CDuiString str_download_path;

	for (size_t i = 0; i < lrcTracks.size(); i++)
	{
		SONGINFO lrc_info;
		//lrc_info.strLrcUrl = str_download_path;
		lrc_info.strTitle  = lrcTracks[i].strTitle;
		lrc_info.strArtist = lrcTracks[i].strArtist;
		lrc_info.strKey = lrcTracks[i].strKey;
		m_SongInfoV.push_back(lrc_info);
	}
	return m_SongInfoV;
}

bool CSearchLrcFromWeb::DownloadContent(const DuiLib::CDuiString& strUrl, std::vector<BYTE>& buffer)
{
#define  BUFFER_LEN 4096 // 4K

	try {
		HINTERNET hInternetSession = NULL, hOpenUrl = NULL;
		hInternetSession = InternetOpen(
			_T("Microsoft@Internet Explorer"),
			INTERNET_OPEN_TYPE_PRECONFIG,
			NULL,
			NULL,
			0);
		assert(hInternetSession);
		if (!hInternetSession) return false;

		hOpenUrl = ::InternetOpenUrl(hInternetSession, strUrl.GetData(), NULL, 0, INTERNET_FLAG_RAW_DATA, 0);
		//		assert(hOpenUrl);
		if (!hOpenUrl)
		{
			InternetCloseHandle(hInternetSession);
			return false;
		}
		buffer.clear();
		ULONG nNumber = 1;
		while (nNumber > 0)
		{
			BYTE Buffer[BUFFER_LEN] = { 0 };
			InternetReadFile(hOpenUrl, (LPVOID)Buffer, BUFFER_LEN, &nNumber);
			size_t pos = buffer.size();
			if (nNumber)
			{
				buffer.resize(pos + nNumber);
				CopyMemory(&buffer[pos], Buffer, nNumber);
			}
		}
		InternetCloseHandle(hOpenUrl);
		InternetCloseHandle(hInternetSession);

		return true;
	}
	catch (...)
	{
		throw L"DownloadContent";
		return false;
	}
#undef BUFFER_LEN
}

int IsLittleEndian()
{
	int x = 1;
	if (*(char*)& x == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void HtmEscapeEntityCodeToUnicode(const char* entity_code, char* html_char)
{
	int decimal_value = atoi(entity_code);
	char* uchari = (char*)& decimal_value;
	if (IsLittleEndian())
	{
		html_char[0] = uchari[0] & 0xFF;
		html_char[1] = uchari[1] & 0xFF;
	}
	else
	{
		html_char[0] = uchari[1] & 0xFF;
		html_char[1] = uchari[0] & 0xFF;
	}
}

void OneUnicode2UTF8(const char* unicode_char, size_t unicode_char_length, char* utf_char)
{
	//unicode: 0x192->110010010 ,utf8:0xC692->1100011010010010
	int value = 0;
	memcpy(&value, unicode_char, unicode_char_length);

	if (value >= 0x0000 && value <= 0x007F)
	{
		utf_char[0] = unicode_char[0];
	}
	else if (value >= 0x0080 && value <= 0x07FF)
	{
		utf_char[0] = ((value >> 6) | 0xC0);
		utf_char[1] = ((value & 0x3F) | 0x80);
	}
	else if (value >= 0x0800 && value <= 0xFFFF)
	{
		utf_char[0] = ((value >> 12) | 0xE0);
		utf_char[1] = ((value >> 6 & 0x3F) | 0x80);
		utf_char[2] = ((value & 0x3F) | 0x80);
	}
	else if (value >= 0x10000 && value <= 0x10FFFF)
	{
		utf_char[0] = (value >> 18 | 0xF0);
		utf_char[1] = ((value >> 12 & 0x3F) | 0x80);
		utf_char[2] = ((value >> 6 & 0x3F) | 0x80);
		utf_char[3] = ((value & 0x3F) | 0x80);
	}
	else
	{
		assert(false && "value too big.");
	}
}

static const char* kEntityNameToEntityCodeMap[] =
{
	"oelig","339","amp","38","rArr","8658","fnof","402"
};
//1.这里可以优化的余地很大.
const char* HtmEscapeEntityNameToEntityCode(const char* entity_name)
{
	static size_t length = sizeof(kEntityNameToEntityCodeMap) / sizeof(char*);
	for (size_t i = 0; i < length; i += 2)
	{
		if (!strcmp(entity_name, kEntityNameToEntityCodeMap[i]))
		{
			return kEntityNameToEntityCodeMap[i + 1];
		}
	}
	return NULL;
}

std::string UnescapeUTFHTMLContent(const char* str)
{
	std::string temp;

	char* pos_amp = NULL;
	char* pos_semicolon = (char*)str;
	const char* start_amp = str;

	int entity_length = 0;
	char entity_code[5];
	const int kMaxEntityLength = 4;

	char entity_name[20];
	const int kMaxEntityNameLength = 18;

	char unicode[3];
	char utf8[4];

	while (true)
	{
		if (!start_amp || !(*start_amp))
		{
			break;
		}
		pos_amp = (char*)strchr(start_amp, '&');
		if (!pos_amp)
		{
			temp.append(start_amp);
			break;
		}
		int pos_no = pos_amp - pos_semicolon;
		if (pos_no > 0)
		{
			temp.append(start_amp, pos_no);
			start_amp = pos_amp;
		}

		char* pos_amp1 = pos_amp + 1;
		if (!pos_amp1 || !(*pos_amp1))
		{
			std::string t2(start_amp);
			temp.append(start_amp);
			break;
		}

		if (isalpha(*pos_amp1))
		{
			pos_semicolon = strchr(pos_amp1, ';');
			if (pos_semicolon)
			{
				//调用 HtmEscapeEntityNameToEntityCode
				memset(entity_name, 0, sizeof(entity_name));
				entity_length = ((pos_semicolon - pos_amp1) >
					kMaxEntityNameLength) ? kMaxEntityNameLength :
					(pos_semicolon - pos_amp1);
				strncpy(entity_name, pos_amp1, entity_length);
				const char* entity_code_c =
					HtmEscapeEntityNameToEntityCode(entity_name);
				if (entity_code_c)
				{
					memset(unicode, 0, sizeof(unicode));
					memset(utf8, 0, sizeof(utf8));
					HtmEscapeEntityCodeToUnicode(entity_code_c, unicode);
					OneUnicode2UTF8(unicode, 2, utf8);
					temp.append(utf8);
				}
				else
				{
					temp.append(entity_name);
				}
				//1.entity_name转换为entity_code之后再转换为utf8字符.
				start_amp = pos_semicolon + 1;
				pos_semicolon += 1;
			}
			else
			{
				start_amp = pos_amp1;
			}
		}
		else if (*pos_amp1 == '#')
		{
			char* pos_digit = (pos_amp1 + 1);
			if (!pos_digit)
			{
				break;
			}

			if (isdigit(*pos_digit))
			{
				//1.需要判断数值小于10000.
				pos_semicolon = (char*)strchr(start_amp, ';');
				if (pos_semicolon)
				{
					memset(entity_code, 0, sizeof(entity_code));
					entity_length = ((pos_semicolon - pos_digit) >
						kMaxEntityLength) ? kMaxEntityLength :
						(pos_semicolon - pos_digit);
					strncpy(entity_code, pos_digit, entity_length);
					memset(unicode, 0, sizeof(unicode));
					memset(utf8, 0, sizeof(utf8));
					HtmEscapeEntityCodeToUnicode(entity_code, unicode);
					OneUnicode2UTF8(unicode, 2, utf8);
					temp.append(utf8);

					start_amp = pos_semicolon + 1;
					pos_semicolon += 1;
				}
				else
				{
					start_amp = pos_digit;
				}
			}
		}
		else
		{
			std::string sa(start_amp, pos_amp1 - start_amp);
			temp.append(sa);
			start_amp = pos_amp1;
		}
	}
	return temp;
}

bool CSearchLrcFromWeb::DownloadLrcFile(const DuiLib::CDuiString& strUrl, const DuiLib::CDuiString& strLocalFilePath)
{
	FILE* stream = NULL;
	_tfopen_s(&stream, strLocalFilePath.GetData(), _T("wb"));
	assert(stream);
	if (!stream)
	{
		return false;
	}
	std::vector<BYTE> buffer;
	if (DownloadContent(strUrl, buffer))
	{
		DuiLib::CDuiString Content;
		DWORD dwSize = buffer.size();
		LPTSTR pstrContent = NULL;
#ifdef _UNICODE
		{
			if (dwSize >= 3 && buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF)
			{
				buffer.erase(buffer.begin(), buffer.begin() + 3);
				dwSize -= 3;
			}
			DWORD nWide = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)& buffer[0], dwSize, NULL, 0);
		
			pstrContent = static_cast<LPTSTR>(malloc((nWide + 1) * sizeof(TCHAR)));
			::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)& buffer[0], dwSize, pstrContent, nWide);
			pstrContent[nWide] = _T('\0');
		}
#else // !_UNICODE
		{
			if (dwSize >= 3 && buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF)
			{
				buffer.erase(buffer.begin(), buffer.begin() + 3);
				dwSize -= 3;
			}
			DWORD nWide = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)& buffer[0], dwSize, NULL, 0);
		
			LPWSTR w_str = static_cast<LPWSTR>(malloc((nWide + 1) * sizeof(WCHAR)));
			::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)& buffer[0], dwSize, w_str, nWide);
			w_str[nWide] = L'\0';
		
			DWORD wide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, NULL, 0, NULL, NULL);
		
			pstrContent = static_cast<LPTSTR>(malloc((wide + 1) * sizeof(TCHAR)));
			::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, pstrContent, wide, NULL, NULL);
			pstrContent[wide] = _T('\0');
			free(w_str);
		}
#endif // _UNICODE
		
		Content = pstrContent;
		Content.Replace(_T("&#09;"), _T("\t"));
		Content.Replace(_T("&#10;"), _T("\n"));
		Content.Replace(_T("&#32;"), _T(" "));
		Content.Replace(_T("&#33;"), _T("!"));
		Content.Replace(_T("&#34;"), _T("\""));
		Content.Replace(_T("&#36;"), _T("$"));
		Content.Replace(_T("&#37;"), _T("%"));
		Content.Replace(_T("&#38;"), _T("&"));
		Content.Replace(_T("&#39;"), _T("'"));
		Content.Replace(_T("&#40;"), _T("("));
		Content.Replace(_T("&#41;"), _T(")"));
		Content.Replace(_T("&#42;"), _T("*"));
		Content.Replace(_T("&#43;"), _T("+"));
		Content.Replace(_T("&#44;"), _T(","));
		Content.Replace(_T("&#45;"), _T("-"));
		Content.Replace(_T("&#46;"), _T("."));
		Content.Replace(_T("&#47;"), _T("/"));
		DuiLib::CDuiString tmp;
		DuiLib::CDuiString tmp2;
		for (int i = 0; i < 9; ++i)
		{
			tmp.Format(_T("&#%d;"), 48 + i);
			TCHAR ch = '0' + i;
			tmp2 = ch;
			Content.Replace(tmp, tmp2);
		}

		Content.Replace(_T("&#58;"), _T(":"));
		Content.Replace(_T("&#59;"), _T(";"));
		Content.Replace(_T("&#60;"), _T("<"));
		Content.Replace(_T("&#61;"), _T("="));
		Content.Replace(_T("&#62;"), _T(">"));
		Content.Replace(_T("&#63;"), _T("?"));
		Content.Replace(_T("&#64;"), _T("@"));	
		for (int i = 65; i <= 90; ++i)
		{
			tmp.Format(_T("&#%d;"), i);
			TCHAR ch = 'A' + (i - 65);
			tmp2 = ch;
			Content.Replace(tmp, tmp2);
		}
		Content.Replace(_T("&#91;"), _T("["));
		Content.Replace(_T("&#92;"), _T("\\"));
		Content.Replace(_T("&#93;"), _T("]"));
		Content.Replace(_T("&#94;"), _T("^"));
		Content.Replace(_T("&#95;"), _T("_"));
		Content.Replace(_T("&#96;"), _T("`"));
		for (int i = 97; i <= 122; ++i)
		{
			tmp.Format(_T("&#%d;"), i);
			TCHAR ch = 'a' + (i - 97);
			tmp2 = ch;
			Content.Replace(tmp, tmp2);
		}
		Content.Replace(_T("&#123;"), _T("{"));
		Content.Replace(_T("&#124;"), _T("|"));
		Content.Replace(_T("&#125;"), _T("}"));
		Content.Replace(_T("&#126;"), _T("~"));
		Content.Replace(_T("&#160;"), _T(" "));

		//const unsigned char* pstr = &buffer[0];
		//std::string ss = UnescapeUTFHTMLContent((const char*)pstr);
		std::string ss = DuiLib::Convert::ToUTF8(Content);
		fwrite(ss.c_str(), sizeof(char), ss.size(), stream);


		fclose(stream);
		return true;
	}

	return false;
}

bool CSearchLrcFromWeb::DownloadLrcTrack(const DuiLib::CDuiString& strUrl, const DuiLib::CDuiString& strLocalFilePath)
{
	FILE* stream = NULL;
	_tfopen_s(&stream, strLocalFilePath.GetData(), _T("wb"));
	assert(stream);
	if (!stream)
	{
		return false;
	}
	std::vector<BYTE> buffer;
	if (DownloadContent(strUrl, buffer))
	{
		const unsigned char* pstr = &buffer[0];
		fwrite(pstr, sizeof(unsigned char), buffer.size(), stream);
		fclose(stream);
		return true;
	}

	return false;
}
//http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?dl?Id=448265&Code=-974517898

// UINT DownloadThread(void *pArg)
// {
// 	return 0;
// }
// bool CSearchLrcFromWeb::DownLoad(const char* url)
// {
// 	
//	return 0;
//}