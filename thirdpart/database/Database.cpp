#include "Database.h"
#include <time.h>
#include <assert.h>
#ifdef _MSC_VER
#	ifdef _DEBUG
#	include <shlwapi.h>
#	pragma comment(lib, "shlwapi.lib")
#	endif
#include <Windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#endif

/************************************************************************/
/*                                                                      */
/************************************************************************/
const char CDatabase::DicTableName[] = "dictionary";
const char CDatabase::DBVersionKey[] = "DBVersion";
const char CDatabase::LastBackupKey[] = "_LastBackup";
bool CDatabase::OpenDB(const char* sFileName)
{
	CloseDB();
	std::string sFileNameUTF8 = sFileName;
	_pDB = new CppSQLite3DB();
	try 
	{
		//printf("\nopen db file: %s", sFileName);
		_pDB->open(sFileNameUTF8.c_str());
	}
	catch(CppSQLite3Exception e)
	{
		printf("\n[Error]open db file, errorCode: %d, errorMessage: %s", e.errorCode(), e.errorMessage());
		return false;
	}
	catch(...)
	{
		printf("\n[Error]open db file unknow error!");
		return false;
	}

	if(!IsOpen())
		return false;

	try
	{
		if(!IsExistTable(DicTableName))
		{
			static const char sSql[] = "CREATE TABLE %Q (key text primary key,value text,comment text)";
			if(SQLERROR == this->ExecSql(sSql,DicTableName))
				goto _OnError;
		}
		if(!OnCreateTables())
			goto _OnError;

		const int OldDBVersion = GetDBVersion();
		//printf("\nOldDBVersion:%d", OldDBVersion);
		if(OldDBVersion > DBVersion)
		{
			goto _OnError;
		}

		if(OldDBVersion < DBVersion)
		{
			if(!BackupDB(sFileName))
				goto _OnError;

			if(!OnUpdateTables(OldDBVersion,DBVersion))
				goto _OnError;

			DeleteDict(DBVersionKey);
			if(!InsertDict(DBVersionKey,DBVersion))
				goto _OnError;
		}
		else
		{
			int nLastBackupTime = 0;
			if(GetDictValue(LastBackupKey,nLastBackupTime))
			{
				int nOffTime = (int)time(NULL) - nLastBackupTime;
				if(nOffTime > BackupDBRate)
						BackupDB(sFileName);
			}
		}
	}catch(CppSQLite3Exception e)
	{
		assert(false);
		printf("\n[Error]openDB errorCode: %d, errorMessage: %s",e.errorCode(), e.errorMessage());
		goto _OnError;
	}
	catch(...)
	{
		assert(false);
		printf("\n[Error]openDB unknow error!");
		goto _OnError;
	}

	return true;

_OnError:
	{
		this->CloseDB();
		return false;
	}
}		

bool CDatabase::CloseDB()
{ 
	if(IsOpen())
	{
		_pDB->close();
		delete _pDB;
		_pDB = NULL;
	}
	return true;
}

#ifndef _MSC_VER
bool CDatabase::copyfile(const char* src, const char* dst, bool bFailIfExists)
{
	int in,out,flag;
    char buffer[10240];
    in = open(src, O_RDONLY, S_IRUSR);
    if(in==-1)
    {
        return false;
    }
    out = creat(dst, S_IWUSR);
    if(out==-1)
    {
        return false;
    }
    while((flag=read(in,buffer,10240))>0)
    {
        write(out,buffer,flag);
    }
    close(in);
    close(out);
	return true;
}
#endif

bool CDatabase::BackupDB(const char* sFileName)
{
	std::string newFileName(sFileName);
	newFileName += ".bak";
#ifdef _MSC_VER
	if(::CopyFileA(sFileName, newFileName.c_str(), FALSE))
#else
	if(copyfile(sFileName, newFileName.c_str(), false))
#endif
	{
		DeleteDict(LastBackupKey);
		InsertDict(LastBackupKey,(int)time(NULL));
		return true;
	}
	
	return false;
}
int CDatabase::GetDBVersion()
{
	int nVer = -1;
	GetDictValue(DBVersionKey,nVer);
	return nVer;
}

bool CDatabase::GetDictValue(const char* sKey, std::string &sValue)
{
	static const char sSql[] = "select value from %Q where key = %Q;";

	std::string sKeyUTF8 = sKey;

	SQLResult rs = this->QuerySql(sSql,DicTableName,sKeyUTF8.c_str());

	if(rs.eof()) return false;

	sValue = rs.getStringField(0);
	return true;
}
bool CDatabase::GetDictValue(const char* sKey, unsigned int& nValue)
{
	std::string utf8Str;
	if (!GetDictValue(sKey, utf8Str) || utf8Str.empty())
		return false;
	nValue = atoi(utf8Str.c_str());
	return true;
}
bool CDatabase::GetDictValue(const char* sKey,int& nValue)
{
	std::string utf8Str;
	if(!GetDictValue(sKey, utf8Str) || utf8Str.empty())
		return false;
	nValue =  atoi(utf8Str.c_str());
	return true;
}
bool CDatabase::GetDictValue(const char* sKey, unsigned long& dwValue)
{
	std::string utf8Str;
	if (!GetDictValue(sKey, utf8Str) || utf8Str.empty())
		return false;
	dwValue = atol(utf8Str.c_str());
	return true;
}
bool CDatabase::GetDictValue(const char* sKey,long& dwValue)
{
	std::string utf8Str;
	if(!GetDictValue(sKey, utf8Str) || utf8Str.empty())
		return false;
	dwValue =  atol(utf8Str.c_str());
	return true;
}
bool CDatabase::GetDictValue(const char* sKey,bool& nValue)
{
	int nV = 0;
	if(!GetDictValue(sKey,nV))
		return false;
	nValue = (0 != nV);
	return true;
}

bool CDatabase::UpdateDictValue(const char* sKey, const char* sValue)
{
	static const char sSql[] = "update %Q set value = %Q where key =%Q;";

	std::string sKeyUTF8   = sKey;
	std::string sValueUTF8 = sValue;

	return SQLERROR != this->ExecSql(sSql,DicTableName,sValueUTF8.c_str(),sKeyUTF8.c_str());
}
bool CDatabase::UpdateDictValue(const char* sKey, unsigned int nValue)
{
	char buf[50] = { 0 };
#ifdef _MSC_VER
	std::string sValue = _itoa(nValue, buf, 10);
#else
	sprintf(buf, "%ud", nValue);
	std::string sValue = buf;
#endif
	return UpdateDictValue(sKey, sValue.c_str());
}
bool CDatabase::UpdateDictValue(const char* sKey,int nValue)
{
	char buf[50] = { 0 };
#ifdef _MSC_VER
	std::string sValue = _itoa(nValue, buf, 10);
#else
	sprintf(buf, "%d", nValue);
	std::string sValue = buf;
#endif
	return UpdateDictValue(sKey,sValue.c_str());
}
bool CDatabase::UpdateDictValue(const char* sKey, unsigned long dwValue)
{
	char buf[50] = { 0 };
#ifdef _MSC_VER
	std::string sValue = _ltoa(dwValue, buf, 10);
#else
	sprintf(buf, "%ld", dwValue);
	std::string sValue = buf;
#endif

	return UpdateDictValue(sKey, sValue.c_str());
}
bool CDatabase::UpdateDictValue(const char* sKey,long dwValue)
{
	char buf[50] = { 0 };
#ifdef _MSC_VER
	std::string sValue = _ltoa(dwValue, buf, 10);
#else
	sprintf(buf, "%ld", dwValue);
	std::string sValue = buf;
#endif

	return UpdateDictValue(sKey, sValue.c_str());
}
bool CDatabase::UpdateDictValue(const char* sKey,bool nValue)
{
	return UpdateDictValue(sKey,(int)nValue);
}

bool CDatabase::InsertDict(const char* sKey, const char* sValue)
{
	static const char sSql[] = "insert into %Q(key,value) values (%Q,%Q);";

	std::string sKeyUTF8 = sKey;
	std::string sValueUTF8 = sValue;

	return SQLERROR != this->ExecSql(sSql,DicTableName,sKeyUTF8.c_str(),sValueUTF8.c_str());
}
bool CDatabase::InsertDict(const char* sKey, unsigned int nValue)
{
	char buf[50] = { 0 };
#ifdef _MSC_VER
	std::string sValue = _itoa(nValue, buf, 10);
#else
	sprintf(buf, "%ud", nValue);
	std::string sValue = buf;
#endif
	return InsertDict(sKey, sValue.c_str());
}
bool CDatabase::InsertDict(const char* sKey,int nValue)
{
	char buf[50] = { 0 };
#ifdef _MSC_VER
	std::string sValue = _itoa(nValue, buf, 10);
#else
	sprintf(buf, "%d", nValue);
	std::string sValue = buf;
#endif
	return InsertDict(sKey,sValue.c_str());
}
bool CDatabase::InsertDict(const char* sKey, unsigned long dwValue)
{
	char buf[50] = { 0 };
#ifdef _MSC_VER
	std::string sValue = _ltoa(dwValue, buf, 10);
#else
	sprintf(buf, "%ld", dwValue);
	std::string sValue = buf;
#endif
	return InsertDict(sKey, sValue.c_str());
}
bool CDatabase::InsertDict(const char* sKey,long dwValue)
{
	char buf[50] = { 0 };
#ifdef _MSC_VER
	std::string sValue = _ltoa(dwValue, buf, 10);
#else
	sprintf(buf, "%ld", dwValue);
	std::string sValue = buf;
#endif
	return InsertDict(sKey, sValue.c_str());
}
bool CDatabase::InsertDict(const char* sKey,bool nValue)
{
	return InsertDict(sKey,(int)nValue);
}

bool CDatabase::DeleteDict(const char* sKey)
{
	static const char sSql[] = "delete from  %Q where key = %Q;";

	std::string sKeyUTF8 = sKey;

	return SQLERROR != this->ExecSql(sSql,DicTableName,sKeyUTF8.c_str());
}

int CDatabase::ExecScalar(const char* fmt,...)
{
	assert(IsOpen());
	assert(fmt);
	if(!IsOpen() || !fmt) return SQLERROR;

	int nResult = SQLERROR;
	SQLBuffer buffer;
	try
	{
		va_list va;
		va_start(va,fmt);
		buffer.format(fmt,va);
		va_end(va);
		nResult = _pDB->execScalar(buffer);
	}
	catch (CppSQLite3Exception e)
	{
		printf("\n[Error]ExecScalar<%s>,error<%s>",buffer.operator const char *(),e.errorMessage());
		assert(false);
		return SQLERROR;
	}
	catch (...)
	{
		printf("\n[Error]ExecScalar<%s>,error<unknow>",buffer.operator const char* ());
		assert(false);
		return SQLERROR;
	}

	return nResult;
}
int CDatabase::ExecSql(const char* fmt,...)
{
	assert(IsOpen());
	assert(fmt);
	if(!IsOpen() || !fmt) return SQLERROR;

	int nResult = SQLERROR;
	SQLBuffer buffer;
	try
	{
		va_list va;
		va_start(va,fmt);
		buffer.format(fmt,va);
		va_end(va);
		nResult = _pDB->execDML(buffer);
		//printf("\nExecSql<%s>", buffer.operator const char* ());
	}
	catch (CppSQLite3Exception e)
	{
		printf("\n[Error]ExecSql<%s>,error<%s",buffer.operator const char* (),e.errorMessage());
		assert(false);
		return SQLERROR;
	}
	catch (...)
	{
		printf("\n[Error]ExecSql<%s>,error<unknow>",buffer.operator const char* ());
		assert(false);
		return SQLERROR;
	}

	return nResult;
}
CDatabase::SQLResult CDatabase::QuerySql(const char* fmt,...)
{
	assert(IsOpen());
	assert(fmt);
	if(!IsOpen() || !fmt) return SQLResult();

	SQLResult rs;
	SQLBuffer buffer;
	try
	{
		va_list va;
		va_start(va,fmt);
		buffer.format(fmt,va);
		va_end(va);
		rs = _pDB->execQuery(buffer);
		//printf("\nQuerySql<%s>", buffer.operator const char* ());
	}
	catch (CppSQLite3Exception e)
	{
		printf("\n[Error]QuerySql<%s>,error<%s>",buffer.operator const char* (), e.errorMessage());
		assert(false);
		return SQLResult();
	}
	catch (...)
	{
		printf("\n[Error]QuerySql<%s>,error<unknow>",buffer.operator const char* ());
		assert(false);
		return SQLResult();
	}

	return rs;
}
void CDatabase::QuerySql(CDatabase::SQLResult* rs, const char* fmt, ...)
{
	assert(IsOpen());
	assert(fmt);
	if (!IsOpen() || !fmt) return;

	SQLBuffer buffer;
	try
	{
		va_list va;
		va_start(va, fmt);
		buffer.format(fmt, va);
		va_end(va);
		*rs = _pDB->execQuery(buffer);
		//printf("\nQuerySql<%s>", buffer.operator const char* ());
	}
	catch (CppSQLite3Exception e)
	{
		printf("\n[Error]QuerySql<%s>,error<%s>", buffer.operator const char* (), e.errorMessage());
		assert(false);
	}
	catch (...)
	{
		printf("\n[Error]QuerySql<%s>,error<unknow>", buffer.operator const char* ());
		assert(false);
	}
}
CDatabase::SQLResult* CDatabase::QuerySqlNew(const char* fmt, ...)
{
	CDatabase::SQLResult* rs = NULL;
	assert(IsOpen());
	assert(fmt);
	if (!IsOpen() || !fmt) return rs;

	SQLBuffer buffer;
	try
	{
		va_list va;
		va_start(va, fmt);
		buffer.format(fmt, va);
		va_end(va);
		_pDB->execQuery(buffer, &rs);
		//printf("\nQuerySqlNew<%s>", buffer.operator const char* ());
	}
	catch (CppSQLite3Exception e)
	{
		printf("\n[Error]QuerySqlNew<%s>,error<%s>", buffer.operator const char* (), e.errorMessage());
		assert(false);
	}
	catch (...)
	{
		printf("\n[Error]QuerySqlNew<%s>,error<unknow>", buffer.operator const char* ());
		assert(false);
	}

	return rs;
}
bool CDatabase::IsExistTable(const char* tablename)
{
	assert(IsOpen());
	if(!IsOpen()) return false;
	return _pDB->tableExists(tablename);
}
bool CDatabase::DeleteTable(const char* tablename)
{
	return SQLERROR != ExecSql("DROP TABLE %q;",tablename);
}

bool CDatabase::BeginTransaction()
{
	//printf("\nBegin Transaction");
	return SQLERROR != ExecSql("begin transaction;");
}
bool CDatabase::Commit()
{
	//printf("\nCommit Transaction");
	return SQLERROR != ExecSql("commit transaction;");
}
bool CDatabase::RollBack()
{
	//printf("\nRollBack Transaction");
	return SQLERROR != ExecSql("rollback transaction");
}

const char* CDatabase::GetErrorMsg()
{
	return _pDB->getErrorMsg();
}