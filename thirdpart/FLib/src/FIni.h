#ifndef __FLIB_INI_H__
#define __FLIB_INI_H__
#pragma once
#include "FType.h"
#include <map> 
#include <fstream>
#include <sstream>
#include <iostream>
_FStdBegin

class F_DLL_API FIni
{
public:
	typedef char														chartype;
	typedef std::string                                                 stringtype;
	typedef std::ifstream                                               ifstreamtype;
	typedef std::ofstream                                               ofstreamtype;
	typedef std::stringstream                                           stringstreamtype;
	struct FINI_KEYVALUE
	{
		bool			hasComment;	//	true, Comment
		stringtype		strComment;	//	comment
		stringtype		strKey;		//	Key name
		stringtype		strValue;	//	Value string

		stringtype ToString();
	};

	struct FINI_SECTION
	{
		bool					hasComment;
		stringtype				strComment;	
		stringtype				strSession;
		std::vector<FINI_KEYVALUE*>	values;	//	Keys
		
		stringtype ToString();
	};

	typedef std::map<stringtype, FINI_SECTION*, std::less<stringtype> >          FINI_SectionMap;
private:
    FINI_SectionMap _SessionArr;
public:
	FIni();
	~FIni();
	void Clear();
public:
	FINI_SECTION * operator[] (const char* psection);
	FINI_SECTION* operator[](const stringtype& section);

	bool OpenIni(const char* pFile);
	bool OpenFromString(const char* content);
	bool SaveIni(const char* pFile);
	
	stringtype GetStr(const chartype* SectionStr, const chartype* KeyStr, const chartype* DefaultStr = "") const;

	int GetInt(const chartype* pSection, const chartype* pKey, int nDefault = 0) const;
	bool GetBool(const chartype* pSection, const chartype* pKey, bool bDefault = false)  const;
	float GetFloat(const chartype* pSection, const chartype* pKey, float fDefault = 0.0f)  const;

	void AddStr(const chartype* pSection, const chartype* pKey, const chartype* pValue, const chartype* strSessionComment = NULL, const chartype* strValueComment = NULL);
	void AddInt(const chartype* pSection, const chartype* pKey, int iValue, const chartype* strSessionComment = NULL, const chartype* strValueComment = NULL);
	void AddFloat(const chartype* pSection, const chartype* pKey, float fValue, const chartype* strSessionComment = NULL, const chartype* strValueComment = NULL);
	void AddBool(const chartype* pSection, const chartype* pKey, bool bValue, const chartype* strSessionComment = NULL, const chartype* strValueComment = NULL);

	inline FINI_SECTION* GetSession(const chartype* SectionStr);
	inline FINI_SECTION* CreateIfNoSession(const chartype* SectionStr, const chartype* strComment = NULL);

	stringtype ToString();

 private:  
	stringtype DoSwitchALine(const stringtype &aLineStr, stringtype &curSection, stringtype& curComment, bool& bComment);
  
	FINI_SECTION* FindSession(const chartype* SectionStr) const;

    FINI_SECTION* AddSection(const chartype* SectionStr, const chartype* CommentStr = NULL);
   
	FINI_KEYVALUE* AddKey(const chartype *SectionStr, const chartype* KeyStr, const chartype* VauleStr, const chartype* strComment = NULL);

    bool GetString(const chartype* SectionStr, const chartype* KeyStr, stringtype &value)  const;
};
_FStdEnd
#endif//__FLIB_INI_H__
