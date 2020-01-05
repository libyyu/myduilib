#define LUA_LIB
#include <lua.hpp>
#include <assert.h>
#include "luaext.h"
#include "Database.h"

#define Meta_Database "Meta_CDatabase"
#define Meta_CppSQLite3Query "Meta_CppSQLite3Query"
#define Meta_CppSQLite3Table "Meta_CppSQLite3Table"

extern lua_State* gL;


class LuaDatabase : public CDatabase
{
public:
	LuaDatabase() : CDatabase(), _luaRef(LUA_NOREF)
	{
	}
	virtual ~LuaDatabase()
	{
		if (_luaRef != LUA_NOREF && gL)
		{
			luaL_unref(gL, LUA_REGISTRYINDEX, _luaRef);
		}
		_luaRef = LUA_NOREF;
	}
	void BindLua(int luaref)
	{
		if (_luaRef != LUA_NOREF && gL)
		{
			luaL_unref(gL, LUA_REGISTRYINDEX, _luaRef);
		}
		_luaRef = luaref;
	}
protected:
	virtual bool OnCreateTables() 
	{ 
		if (gL && _luaRef != LUA_NOREF)
		{
			int nTop = lua_gettop(gL);
			lua_rawgeti(gL, LUA_REGISTRYINDEX, _luaRef);
			if (!lua_istable(gL, -1))
			{
				lua_settop(gL, nTop);
				luaL_error(gL, "luaref is not a table!");
				return false;
			}
			lua_getfield(gL, -1, "OnCreateTables");	//--> t, func
			if (!lua_isfunction(gL, -1))
			{
				lua_settop(gL, nTop);
				luaL_error(gL, "Fail to find function: OnCreateTables");
				return false;
			}

			int ret = lua_pcall(gL, 0, 1, 0);
			if (0 != ret)
			{
				lua_error(gL);
				lua_settop(gL, nTop);
				return false;
			}
			bool ok = false;
			if (lua_isboolean(gL, -1))
			{
				ok = lua_toboolean(gL, -1);
			}
			lua_settop(gL, nTop);
			return ok;
		}
		return true; 
	}
	virtual bool OnUpdateTables(int oldVer, int newVer) 
	{ 
		if (gL && _luaRef != LUA_NOREF)
		{
			int nTop = lua_gettop(gL);
			lua_rawgeti(gL, LUA_REGISTRYINDEX, _luaRef);
			if (!lua_istable(gL, -1))
			{
				lua_settop(gL, nTop);
				luaL_error(gL, "luaref is not a table!");
				return false;
			}
			lua_getfield(gL, -1, "OnUpdateTables");	//--> t, func
			if (!lua_isfunction(gL, -1))
			{
				lua_settop(gL, nTop);
				luaL_error(gL, "Fail to find function: OnUpdateTables");
				return false;
			}
			lua_pushinteger(gL, oldVer);
			lua_pushinteger(gL, newVer);
			int ret = lua_pcall(gL, 2, 1, 0);
			if (0 != ret)
			{
				lua_error(gL);
				lua_settop(gL, nTop);
				return false;
			}
			bool ok = false;
			if (lua_isboolean(gL, -1))
			{
				ok = lua_toboolean(gL, -1);
			}
			lua_settop(gL, nTop);
			return ok;
		}
		return true; 
	}
private:
	int _luaRef;
};

static int AutoDel(lua_State* L) 
{ 		
	lua::LuaUserData* userdata = (lua::LuaUserData*)luaL_checkudata(L, 1, Meta_Database);
	if (userdata && userdata->object && userdata->flag == 1)
	{
		LuaDatabase* pDB = static_cast<LuaDatabase*>(userdata->object);
		if (pDB) delete pDB;
		userdata->flag = 0;
		userdata->object = NULL;
	}
	return 0; 
}
static int AutoDelCppSQLite3Query(lua_State* L)
{
	lua::LuaUserData* userdata = (lua::LuaUserData*)luaL_checkudata(L, 1, Meta_CppSQLite3Query);
	if (userdata && userdata->object && userdata->flag == 1)
	{
		CDatabase::SQLResult* pQuery = static_cast<CDatabase::SQLResult*>(userdata->object);
		if (pQuery) delete pQuery;
		userdata->flag = 0;
		userdata->object = NULL;
	}
	return 0;
}
static int NewDB(lua_State* L)
{
	LuaDatabase* pDB = new LuaDatabase;
	assert(pDB);
	if (!pDB)
	{
		luaL_error(L, "Alloc CDatabase Failed");
		return 0;
	}
	if (!lua_isnoneornil(L, 1))
	{
		int luaRef = luaL_ref(L, LUA_REGISTRYINDEX);
		pDB->BindLua(luaRef);
	}
	lua::push_to_lua(L, pDB, Meta_Database);
	return 1;
}
static int OpenDB(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sFileName = lua_tostring(L, 2);
		bool ret = pDB->OpenDB(sFileName);
		lua_pushboolean(L, ret);
		return 1;
	}
	return 0;
}
static int BindLua(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	lua_pushvalue(L, 2);
	int luaRef = luaL_ref(L, LUA_REGISTRYINDEX);
	pDB->BindLua(luaRef);
	return 0;
}
static int CloseDB(lua_State* L)
{
	bool ret = false;
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	ret = pDB->CloseDB();
	lua_pushboolean(L, ret);
	return 1;
}
static int BackupDB(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	const char* sFileName = lua_tostring(L, 2);
	if (pDB)
	{
		lua_pushboolean(L, pDB->BackupDB(sFileName));
		return 1;
	}
	return 0;
}
static int IsOpen(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		lua_pushboolean(L, pDB->IsOpen());
		return 1;
	}
	return 0;
}
static int GetDBVersion(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		lua_pushinteger(L, pDB->GetDBVersion());
		return 1;
	}
	return 0;
}
static int GetDictString(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sKey = lua_tostring(L, 2);
		std::string sValue;
		bool ret = pDB->GetDictValue(sKey, sValue);
		if (ret)
		{
			lua_pushstring(L, sValue.c_str());
			return 1;
		}
	}
	return 0;
}
static int GetDictInt(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sKey = lua_tostring(L, 2);
		int nValue;
		bool ret = pDB->GetDictValue(sKey, nValue);
		if (ret)
		{
			lua_pushinteger(L, nValue);
			return 1;
		}
	}
	return 0;
}
static int GetDictBool(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sKey = lua_tostring(L, 2);
		bool nValue;
		bool ret = pDB->GetDictValue(sKey, nValue);
		if (ret)
		{
			lua_pushboolean(L, nValue);
			return 1;
		}
	}
	return 0;
}
static int SetDictString(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sKey = lua_tostring(L, 2);
		const char* sValue = lua_tostring(L, 3);
		bool ret = pDB->UpdateDictValue(sKey, sValue);
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 0;
}
static int SetDictInt(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sKey = lua_tostring(L, 2);
		int nValue = lua_tointeger(L, 3);
		bool ret = pDB->UpdateDictValue(sKey, nValue);
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 0;
}
static int SetDictBool(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sKey = lua_tostring(L, 2);
		bool nValue = lua_toboolean(L, 3);
		bool ret = pDB->UpdateDictValue(sKey, nValue);
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 0;
}
static int InsertDictString(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sKey = lua_tostring(L, 2);
		const char* sValue = lua_tostring(L, 3);
		bool ret = pDB->InsertDict(sKey, sValue);
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 0;
}
static int InsertDictInt(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sKey = lua_tostring(L, 2);
		int nValue = lua_tointeger(L, 3);
		bool ret = pDB->InsertDict(sKey, nValue);
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 0;
}
static int InsertDictBool(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sKey = lua_tostring(L, 2);
		bool nValue = lua_toboolean(L, 3);
		bool ret = pDB->InsertDict(sKey, nValue);
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 0;
}
static int DeleteDict(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sKey = lua_tostring(L, 2);
		bool ret = pDB->DeleteDict(sKey);
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}
static int ExecScalar(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sql = lua_tostring(L, 2);
		int ret = pDB->ExecScalar(sql);
		lua_pushinteger(L, ret);
		return 1;
	}
	lua_pushinteger(L, -1);
	return 1;
}
static int ExecSql(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sql = lua_tostring(L, 2);
		int ret = pDB->ExecSql(sql);
		lua_pushinteger(L, ret);
		return 1;
	}
	lua_pushinteger(L, -1);
	return 1;
}
static int QuerySql(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* sql = lua_tostring(L, 2);
		CDatabase::SQLResult* pQuery = pDB->QuerySqlNew(sql);
		lua::push_to_lua(L, pQuery, Meta_CppSQLite3Query);
		return 1;
	}
	lua_pushnil(L);
	return 1;
}
static int IsExistTable(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* tablename = lua_tostring(L, 2);
		bool ret = pDB->IsExistTable(tablename);
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}
static int DeleteTable(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		const char* tablename = lua_tostring(L, 2);
		bool ret = pDB->DeleteTable(tablename);
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}
static int BeginTransaction(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		bool ret = pDB->BeginTransaction();
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}
static int Commit(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		bool ret = pDB->Commit();
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}
static int RollBack(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		bool ret = pDB->RollBack();
		lua_pushboolean(L, ret);
		return 1;
	}
	lua_pushboolean(L, false);
	return 1;
}
static int GetErrorMsg(lua_State* L)
{
	LuaDatabase* pDB = lua::get_from_lua<LuaDatabase>(L, 1, Meta_Database);
	assert(pDB);
	if (pDB)
	{
		lua_pushstring(L, pDB->GetErrorMsg());
		return 1;
	}
	lua_pushnil(L);
	return 1;
}

static int Query_numFields(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		lua_pushinteger(L, pQuery->numFields());
		return 1;
	}
	lua_pushnil(L);
	return 1;
}
static int Query_fieldIndex(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		const char* szField = lua_tostring(L, 2);
		lua_pushinteger(L, pQuery->fieldIndex(szField));
		return 1;
	}
	lua_pushnil(L);
	return 1;
}
static int Query_fieldName(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		int nCol = lua_tointeger(L, 2);
		lua_pushstring(L, pQuery->fieldName(nCol));
		return 1;
	}
	lua_pushnil(L);
	return 1;
}
static int Query_fieldDeclType(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		int nCol = lua_tointeger(L, 2);
		lua_pushstring(L, pQuery->fieldDeclType(nCol));
		return 1;
	}
	lua_pushnil(L);
	return 1;
}
static int Query_fieldDataType(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		int nCol = lua_tointeger(L, 2);
		lua_pushinteger(L, pQuery->fieldDataType(nCol));
		return 1;
	}
	lua_pushnil(L);
	return 1;
}
static int Query_fieldValue(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		if (lua_isnumber(L, 2))
		{
			int nField = lua_tointeger(L, 2);
			lua_pushstring(L, pQuery->fieldValue(nField));
			return 1;
		}
		else if (lua_isstring(L, 2))
		{
			const char* szField = lua_tostring(L, 2);
			lua_pushstring(L, pQuery->fieldValue(szField));
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
static int Query_getIntField(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		int nNullValue = 0;
		if (lua_isnumber(L, 3))
			nNullValue = lua_tointeger(L, 3);
		if (lua_isnumber(L, 2))
		{
			int nField = lua_tointeger(L, 2);
			lua_pushinteger(L, pQuery->getIntField(nField, nNullValue));
			return 1;
		}
		else if (lua_isstring(L, 2))
		{
			const char* szField = lua_tostring(L, 2);
			lua_pushinteger(L, pQuery->getIntField(szField, nNullValue));
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
static int Query_getFloatField(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		double fNullValue = 0;
		if (lua_isnumber(L, 3))
			fNullValue = lua_tonumber(L, 3);
		
		if (lua_isnumber(L, 2))
		{
			int nField = lua_tointeger(L, 2);
			lua_pushnumber(L, pQuery->getFloatField(nField, fNullValue));
			return 1;
		}
		else if (lua_isstring(L, 2))
		{
			const char* szField = lua_tostring(L, 2);
			lua_pushnumber(L, pQuery->getFloatField(szField, fNullValue));
			return 1;
		}
		
	}
	lua_pushnil(L);
	return 1;
}
static int Query_getStringField(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		std::string szNullValue = "";
		if (lua_isstring(L, 3))
			szNullValue = lua_tostring(L, 3);
		if (lua_isnumber(L, 2))
		{
			int nField = lua_tointeger(L, 2);
			lua_pushstring(L, pQuery->getStringField(nField, szNullValue.c_str()));
			return 1;
		}
		else if (lua_isstring(L, 2))
		{
			const char* szField = lua_tostring(L, 2);
			lua_pushstring(L, pQuery->getStringField(szField, szNullValue.c_str()));
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
static int Query_getBlobField(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		int nLen;
		
		if (lua_isnumber(L, 2))
		{
			int nField = lua_tointeger(L, 2);
			const unsigned char* v = pQuery->getBlobField(nField, nLen);
			lua_pushlstring(L, (const char*)v, nLen);
			return 1;
		}
		else if (lua_isstring(L, 2))
		{
			const char* szField = lua_tostring(L, 2);
			const unsigned char* v = pQuery->getBlobField(szField, nLen);
			lua_pushlstring(L, (const char*)v, nLen);
			return 1;
		}
		
	}
	lua_pushnil(L);
	return 1;
}
static int Query_fieldIsNull(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		if (lua_isnumber(L, 2))
		{
			int nField = lua_tointeger(L, 2);
			lua_pushboolean(L, pQuery->fieldIsNull(nField));
			return 1;
		}
		else if (lua_isstring(L, 2))
		{
			const char* szField = lua_tostring(L, 2);
			lua_pushboolean(L, pQuery->fieldIsNull(szField));
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
static int Query_eof(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		lua_pushboolean(L, pQuery->eof());
		return 1;
	}
	lua_pushnil(L);
	return 1;
}
static int Query_nextRow(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		pQuery->nextRow();
		return 0;
	}
	luaL_error(L, "nextRow Failed");
	return 0;
}
static int Query_finalize(lua_State* L)
{
	CDatabase::SQLResult* pQuery = lua::get_from_lua<CDatabase::SQLResult>(L, 1, Meta_CppSQLite3Query);
	assert(pQuery);
	if (pQuery)
	{
		pQuery->finalize();
		return 0;
	}
	luaL_error(L, "finalize Failed");
	return 0;
}
static int Query_close(lua_State* L)
{
	lua::LuaUserData* userdata = (lua::LuaUserData*)luaL_checkudata(L, 1, Meta_CppSQLite3Query);
	if (userdata && userdata->object && userdata->flag == 1)
	{
		CDatabase::SQLResult* pQuery = static_cast<CDatabase::SQLResult*>(userdata->object);
		if (pQuery) delete pQuery;
		userdata->flag = 0;
		userdata->object = NULL;
		return 0;
	}
	luaL_error(L, "SQLResult close failed");
	return 0;
}

static int Table_numFields(lua_State* L)
{
	CppSQLite3Table* pTable = lua::get_from_lua<CppSQLite3Table>(L, 1, Meta_CppSQLite3Table);
	assert(pTable);
	if (pTable)
	{
		lua_pushinteger(L, pTable->numFields());
		return 1;
	}
	lua_pushnil(L);
	return 1;
}
static int Table_numRows(lua_State* L)
{
	CppSQLite3Table* pTable = lua::get_from_lua<CppSQLite3Table>(L, 1, Meta_CppSQLite3Table);
	assert(pTable);
	if (pTable)
	{
		lua_pushinteger(L, pTable->numRows());
		return 1;
	}
	lua_pushnil(L);
	return 1;
}
static int Table_fieldName(lua_State* L)
{
	CppSQLite3Table* pTable = lua::get_from_lua<CppSQLite3Table>(L, 1, Meta_CppSQLite3Table);
	assert(pTable);
	if (pTable)
	{
		int nCol = lua_tointeger(L, 2);
		lua_pushstring(L, pTable->fieldName(nCol));
		return 1;
	}
	lua_pushnil(L);
	return 1;
}
static int Table_fieldValue(lua_State* L)
{
	CppSQLite3Table* pTable = lua::get_from_lua<CppSQLite3Table>(L, 1, Meta_CppSQLite3Table);
	assert(pTable);
	if (pTable)
	{
		if (lua_isnumber(L, 2))
		{
			int nField = lua_tointeger(L, 2);
			lua_pushstring(L, pTable->fieldValue(nField));
			return 1;
		}
		else if (lua_isstring(L, 2))
		{
			const char* szField = lua_tostring(L, 2);
			lua_pushstring(L, pTable->fieldValue(szField));
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
static int Table_getIntField(lua_State* L)
{
	CppSQLite3Table* pTable = lua::get_from_lua<CppSQLite3Table>(L, 1, Meta_CppSQLite3Table);
	assert(pTable);
	if (pTable)
	{
		int nNullValue = 0;
		if (lua_isnumber(L, 3))
			nNullValue = lua_tointeger(L, 3);
		if (lua_isnumber(L, 2))
		{
			int nField = lua_tointeger(L, 2);
			lua_pushinteger(L, pTable->getIntField(nField, nNullValue));
			return 1;
		}
		else if (lua_isstring(L, 2))
		{
			const char* szField = lua_tostring(L, 2);
			lua_pushinteger(L, pTable->getIntField(szField, nNullValue));
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
static int Table_getFloatField(lua_State* L)
{
	CppSQLite3Table* pTable = lua::get_from_lua<CppSQLite3Table>(L, 1, Meta_CppSQLite3Table);
	assert(pTable);
	if (pTable)
	{
		double fNullValue = 0;
		if (lua_isnumber(L, 3))
			fNullValue = lua_tonumber(L, 3);
		if (lua_isnumber(L, 2))
		{
			int nField = lua_tointeger(L, 2);
			lua_pushnumber(L, pTable->getFloatField(nField, fNullValue));
			return 1;
		}
		else if (lua_isstring(L, 2))
		{
			const char* szField = lua_tostring(L, 2);
			lua_pushnumber(L, pTable->getFloatField(szField, fNullValue));
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
static int Table_getStringField(lua_State* L)
{
	CppSQLite3Table* pTable = lua::get_from_lua<CppSQLite3Table>(L, 1, Meta_CppSQLite3Table);
	assert(pTable);
	if (pTable)
	{
		std::string szNullValue = "";
		if (lua_isstring(L, 3))
			szNullValue = lua_tostring(L, 3);
		if (lua_isnumber(L, 2))
		{
			int nField = lua_tointeger(L, 2);
			lua_pushstring(L, pTable->getStringField(nField, szNullValue.c_str()));
			return 1;
		}
		else if (lua_isstring(L, 2))
		{
			const char* szField = lua_tostring(L, 2);
			lua_pushstring(L, pTable->getStringField(szField, szNullValue.c_str()));
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
static int Table_fieldIsNull(lua_State* L)
{
	CppSQLite3Table* pTable = lua::get_from_lua<CppSQLite3Table>(L, 1, Meta_CppSQLite3Table);
	assert(pTable);
	if (pTable)
	{
		if (lua_isnumber(L, 2))
		{
			int nField = lua_tointeger(L, 2);
			lua_pushboolean(L, pTable->fieldIsNull(nField));
			return 1;
		}
		else if (lua_isstring(L, 2))
		{
			const char* szField = lua_tostring(L, 2);
			lua_pushboolean(L, pTable->fieldIsNull(szField));
			return 1;
		}
	}
	lua_pushnil(L);
	return 1;
}
static int Table_setRow(lua_State* L)
{
	CppSQLite3Table* pTable = lua::get_from_lua<CppSQLite3Table>(L, 1, Meta_CppSQLite3Table);
	assert(pTable);
	if (pTable)
	{
		int nRow = lua_tointeger(L, 2);
		pTable->setRow(nRow);
		return 0;
	}
	luaL_error(L, "setRow Failed");
	return 0;
}
static int Table_finalize(lua_State* L)
{
	CppSQLite3Table* pTable = lua::get_from_lua<CppSQLite3Table>(L, 1, Meta_CppSQLite3Table);
	assert(pTable);
	if (pTable)
	{
		pTable->finalize();
		return 0;
	}
	luaL_error(L, "finalize Failed");
	return 0;
}
static int Table_close(lua_State* L)
{
	lua::LuaUserData* userdata = (lua::LuaUserData*)luaL_checkudata(L, 1, Meta_CppSQLite3Query);
	if (userdata && userdata->object && userdata->flag == 1)
	{
		CppSQLite3Table* pTable = static_cast<CppSQLite3Table*>(userdata->object);
		if (pTable) delete pTable;
		userdata->flag = 0;
		userdata->object = NULL;
		return 0;
	}
	luaL_error(L, "SQLTable close failed");
	return 0;
}

int lua_register_Database(lua_State* L)
{
	luaL_Reg reg[] = { 
		{"New", NewDB },
		{NULL,NULL}, 
	}; 

	luaL_Reg mt_reg[] = {
		{"OpenDB", OpenDB },
		{"CloseDB", CloseDB },
		{"BackupDB", BackupDB },
		{"IsOpen", IsOpen },
		{"GetDBVersion", GetDBVersion },
		{"GetDictString", GetDictString },
		{"GetDictInt", GetDictInt },
		{"GetDictBool", GetDictBool },
		{"SetDictString", SetDictString },
		{"SetDictInt", SetDictInt },
		{"SetDictBool", SetDictBool },
		{"InsertDictString", InsertDictString },
		{"InsertDictInt", InsertDictInt },
		{"InsertDictBool", InsertDictBool },
		{"DeleteDict", DeleteDict },
		{"ExecScalar", ExecScalar },
		{"ExecSql", ExecSql },
		{"QuerySql", QuerySql },
		{"IsExistTable", IsExistTable },
		{"DeleteTable", DeleteTable },
		{"BeginTransaction", BeginTransaction },
		{"Commit", Commit },
		{"RollBack", RollBack },
		{"BindLua", BindLua },
		{"GetErrorMsg", GetErrorMsg }
	};

	luaL_Reg mt_query[] = {
		{"numFields", Query_numFields },
		{"fieldIndex", Query_fieldIndex },
		{"fieldName", Query_fieldName },
		{"fieldDeclType", Query_fieldDeclType },
		{"fieldDataType", Query_fieldDataType },
		{"fieldValue", Query_fieldValue },
		{"getIntField", Query_getIntField },
		{"getFloatField", Query_getFloatField },
		{"getStringField", Query_getStringField },
		{"getBlobField", Query_getBlobField },
		{"fieldIsNull", Query_fieldIsNull },
		{"eof", Query_eof },
		{"nextRow", Query_nextRow },
		{"finalize", Query_finalize },
		{"close", Query_close },
	};

	int nTop = lua_gettop(L);
	{
		luaL_newmetatable(L, Meta_CppSQLite3Query); //mt
		lua_pushvalue(L, -1);//mt, mt
		lua_setfield(L, -2, "__index"); // mt

		for (int i = 0; i < sizeof(mt_query) / sizeof(mt_query[0]); ++i)
		{
			lua_pushcfunction(L, mt_query[i].func);
			lua_setfield(L, -2, mt_query[i].name);
		}
		lua_pushcfunction(L, AutoDelCppSQLite3Query);
		lua_setfield(L, -2, "__gc");

		lua_settop(L, nTop);
	}
	
	{
		/// 注册reg中的方法到Lua中（“Database”相当于类名）,reg中的方法相当于Database成员函数
		luaL_register(L, "ldb", reg);

		/// 创建名字为 Meta_Database 元表	
		luaL_newmetatable(L, Meta_Database);//mt
		/// 修改元表 Meta_Database 查找索引，把它指向 Meta_Database 自身	
		lua_pushvalue(L, -1);//mt,mt
		lua_setfield(L, -2, "__index");//mt
		/// 自动删除，如果表里有__gc,Lua的垃圾回收机制会调用它。	
		lua_pushcfunction(L, AutoDel);	//mt,func
		lua_setfield(L,-2, "__gc"); 	//mt

		for (int i = 0; i < sizeof(mt_reg) / sizeof(mt_reg[0]); ++i)
		{
			lua_pushcfunction(L, mt_reg[i].func);
			lua_setfield(L, -2, mt_reg[i].name);
		}
		
		lua_settop(L, nTop);
	}

	return 1;
}