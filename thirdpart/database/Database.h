#pragma once

#ifdef _MSC_VER
#ifdef BUILD_AS_DLL
#ifdef DATABASE_EXPORTS
#define DATABASE_API __declspec(dllexport)
#else
#define DATABASE_API __declspec(dllimport)
#pragma comment(lib,"database.lib")
#endif
#else
#define DATABASE_API
#endif
#else
#define DATABASE_API
#endif

#include "CppSQLite3.h"
#include <string>
#include <memory>

class DATABASE_API CDatabase
{
public:
	static const int  SQLERROR = -1;
	static const int  DBVersion = 0;
	static const int  BackupDBRate = 60*60*24;
	static const char DicTableName[]; 
	static const char DBVersionKey[];
	static const char LastBackupKey[];
	typedef CppSQLite3Query     SQLResult;
	typedef CppSQLite3Buffer    SQLBuffer;

	CDatabase():_pDB(NULL){}
	virtual ~CDatabase(){ CloseDB(); }

	bool OpenDB(const char* sFileName);
	bool CloseDB();
	bool BackupDB(const char* sFileName);
	bool IsOpen() const {return !!_pDB;}
	int GetDBVersion();

	bool GetDictValue(const char* sKey, std::string& sValue); //if sKey is exsited return true,if not exsit return false
	bool GetDictValue(const char* sKey, unsigned int& nValue);
	bool GetDictValue(const char* sKey,int& nValue);
	bool GetDictValue(const char* sKey, unsigned long& dwValue);
	bool GetDictValue(const char* sKey,long& dwValue);
	bool GetDictValue(const char* sKey,bool& nValue);
	bool UpdateDictValue(const char* sKey, const char* sValue); //if Key is exsited update it and  return true,if not exsit return false
	bool UpdateDictValue(const char* sKey, unsigned int nValue);
	bool UpdateDictValue(const char* sKey,int nValue);
	bool UpdateDictValue(const char* sKey, unsigned long dwValue);
	bool UpdateDictValue(const char* sKey, long dwValue);
	bool UpdateDictValue(const char* sKey,bool nValue);
	bool InsertDict(const char* sKey, const char* sValue);//if Key is exsited return false ,if not exsit insert new one and return true
	bool InsertDict(const char* sKey, unsigned int nValue);
    bool InsertDict(const char* sKey,int nValue);
	bool InsertDict(const char* sKey, unsigned long dwValue);
	bool InsertDict(const char* sKey, long dwValue);
    bool InsertDict(const char* sKey, bool nValue);
    bool DeleteDict(const char* sKey);

	const char* GetErrorMsg();
//protected:
	int ExecScalar(const char* fmt,...);
	int ExecSql(const char* fmt,...);
	void QuerySql(SQLResult* rs, const char* fmt, ...);
	SQLResult QuerySql(const char* fmt,...);
	SQLResult* QuerySqlNew(const char* fmt, ...);
#ifndef _MSC_VER
	bool copyfile(const char* src, const char* dst, bool bFailIfExists);
#endif
public:
	bool IsExistTable(const char* tablename);
	bool DeleteTable(const char* tablename);
	bool BeginTransaction();
	bool Commit();
	bool RollBack();
protected:
	virtual bool OnCreateTables() { return true; }
	virtual bool OnUpdateTables(int oldVer, int newVer) { return true; }
private:
	CppSQLite3DB * _pDB;
};

typedef std::shared_ptr<CDatabase> spCDatabaseT;