#include "stdafx.h"

#include"lrcFileReader.h"
#include <algorithm>
#ifdef _WIN32
#include <atlstr.h>
#include <atlconv.h>
#endif

#define MAX_LINE_LENGTH 1024

bool CLrcFileReader::Load(const wchar_t * FileName)
{
	bool bRet = true;

	m_lock.Lock();
	Clear();

	FILE *file = NULL;
	_wfopen_s(&file,FileName,L"r");
// 	if ((file = _wfopen(FileName, L"r")) == NULL)
// 		return false;
	if(NULL == file)
		return false;
//	Parse(file);
	if (!Parse(file))
	{
		bRet = false;
#ifdef _DEBUG
		DuiLib::CDuiString str;
		str.Format(_T("%s, �ø�ʲ���LRC\n"), FileName);
		OutputDebugString(str);
#endif
	}
	fclose(file);	

	m_lock.Unlock();
	return bRet;
}

bool CLrcFileReader::Load(const char * FileName)
{
	bool bRet = true;

	m_lock.Lock();
	Clear();

	FILE *file = NULL;
	fopen_s(&file,FileName,"r");
// 	if ((file = fopen(FileName,"r")) == NULL)
// 		return false;
	if(NULL == file)
		return false;

	fseek(file,0,SEEK_END);
	int nFileSize = ftell(file);
	assert(nFileSize);

	if(nFileSize == 0)
		return false;
//	Parse(file);
	if (!Parse(file))
	{
		bRet = false;
#ifdef _DEBUG
		DuiLib::CDuiString str;
		str.Format(_T("%s, �ø�ʲ���LRC\n"), FileName);
		OutputDebugString(str);
#endif
	}
	fclose(file);
	m_lock.Unlock();
	return bRet;
}

bool CLrcFileReader::Parse(FILE * file )
{
	m_nErrorCounter = 0;
	TextEncodedMode mode = CheckTextEncodedMode(file);
	
	char buf[MAX_LINE_LENGTH];
	DuiLib::CDuiString sLineData;
	while (!feof(file))
	{
		buf[0] = 0;
		fgets(buf,MAX_LINE_LENGTH,file);
		
		if (mode == TEM_ANSI)
		{
			sLineData = CA2W(buf,CP_ACP);
		}
		else if (mode == TEM_UTF8)
		{
			sLineData = CA2W(buf,CP_UTF8);
		}
		else if (mode == TEM_UNICODE)
		{
			sLineData = CA2W(buf,CP_ACP);
		}
		else 
		{
			break;
		}
//		sLineData = mci::ToString(buf);
		if (!ParseLine(sLineData))
			return false;
	}
	for (size_t i=0;i<Lyrics.size();++i)
	{
		if(Lyrics[i].sLyric.IsEmpty())
		{
			Lyrics[i].sLyric = _T("�������");
		}
	}
	std::sort(Lyrics.begin(),Lyrics.end());
	if(Lyrics.size()>0)
		Lyrics.insert(Lyrics.end(),LrcNode(Lyrics[Lyrics.size()-1].StartShowTime+50,_T("�������,��������")));
	else
		Lyrics.insert(Lyrics.end(), LrcNode(50, _T("�������,��������")));
	iCount = Lyrics.size();

	return true;
}

bool CLrcFileReader::ParseLine(const DuiLib::CDuiString &line)
{
	if (!line.IsEmpty())
	{
		TCHAR character = line[0];
		if (character != _T('['))
			++m_nErrorCounter;
	}
// 	if (m_nErrorCounter > 5)
// 		return false;

	int index = 0;	
	TCHAR preKeyChar = 0;
	
	std::vector<int> list;
	DuiLib::CDuiString First, Second;

	DuiLib::CDuiString buf;
GoNext:
	while(index < line.GetLength() && line[index] != '\n'&& line[index] != '\r')
	{
		TCHAR cur_c = line[index];
		switch (cur_c)
		{
		case '['://��ʼһ���µ�tag
			if(buf.GetLength()>0)
				goto OK;
			preKeyChar='[';
			First.Empty();
			Second.Empty();
			buf.Empty();

			break;
		case ']':
			preKeyChar=']';
			Second = buf; 
			//First.ToLower();
			if(First == _T("ar" ))
				sArtist=Second;
			else if(First == _T("ti"))
				sTitle = Second;
			else if(First == _T("al"))
				sAlbum = Second;
			else if(First == _T("by"))
				sLrcWriter = Second;
			else if(First == _T("offset"))
				Offset = _ttoi((TCHAR*)&Second);
			else 
			{
				
			//	if(First.find_first_not_of("0123456789") ==string::npos  
			//		&&Second.find_first_not_of("-.0123456789") ==string::npos  )
				{
				
					int BeginShowTime=0;
					BeginShowTime+= _ttoi(First)*60*1000;
					BeginShowTime+= int(_ttof(Second)*1000);
					list.insert(list.end(),BeginShowTime);
				}
			}

			First.Empty();
			Second.Empty();
			buf.Empty();


			break;
		case ':':

			if(preKeyChar == '[')
			{
				preKeyChar=':';
				First=buf;
				buf.Empty();
			}
			else
			{
				TCHAR s[2];
				s[0]=cur_c;
				s[1]=0;
		
				buf+=s;
			}
			break;
		
		default:

			{
				TCHAR s[2];
				s[0]=cur_c;
				s[1]=0;
		
				buf+=s;			
			}
		}
		index++;
	}

OK:
	if(line[index] == '\n' || line[index] == '\r')
	{
		index++;
	}
	if(list.size() > 0)
	{
		for(size_t i=0;i<list.size();i++)
		{
			Lyrics.insert(Lyrics.end(),LrcNode(list[i],buf));
		}
	}
	buf.Empty();
	list.clear();
	if(line.GetLength() - index >5 )
		goto GoNext;
	return true;
}

 void CLrcFileReader::Clear()
 {
	sArtist.Empty();
	sTitle.Empty();
	sAlbum.Empty();
	sLrcWriter.Empty();
	iCount = 0;
	Offset = 0;
	Lyrics.clear();
 }
 /*
    ���ܣ�ʵ���ļ������ʽ���ж�
    ͨ��һ���ļ�����ǰ�������ֽڣ������жϳ��õı������ͣ�
    ANSI�������������������޸�ʽ���壻(��һ���ֽڿ�ʼ�����ļ�����)
    Unicode�� ������������ǰ�����ֽ�ΪFFFE��
    Unicode big endian����ǰ���ֽ�ΪFEFF��
    UTF-8���� ������������ǰ���ֽ�ΪEFBB�������ֽ�ΪBF
*/
 
 CLrcFileReader::TextEncodedMode CLrcFileReader::CheckTextEncodedMode(FILE * file)
 {
	 CLrcFileReader::TextEncodedMode Encode = TEM_UTF8;
	 //modify by dfn.li 2013-7-10 17:40
	 unsigned char  unicode[3] = {0xFF,0xFE,0};
	 unsigned char  ubigendian[3] = {0xFE,0xFF,0};
	 unsigned char  utf8[3] = {0xEF,0xBB,0xBF};

	 byte* lpHeader = new byte[3];
	 fseek(file,0,SEEK_SET);

	 fread(lpHeader,1,3,file);
	 fseek(file,0,SEEK_SET);

	 if(unicode[0]==lpHeader[0]&&unicode[1]==lpHeader[1])
		 Encode =  TEM_UNICODE;
	 else if(ubigendian[0]==lpHeader[0]&&ubigendian[1]==lpHeader[1])
		 Encode =  TEM_UNICODE_BIG;
	 else if(utf8[0]==lpHeader[0]&&utf8[1]==lpHeader[1]&&utf8[2]==lpHeader[2])
		 Encode =  TEM_UTF8;
	 delete []lpHeader;

    return Encode;
 }
 
 /*
 CLrcFileReader::TextEncodedMode CLrcFileReader:: CheckTextEncodedMode( FILE * file)
 {//29787 UTF-8
	 fseek(file,0,SEEK_SET);

	 short data;
	 short simples[3]={0xFEFF,0xFFFE,0xBBEF};
	 fread((char*)&data,2,1,file);
	
	 fseek(file,0,SEEK_SET);
	
	 if(data == 0xFEFF )
		 return TEM_UNICODE;	 
	 else  if(data == 0xFFFE)
		 return TEM_UNICODE_BIG;
	 else  if(data == 0xEFBB)
		 return TEM_UTF8;
	 else  
 		 return TEM_ANSI;
//		 return TEM_UTF8;
 } 
 */