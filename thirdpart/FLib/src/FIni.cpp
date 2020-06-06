#define FLIB_DLL
#include "FIni.h"
#include <functional>
#include <memory>
_FStdBegin


FIni::stringtype FIni::FINI_KEYVALUE::ToString()
{
	stringtype result;
	for (size_t i = 0; i < nblank; ++i) result += "\n";
	for (size_t i = 0; i < comments.size(); ++i) {
		result += ";";
		result += comments[i];
		result += "\n";
	}

	result += strKey + "=" + strValue;
	return result;
}

FIni::stringtype FIni::FINI_SECTION::ToString()
{
	stringtype result;
	for (size_t i = 0; i < nblank; ++i) result += "\n";
	for (size_t i = 0; i < comments.size(); ++i) {
		result += ";";
		result += comments[i];
		result += "\n";
	}
	result += "[" + strSession + "]";
	result += "\n";

	size_t cursor = 0;
	for (std::vector<FINI_KEYVALUE*>::const_iterator it = values.begin(); it != values.end(); ++it, ++cursor)
	{
		result += (*it)->ToString();
		result += "\n";
	}

	return result;
}

FIni::FIni() {};
FIni::~FIni() { Clear(); };

void FIni::Clear()
{
	for (FINI_Type::iterator itor = _SessionArr.begin(); itor != _SessionArr.end(); ++itor)
	{
		FINI_SECTION* pKeyMap = *itor;
		for (std::vector<FINI_KEYVALUE*>::iterator it = pKeyMap->values.begin(); it != pKeyMap->values.end(); ++it)
		{
			FINI_KEYVALUE* pKV = *it;
			pKV->comments.clear();
			delete pKV;
		}
		pKeyMap->values.clear();
		delete pKeyMap;
	}
	_SessionArr.clear();
}

FIni::FINI_SECTION* FIni::operator[] (const chartype* psection)
{
	return GetSession(psection);
}
FIni::FINI_SECTION* FIni::operator[](const FIni::stringtype& section)
{
	return (*this)[section.c_str()];
}

bool FIni::OpenIni(const char* pFile)
{
	assert(pFile);
	if (NULL == pFile)
		return false;

	ifstreamtype ifs(pFile);
	if (!ifs.is_open())
		return false;

	Clear();

	std::vector<stringtype> filebuf;
	std::vector<stringtype>::const_iterator itor;
	while (!ifs.eof())
	{
		stringtype buf;
		getline(ifs, buf);
		filebuf.push_back(buf);
	}
	stringtype curSection = "";
	size_t nblank = 0;
	std::vector<stringtype> comments;
	for (itor = filebuf.begin(); itor != filebuf.end(); ++itor)
	{
		curSection = DoSwitchALine(*itor, curSection, comments, nblank);
	}
	return true;
}

bool FIni::OpenFromString(const chartype* content)
{
	assert(content);
	if (!content) return false;
	Clear();
	const chartype* p = content;
	const chartype* prev = content;
	std::vector<stringtype> filebuf;
	std::vector<stringtype>::const_iterator itor;
	while (*p)
	{
		if (*p == '\n')
		{
			stringtype buf(prev, p - prev);
			p++;
			prev = p;
			filebuf.push_back(buf);
		}
		p++;
	}
	stringtype curSection = "";
	std::vector<stringtype> comments;
	size_t nblank = 0;
	for (itor = filebuf.begin(); itor != filebuf.end(); ++itor)
	{
		curSection = DoSwitchALine(*itor, curSection, comments, nblank);
	}
	return true;
}

bool FIni::SaveIni(const char* pFile)
{
	assert(pFile);
	if (NULL == pFile)
		return false;

	ofstreamtype ofs(pFile);
	if (!ofs.is_open())
		return false;

	stringtype content = ToString();
	ofs << content.c_str();

	ofs.flush();
	ofs.close();
	return true;
}

FIni::stringtype FIni::GetStr(const FIni::chartype* SectionStr, const FIni::chartype* KeyStr, const FIni::chartype* DefaultStr/* = ""*/) const
{
	stringtype value;
	if (!GetString(SectionStr, KeyStr, value) || value.empty())
		value = DefaultStr;

	return value;
}

int FIni::GetInt(const FIni::chartype* pSection, const FIni::chartype* pKey, int nDefault/* = 0*/) const
{
	stringtype value;
	if (!GetString(pSection, pKey, value) || value.empty())
		return nDefault;

	return atoi(value.c_str());
}
bool FIni::GetBool(const FIni::chartype* pSection, const FIni::chartype* pKey, bool bDefault/* = false*/)  const
{
	return GetInt(pSection, pKey, bDefault ? 1 : 0) != 0 ? true : false;
}
float FIni::GetFloat(const FIni::chartype* pSection, const FIni::chartype* pKey, float fDefault/* = 0.0f*/)  const
{
	stringtype value;
	if (!GetString(pSection, pKey, value) || value.empty())
		return fDefault;

	return (float)atof(value.c_str());
}

void FIni::AddStr(const FIni::chartype* pSection, const FIni::chartype* pKey, const FIni::chartype* pValue, const FIni::chartype* strSessionComment/* = NULL*/, const FIni::chartype* strValueComment/* = NULL*/)
{
	AddSection(pSection, strSessionComment);
	AddKey(pSection, pKey, pValue, strValueComment);
}
void FIni::AddInt(const FIni::chartype* pSection, const FIni::chartype* pKey, int iValue, const FIni::chartype* strSessionComment/* = NULL*/, const FIni::chartype* strValueComment/* = NULL*/)
{
	stringstreamtype ss;
	ss << iValue;
	stringtype str = ss.str();
	AddSection(pSection, strSessionComment);
	AddKey(pSection, pKey, str.c_str(), strValueComment);
}
void FIni::AddFloat(const FIni::chartype* pSection, const FIni::chartype* pKey, float fValue, const FIni::chartype* strSessionComment/* = NULL*/, const FIni::chartype* strValueComment/* = NULL*/)
{
	stringstreamtype ss;
	ss << fValue;
	stringtype str = ss.str();
	AddSection(pSection, strSessionComment);
	AddKey(pSection, pKey, str.c_str(), strValueComment);
}
void FIni::AddBool(const FIni::chartype* pSection, const FIni::chartype* pKey, bool bValue, const FIni::chartype* strSessionComment/* = NULL*/, const FIni::chartype* strValueComment/* = NULL*/)
{
	stringstreamtype ss;
	bValue ? (ss << 1) : (ss << 0);
	stringtype sValue = ss.str();
	AddSection(pSection, strSessionComment);
	AddKey(pSection, pKey, sValue.c_str(), strValueComment);
}

FIni::FINI_SECTION* FIni::GetSession(const FIni::chartype* SectionStr)
{
	return FindSession(SectionStr);
}
FIni::FINI_SECTION* FIni::CreateIfNoSession(const FIni::chartype* SectionStr, const FIni::chartype* strComment/* = NULL*/, size_t nblank/* = 0*/)
{
	return AddSection(SectionStr, strComment, nblank);
}

FIni::stringtype FIni::ToString()
{
	size_t cursor = 0;
	std::stringstream str;
	for (FINI_Type::iterator itor = _SessionArr.begin(); itor != _SessionArr.end(); ++itor, ++cursor)
	{
		FINI_SECTION* pKeyMap = *itor;
		str << pKeyMap->ToString();
		if (cursor < _SessionArr.size() - 1) str << std::endl;
	}

	return str.str();
}

FIni::stringtype FIni::DoSwitchALine(const FIni::stringtype &aLineStr, FIni::stringtype &curSection, std::vector<stringtype>& comments, size_t& nblank)
{
	if (aLineStr.empty()) 
	{ //空行
		nblank++;
		return curSection; 
	}
	if (';' == aLineStr.at(0))
	{ //注释
		comments.push_back(aLineStr.substr(1));
		return curSection;
	}

	stringtype::size_type n;
	if ('[' == aLineStr.at(0))
	{ //section开始
		n = aLineStr.find(']', 1);
		curSection = aLineStr.substr(1, n - 1);
		FIni::stringtype strComment;
		for (size_t i=0;i<comments.size();++i)
		{
			strComment += comments[i];
			if (i < comments.size() - 1) strComment += "\n";
		}
		AddSection(curSection.c_str(), !comments.empty() ? strComment.c_str() : NULL, nblank);
		comments.clear();
		nblank = 0;
		return curSection;
	}

	stringtype strKey;
	stringtype strVaule;
	n = aLineStr.find('=', 0);
	if (stringtype::npos == n) {
		return curSection; //不是一个正确的key=value 
	}

	strKey = aLineStr.substr(0, n);
	if (stringtype::npos == n + 1)
		strVaule = "";
	else
		strVaule = aLineStr.substr(n + 1);
	stringtype strComment;
	for (size_t i = 0; i < comments.size(); ++i)
	{
		strComment += comments[i];
		if (i < comments.size() - 1) strComment += "\n";
	}
	AddKey(curSection.c_str(), strKey.c_str(), strVaule.c_str(), !comments.empty() ? strComment.c_str() : NULL, nblank);
	comments.clear();
	nblank = 0;
	return curSection;
}

FIni::FINI_SECTION* FIni::FindSession(const FIni::chartype* SectionStr) const
{
	std::string strSession(SectionStr);
	FINI_SECTION* pSession = nullptr;
	for (auto it= _SessionArr.begin(); it != _SessionArr.end(); ++it)
	{
		if ((*it)->strSession == strSession) {
			pSession = *it;
			break;
		}
	}
	return pSession;
}

FIni::FINI_SECTION* FIni::AddSection(const FIni::chartype* SectionStr, const FIni::chartype* CommentStr/* = NULL*/, size_t nblank/*=0*/)
{
	FIni::stringtype strSession(SectionStr);
	FINI_SECTION* pSession = FindSession(SectionStr);
	if (!pSession)
	{
		pSession = new FINI_SECTION;
		pSession->strSession = strSession;
		_SessionArr.push_back(pSession);
	}
	pSession->nblank = nblank;

	if (!!CommentStr) {
		FIni::stringtype strComment;
		while (*CommentStr)
		{
			if (*CommentStr == '\n') {
				pSession->comments.push_back(strComment);
				strComment.clear();
			}
			else {
				strComment += *CommentStr;
			}
			CommentStr++;
		}
		if (!strComment.empty()) {
			pSession->comments.push_back(strComment);
			strComment.clear();
		}
	}
	pSession->strSession = strSession;

	return pSession;
}

FIni::FINI_KEYVALUE* FIni::AddKey(const FIni::chartype *SectionStr, const FIni::chartype* KeyStr, const FIni::chartype* VauleStr, const FIni::chartype* CommentStr/* = NULL*/, size_t nblank/* = 0*/)
{
	FINI_SECTION* pSession = FindSession(SectionStr);
	if (!pSession) return nullptr;
	FINI_KEYVALUE* pKV = new FINI_KEYVALUE;
	pKV->strKey = KeyStr;
	pKV->strValue = VauleStr;
	pKV->nblank = nblank;
	if (!!CommentStr) {
		FIni::stringtype strComment;
		while (*CommentStr)
		{
			if (*CommentStr == '\n') {
				pKV->comments.push_back(strComment);
				strComment.clear();
			}
			else {
				strComment += *CommentStr;
			}
			CommentStr++;
		}
		if (!strComment.empty()) {
			pKV->comments.push_back(strComment);
			strComment.clear();
		}
	}

	pSession->values.push_back(pKV);

	return pKV;
}

bool FIni::GetString(const FIni::chartype* SectionStr, const FIni::chartype* KeyStr, FIni::stringtype &value)  const
{
	FINI_SECTION* pSession = FindSession(SectionStr);
	if (!pSession)
		return false;

	for (std::vector<FINI_KEYVALUE*>::iterator it = pSession->values.begin(); it != pSession->values.end(); ++it)
	{
		if ((*it)->strKey.compare(KeyStr) == 0)
		{
			value = (*it)->strValue;
			return true;
		}

	}

	return false;
}

_FStdEnd

