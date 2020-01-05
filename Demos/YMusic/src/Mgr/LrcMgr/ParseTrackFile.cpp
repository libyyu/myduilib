#include "StdAfx.h"
#include "ParseTrackFile.h"
#include "Utility/Convert.h"
#include "Utility/tinyxml/tinyxml.h"
#include "Utility/json/DataParser.h"
#include "YMusic.h"
#include "YApp.h"
#include "Utility/win_io/FileMgr.h"

CParseXmlFile::CParseXmlFile(void)
{

}

CParseXmlFile::~CParseXmlFile(void)
{

}

BOOL CParseXmlFile::Parse(fileinfo_container& filesInfo,const DuiLib::CDuiString& strXmlFileName)
{
	if (strXmlFileName.IsEmpty())
		return FALSE;
	
	if (em_utility::mci::file_exist(strXmlFileName))
	{
		em_utility::TiXmlDocument doc;
		std::string ss = TransToStdString(strXmlFileName);
		if (doc.LoadFile(ss))
		{
			em_utility::TiXmlElement* root = doc.FirstChildElement("result");
			if (NULL!=root)
			{
				em_utility::TiXmlElement* pComponet = root->FirstChildElement("lrc");
				while (NULL!=pComponet)
				{
					std::string s_id = pComponet->Attribute((const char*)"id");
					std::string s_artist = pComponet->Attribute((const char*)"artist");
					std::string s_title = pComponet->Attribute((const char*)"title");
					DuiLib::CDuiString sId = TransToDuiString(s_id.c_str());
					DuiLib::CDuiString sArtist = TransToDuiString(s_artist.c_str());
					DuiLib::CDuiString sTitle = TransToDuiString(s_title.c_str());
					stLrcInfo sLrc = {_ttol(s_id.c_str()),sArtist,sTitle, _T("")};
					filesInfo.push_back(sLrc);

					pComponet = pComponet->NextSiblingElement("lrc");
				}
			}
		}
	}
	return filesInfo.size() > 0;
}

BOOL CParseXmlFile::ParseBaiduLrcList(fileinfo_container& filesInfo,const DuiLib::CDuiString& strXmlFileName)
{
	if (strXmlFileName.IsEmpty())
		return FALSE;

	if (em_utility::mci::file_exist(strXmlFileName))
	{
		em_utility::TiXmlDocument doc;
		std::string ss = TransToStdString(strXmlFileName);
		if (doc.LoadFile(ss))
		{
			em_utility::TiXmlElement* root = doc.FirstChildElement("result");
			if (root != NULL)
			{
				em_utility::TiXmlElement* pComponet = root->FirstChildElement("url");
				if (pComponet != NULL)
				{
					em_utility::TiXmlElement *pLrcID = pComponet->FirstChildElement("lrcid");
					if (pLrcID != NULL)
					{
						std::string lrc_id = pLrcID->GetText();

						stLrcInfo lrc_data;
						lrc_data.nLrcID = atoi(lrc_id.c_str());
						lrc_data.strArtist = _T("");
						lrc_data.strTitle = _T("");
						filesInfo.push_back(lrc_data);
						pComponet = pComponet->NextSiblingElement("lrc");
					}						
				}
			}
		}
	}
	return filesInfo.size() > 0;
}

//////////////////////////////////////////////////////////////////////////
CParseJsonFile::CParseJsonFile(void)
{

}

CParseJsonFile::~CParseJsonFile(void)
{

}

BOOL CParseJsonFile::Parse(fileinfo_container& filesInfo, const DuiLib::CDuiString& strJsonFileName)
{
	if (strJsonFileName.IsEmpty())
		return FALSE;

	if (em_utility::mci::file_exist(strJsonFileName.GetData()))
	{
		em_utility::TiXmlDocument doc;
		std::string ss = TransToStdString(strJsonFileName);
		
		std::vector<BYTE> bytes;

		if (CFileMgr::Instance()->ReadFileAllBytes(strJsonFileName.GetData(), bytes))
		{
			DWORD dwSize = bytes.size();
			LPTSTR pstrXML = NULL;
#ifdef _UNICODE
			{
				if (dwSize >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF)
				{
					bytes.erase(bytes.begin(), bytes.begin() + 3);
					dwSize -= 3;
				}
				DWORD nWide = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)&bytes[0], dwSize, NULL, 0);

				pstrXML = static_cast<LPTSTR>(malloc((nWide + 1) * sizeof(TCHAR)));
				::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)&bytes[0], dwSize, pstrXML, nWide);
				pstrXML[nWide] = _T('\0');
			}
#else // !_UNICODE
			{
				if (dwSize >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF)
				{
					bytes.erase(bytes.begin(), bytes.begin()+3);
					dwSize -= 3;
				}
				DWORD nWide = ::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)&bytes[0], dwSize, NULL, 0);

				LPWSTR w_str = static_cast<LPWSTR>(malloc((nWide + 1) * sizeof(WCHAR)));
				::MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)&bytes[0], dwSize, w_str, nWide);
				w_str[nWide] = L'\0';

				DWORD wide = ::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, NULL, 0, NULL, NULL);

				pstrXML = static_cast<LPTSTR>(malloc((wide + 1) * sizeof(TCHAR)));
				::WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)w_str, nWide, pstrXML, wide, NULL, NULL);
				pstrXML[wide] = _T('\0');
				free(w_str);
			}
#endif // _UNICODE

			Json::Reader reader;
			Json::Value value;
			DuiLib::CDuiString sParam = pstrXML;
			sParam.Replace(_T("\'"), _T("\""));
			std::string sMsgUtf8 = DuiLib::Convert::ToUTF8(sParam.GetData());
			if (reader.parse(sMsgUtf8, value) && !value.isNull())
			{
				unsigned int nSize = value.size();
				for (unsigned int i = 0; i < nSize; ++i)
				{
					Json::Value& node = value[i];
					std::string s_title = JsonParser::getValue<std::string>(node, "title");
					std::string s_artist = JsonParser::getValue<std::string>(node, "singer");
					std::string s_song_key = JsonParser::getValue<std::string>(node, "song_key");
					long id = JsonParser::getValue<long>(node, "id");
					std::wstring s_temp;
					DuiLib::CDuiString d_temp;
#define  U8_2_CDUISTRING(x) {   s_temp = DuiLib::Convert::UTF8ToUnicode(x.c_str()); d_temp = s_temp.c_str(); }
					U8_2_CDUISTRING(s_title)
					DuiLib::CDuiString sArtist = d_temp;
					U8_2_CDUISTRING(s_artist)
					DuiLib::CDuiString sTitle = d_temp;
					U8_2_CDUISTRING(s_song_key)
					DuiLib::CDuiString sKey = d_temp;
					stLrcInfo sLrc = { id,sArtist,sTitle, sKey };
					filesInfo.push_back(sLrc);
#undef U8_2_CDUISTRING
				}
			}
		}	
	}
	return filesInfo.size() > 0;
}



