#ifndef LUA_SCRIPTS_HPP
#define LUA_SCRIPTS_HPP
#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <cassert>
#include <lua.hpp>
#include <tuple>
#include <functional>

#define GC_META_NAME "(_GC_META_)"
#define LUA_FUNC_DELC(name) int lua_##name(lua_State* L);
#define LUA_FUNC_IMPL(name) int lua_##name(lua_State* L)
#define LUA_FUNC_NAME(name) lua_##name
#define LUA_FUNC_REG(name) { #name, lua_##name }

//base types
namespace lua
{
#ifdef _MSC_VER
    typedef __int64           				int64;
    typedef unsigned __int64  				uint64;
#else
    typedef __int64_t         				int64;
    typedef __uint64_t        				uint64;
#endif

    enum class type : int {
        none          = LUA_TNONE,
        nil           = LUA_TNIL,
        string        = LUA_TSTRING,
        number        = LUA_TNUMBER,
        thread        = LUA_TTHREAD,
        boolean       = LUA_TBOOLEAN,
        function      = LUA_TFUNCTION,
        userdata      = LUA_TUSERDATA,
        lightuserdata = LUA_TLIGHTUSERDATA,
        table         = LUA_TTABLE,
        poly          = none   | nil     | string   | number   | thread       |
                        table  | boolean | function | userdata | lightuserdata
    };

    struct lua_nil_t {};
	static const lua_nil_t Nil;

    struct lua_bytes_t {
		std::basic_string<unsigned char, std::char_traits<unsigned char>, std::allocator<unsigned char> > buff;
		bool is_nil;
		lua_bytes_t(unsigned char* buff_ = nullptr) {
			init(buff_);
		}
		void init(unsigned char* buff_){
			if (buff_){
				is_nil = false;
				buff.assign(buff_);
			}
			else {
				is_nil = true;
			}
		}
		size_t length() const {
			return buff.size();
		}
		operator const unsigned char*() { return buff.c_str(); }
	};

	class lua_exception_t : public std::exception
	{
	public:
		explicit lua_exception_t(const char* err_) :
			m_err(err_)
		{}
		explicit lua_exception_t(const std::string& err_) :
			m_err(err_){}
		~lua_exception_t() noexcept {}
		virtual char const* what() const
		{
			return m_err.c_str();
		}
	private:
		std::string m_err;
	};

	struct lua_stack_gurad
	{
		lua_State* L;
		int nTop;
		lua_stack_gurad(lua_State* L_) : L(L_){ nTop = lua_gettop(L); }
		~lua_stack_gurad() { lua_settop(L, nTop); }
	};

	template < typename T >
	struct class_type
	{
		static std::string name_;
		static std::string meta()
		{
			return name_ +"_meta";
		}

		static bool is_registered()
		{
			return !name_.empty();
		}
	};

	template < typename T >
	std::string class_type<T>::name_;

	// userdata holder
	struct user {
		user(void* p, int flag_) : obj(p), flag(flag_) {}
		virtual ~user() {
			flag = 0; obj = nullptr;
		}
		void* obj;
		int flag;
	};
	
}

//utils
namespace lua
{
#define LUA_CHECK_PARAM(expr, type, idx) lua::type_assert(L, (expr), (int)type, idx, #expr, __FILE__, __LINE__);

    inline void type_assert(lua_State *L, bool suc, int type, int idx, const char *msg, const char* file, int line)
	{
		if( suc )
			return;

		std::ostringstream os;
		os << "lua parameter error: " << std::endl
			<< "\tindex [" << idx << "]" << std::endl
			<< "\treal type [" << ::lua_typename(L, ::lua_type(L, idx)) << "]" << std::endl
			<< "\texpected type [" << ::lua_typename(L, type) << "]" << std::endl
			<< "\tinformation [" << msg << "]" << std::endl
			<< "\t" << file << ":" << line << std::endl;

		luaL_error(L, "%s\n", os.str().c_str());
	}

	//create table and put it at top
	inline void create_table(lua_State* L, const char* name)
	{
		assert(name);
		lua_getglobal(L, "_G");//g
		std::string tn;
		for (size_t i = 0; i < strlen(name); ++i)
		{
			if (name[i] == '.') {
				if (tn.empty()) continue;
				lua_pushstring(L, tn.c_str());
				lua_rawget(L, -2);
				if (lua_isnil(L, -1))
				{
					lua_pop(L, 1);
					lua_createtable(L, 0, 0);//g,t
					lua_pushstring(L, tn.c_str());//g,t,n
					lua_pushvalue(L, -2);//g,t,n,t
					lua_rawset(L, -4);//g,t //g.n=t
				}
				lua_remove(L, -2); //t
				tn.clear();
			}
			else{
				tn += name[i];
			}
		}
		if (!tn.empty())
		{
			lua_pushstring(L, tn.c_str());//t,name
			lua_rawget(L, -2);//t,t1
			if (lua_isnil(L, -1))
			{
				lua_pop(L, 1);//g
				lua_createtable(L, 0, 0);//g,t
				lua_pushstring(L, tn.c_str());//g,t,name
				lua_pushvalue(L, -2);//g,t,name,t
				lua_rawset(L, -4);//g,t
			}
			lua_remove(L, -2);//t
		}
	}

	//get table and put it at top, maybe nil
	inline void get_table(lua_State* L, const char* name)
	{
		assert(name);
		lua_getglobal(L, "_G");//g
		std::string tn;
		for (size_t i = 0; i < strlen(name); ++i)
		{
			if (name[i] == '.'){
				if (tn.empty()) continue;
				lua_pushstring(L, tn.c_str());
				lua_rawget(L, -2);
				lua_remove(L, -2);
				tn.clear();
			}
			else{
				tn += name[i];
			}
		}
		if (!tn.empty())
		{
			lua_pushstring(L, tn.c_str());//g,name
			lua_rawget(L, -2);//g,t
			lua_remove(L, -2);//
		}
	}

	/////////////////////////////////////////////////////////////////////////////
	inline void push_metatable(lua_State* L, const std::string& name) {
		luaL_getmetatable(L, name.c_str());
	}
	inline void push_gc_metatable(lua_State* L) {
		luaL_getmetatable(L, GC_META_NAME);
	}
	inline void bind_metatable(lua_State* L, const std::string& name) {
		//-1 is userdata
		push_metatable(L, name);
		lua_setmetatable(L, -2);
	}
	inline void bind_gc_metatable(lua_State* L) {
		push_gc_metatable(L);
		lua_setmetatable(L, -2);
	}
	inline void init_gc_meta(lua_State* L)
	{
		auto lambda = [](lua_State* l)->int
		{
			return 0;
		};
		luaL_newmetatable(L, GC_META_NAME);
		lua_pushliteral(L, "__gc");
		lua_pushcfunction(L, lambda);
		lua_rawset(L, -3);

		lua_pop(L, 1);
	}
	template<typename T>
	inline void bind_property_metatable(lua_State* L) {
		if (class_type<T>::is_registered()) {
			bind_metatable(L, class_type<T>::name());
		}
		else {
			bind_gc_metatable(L);
		}
	}

	/////////////////////////////////////////////////////////////////////
	inline bool record_object_to_lua_global_table(lua_State* L, void* obj)
	{//top is ud
		assert(obj);
		lua_getglobal(L, "_G");//ret,G
		lua_getfield(L, -1, "_accessChildren");//ret,G,t
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);//ret,G
			lua_newtable(L);//ret,G,t
			lua_pushvalue(L, -1);//ret,G,t,t
			lua_setfield(L, -3, "_accessChildren");//ret,G,t
		}
		lua_pushvalue(L, -3);//ret,G,t,ret

		lua_gettable(L, -2);//ret,G,t,v
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1); //ret,G,t
			lua_pushvalue(L, -3);//ret,G,t,ret
			lua_pushboolean(L, 1);//ret,G,t,ret,true
			lua_settable(L, -3);//ret,G,t
			lua_pop(L, 2);//ret
		}
		else
		{
			lua_pop(L, 3);
		}
		return true;
	}

	inline bool remove_object_from_lua_global_table(lua_State* L, void* obj)
	{//top is ud
		if (lua_isnil(L, -1))
			return false;

		lua_getglobal(L, "_G");//ret,G
		lua_getfield(L, -1, "_accessChildren");//ret,G,t
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 2);//
			return false;
		}
		lua_pushvalue(L, -3);//ret,G,t,ret

		lua_gettable(L, -2); //ret,G,t,v
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 3); //ret
			return false;
		}
		else
		{
			lua_pop(L, 1);//ret,G,t
			lua_pushvalue(L, -3);//ret,G,t,ret
		}

		lua_pushnil(L);//ret,G,t,ret,nil
		lua_settable(L, -3);//ret,G,t
		lua_pop(L, 2);//ret
		return false;
	}
	
	inline bool try_get_from_weaktable(lua_State* L, void* obj)
	{
		lua_getglobal(L, "g_udref"); //ref
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);	//
			lua_newtable(L); //ref
			lua_newtable(L); //ref,mt
			lua_pushstring(L, "v"); //ref,mt,"v"
			lua_setfield(L, -2, "__mode"); //ref,mt
			lua_setmetatable(L, -2); //ref
			lua_pushvalue(L, -1); //ref,ref
			lua_setglobal(L, "g_udref"); //ref
		}
		lua_pushlightuserdata(L, obj); //ref,obj
		lua_gettable(L, -2); //ref,ud
		
		if (lua_isuserdata(L, -1) || lua_islightuserdata(L, -1))
		{
			user* p = (user*)lua_touserdata(L, -1);
			if (p != nullptr) {
				lua_remove(L, -2);
				p->obj = obj;
				assert(p->flag == 1);
				return true;
			}
		}
		else if (!lua_isnoneornil(L, -1))
		{
			const char* n = lua_typename(L, lua_type(L, -1));
			luaL_error(L, "argument types do not match, userdata expected get %s.", n);
		}
		lua_pop(L, 2);
		return false;
	}

	inline bool remove_from_weaktable(lua_State* L, void* obj)
	{
		lua_getglobal(L, "g_udref"); //ref
		if (lua_isnil(L, -1))
		{
			lua_pop(L, 1);	//
			return false;
		}
		lua_pushlightuserdata(L, obj);//ref, obj
		lua_gettable(L, -2); //ref,ud
		if (lua_isnoneornil(L, -1)) {
			lua_pop(L, 2);
			return false;
		}
		lua_pop(L, 1);//ref
		lua_pushlightuserdata(L, obj);//ref, obj
		lua_pushnil(L);
		lua_settable(L, -3);//ref
		lua_pop(L, 1);
		return true;
	}

	inline void record_to_weaktable(lua_State* L, void* obj)
	{//top is ud
		lua_getglobal(L, "g_udref"); //ud,t
		lua_pushlightuserdata(L, obj); //ud,t,obj
		lua_pushvalue(L, -3); //ud,t,obj,ud
		lua_settable(L, -3); //ud, t
		lua_pop(L, 1);
	}
	
	// to lua
	struct ptr2user : user {
		lua_State* L = nullptr;
		ptr2user(lua_State* L_, void* ptr, int flag_) : L(L_), user(ptr, flag_) {}
		virtual ~ptr2user() noexcept {
			//if (obj && L) {
			//	remove_object_from_lua_global_table(L, obj);
			//	remove_from_weaktable(L, obj);
			//}
		}
		template <typename T>
		void unbind(lua_State* l, bool gc) {
			remove_object_from_lua_global_table(l, obj);
			bool ret = remove_from_weaktable(l, obj);
			T* o = static_cast<T*>(obj);
			/*try {
				if (ret && !gc && o) o->~T();
			}
			catch (std::exception& e)
			{
				printf("%s\n", e.what());
			}
			catch (...)
			{

			}*/
			
			this->~ptr2user();
		}
	};

	inline void push_object(lua_State* L, const void* const_obj, const char* mtname)
	{
		if (!const_obj)
		{
			lua_pushnil(L);
			return;
		}
		void* obj = const_cast<void*>(const_obj);
		return push_object(L, obj, mtname);
	}
	inline void push_object(lua_State* L, void* obj, const char* mtname)
	{
		if (!obj)
		{
			lua_pushnil(L);
			return;
		}
		if (!try_get_from_weaktable(L, obj)) {//ud,poped is false
			if (mtname) {
				new (lua_newuserdata(L, sizeof(ptr2user))) ptr2user(L, obj, 1);//ud
				bind_metatable(L, mtname);

				record_to_weaktable(L, obj);
			}
			else {
				lua_pushlightuserdata(L, obj);//ud
			}
		}
	}
	
	inline user* get_userdata(lua_State* L, int ParamIndex, const char* mtname)
	{
		if (!lua_isuserdata(L, ParamIndex))
			return nullptr;

		user* ud = nullptr;
		if (mtname)
			ud = (user*)luaL_checkudata(L, ParamIndex, mtname);
		else
			ud = (user*)lua_touserdata(L, ParamIndex);
		return ud;
	}

	inline user* get_userdata(lua_State* L, int ParamIndex)
	{
		if (!lua_isuserdata(L, ParamIndex))
			return nullptr;

		bool has_mt = lua_getmetatable(L, ParamIndex);//ud,mt,失败不压栈
		if (has_mt)
		{
			lua_pushstring(L, "__mtname");//ud,mt,key
			lua_rawget(L, -2); //ud,mt, mtname
			if (lua_isstring(L, -1))
			{
				const char* mtname = lua_tostring(L, -1);
				lua_pop(L, 2); //ud
				return get_userdata(L, ParamIndex, mtname);
			}
			else
			{
				lua_pop(L, 2); //ud
				return get_userdata(L, ParamIndex, nullptr);
			}
		}
		else//ud
		{
			return get_userdata(L, ParamIndex, nullptr);
		}
	}

	inline void* get_object(lua_State* L, int ParamIndex, const char* mtname, bool poperror = true)
	{
		if (mtname)
		{
			user* ud = get_userdata(L, ParamIndex, mtname);
			if (ud && !ud->obj)
			{
				if (poperror)
				{
					lua_pushstring(L, "content of userdata has been removed!");
					lua_error(L);
				}
				return nullptr;
			}
			if (ud && !ud->flag)
			{
				ud->obj = nullptr;
				if (poperror)
				{
					lua_pushstring(L, "content of userdata has been disappeared!");
					lua_error(L);
				}
				return nullptr;
			}
			return ud != nullptr ? ud->obj : nullptr;
		}
		else
		{
			if (!lua_isuserdata(L, ParamIndex))
			{
				if (poperror)
				{
					lua_pushstring(L, "get_object #1 must be userdata");
					lua_error(L);
				}
				return nullptr;
			}
			return lua_touserdata(L, ParamIndex);
		}
	}

	inline void* get_object(lua_State* L, int ParamIndex, bool poperror = true)
	{
		if (!lua_isuserdata(L, ParamIndex))
		{
			if (poperror)
			{
				lua_pushstring(L, "get_object #1 must be userdata");
				lua_error(L);
			}
			return nullptr;
		}
		bool has_mt = lua_getmetatable(L, ParamIndex);//ud, mt,失败不压栈
		if (has_mt)
		{
			lua_pushstring(L, "__mtname");//ud, mt, key
			lua_rawget(L, -2); //ud, mt, mtname
			if (lua_isstring(L, -1))
			{
				const char* mtname = lua_tostring(L, -1);
				lua_pop(L, 2); //ud
				return get_object(L, ParamIndex, mtname, poperror);
			}
			else
			{
				lua_pop(L, 2); //ud
				return get_object(L, ParamIndex, nullptr, poperror);
			}
		}
		else
		{
			return get_object(L, ParamIndex, nullptr, poperror);
		}
	}

	template<typename T>
	inline T* check_type(lua_State* L, int ParamIndex)
	{
		void* obj = get_object(L, ParamIndex, true);
		T* ud = static_cast<T*>(obj);
		if (!ud)
		{
			char err[100] = { 0 };
			sprintf(err, "param #%d must be a userdata", ParamIndex);
			lua_pushstring(L, err);
			lua_error(L);
			return nullptr;
		}
		return ud;
	}
	template<typename T>
	inline T* get_type(lua_State* L, int ParamIndex)
	{
		void* obj = get_object(L, ParamIndex, false);
		T* ud = static_cast<T*>(obj);
		return ud;
	}
}

//stack push
namespace lua
{
    template<typename T>
	struct lua_op_t;

    template<>
	struct lua_op_t<int64>
	{
		static int push_stack(lua_State* L, int64 value)
		{
			lua_pushlstring(L, (const char*)&value, 8);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, int64* value)
		{	
            if(!lua_isnumber(L, pos) && !lua_isstring(L, pos))
            {
                luaL_error(L, "expected 'int64', received %s", lua_typename(L, lua_type(L, pos)));
                return;
            }
			if (lua_isnumber(L, pos))
			{
				lua_Number n = luaL_checknumber(L, pos);
				*value = (int64)n;
				return;
			}
			size_t len = 0;
			const char* str = luaL_checklstring(L, pos, &len);
			if (len != 8)
			{
				luaL_error(L, "bad int64 string length (8 expected, got %d)", int(len));
				return;
			}
			*value = *(int64*)str;
		}
		static bool try_get(lua_State* L, int pos, int64* value)
		{
			if (lua_isstring(L, pos) || lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<uint64>
	{
		static int push_stack(lua_State* L, uint64 value)
		{
			lua_pushlstring(L, (const char*)&value, 8);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, uint64* value)
		{
			if(!lua_isnumber(L, pos) && !lua_isstring(L, pos))
            {
                luaL_error(L, "expected 'uint64', received %s", lua_typename(L, lua_type(L, pos)));
                return;
            }
			if (lua_isnumber(L, pos))
			{
				lua_Number n = luaL_checknumber(L, pos);
				*value = (int64)n;
				return;
			}
			size_t len = 0;
			const char* str = luaL_checklstring(L, pos, &len);
			if (len != 8)
			{
				luaL_error(L, "bad uint64 string length (8 expected, got %d)", int(len));
				return;
			}
			
			*value = *(uint64*)str;
		}
		static bool try_get(lua_State* L, int pos, uint64* value)
		{
			if (lua_isnumber(L, pos) || lua_isstring(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<lua_nil_t>
	{
		static int push_stack(lua_State* L, const lua_nil_t&)
		{
			lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, lua_nil_t* )
		{
		}
		static bool try_get(lua_State * L, int pos, lua_nil_t*)
		{
			return true;
		}
	};

    template<>
	struct lua_op_t<std::nullptr_t>
	{
		static int push_stack(lua_State* L, std::nullptr_t&)
		{
			lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, std::nullptr_t* )
		{
		}
		static bool try_get(lua_State * L, int pos, std::nullptr_t*)
		{
			return true;
		}
	};
    
	template<>
	struct lua_op_t<lua_bytes_t>
	{
		static int push_stack(lua_State* L, const lua_bytes_t& value)
		{
			if (!value.is_nil)
				lua_pushlstring(L, (const char*)(value.buff.c_str()), value.length());
			else
				lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, lua_bytes_t* value)
		{
			if (lua_isnoneornil(L, pos))
			{
				(*value).init(nullptr);
				return;
			}
            LUA_CHECK_PARAM(lua_isstring(L, pos), type::string, pos);
			size_t sz;
			(*value).init((unsigned char*)lua_tolstring(L, pos, &sz));
		}
		static bool try_get(lua_State * L, int pos, lua_bytes_t* value)
		{
			if (lua_isnoneornil(L, pos) || lua_isstring(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<char>
	{
		static int push_stack(lua_State* L, char value)
		{
			lua_pushnumber(L, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, char* value)
		{
            LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (char)luaL_checknumber(L, pos);
		}
		static bool try_get(lua_State * L, int pos, char* value)
		{
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<unsigned char>
	{
		static int push_stack(lua_State* L, unsigned char value)
		{
			lua_pushnumber(L, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, unsigned char* value)
		{
			LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (unsigned char)luaL_checknumber(L, pos);
		}
		static bool try_get(lua_State * L, int pos, unsigned char* value)
		{
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<signed char>
	{
		static int push_stack(lua_State* L, signed char value)
		{
			lua_pushnumber(L, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, signed char* value)
		{
			LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (signed char)luaL_checknumber(L, pos);
		}
		static bool try_get(lua_State * L, int pos, signed char* value)
		{
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};
	
	template<>
	struct lua_op_t<char*>
	{
		static int push_stack(lua_State* L, char* value)
		{
			if (value)
				lua_pushstring(L, value);
			else
				lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, char** value)
		{
			if (lua_isnoneornil(L, pos))
			{
				*value = nullptr;
				return;
			}
            LUA_CHECK_PARAM(lua_isstring(L, pos), type::string, pos);
			*value = (char*)luaL_checkstring(L, pos);
		}
		static bool try_get(lua_State * L, int pos, char** value)
		{
			if (lua_isnoneornil(L, pos) || lua_isstring(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<const char*>
	{
		static int push_stack(lua_State* L, const char* value)
		{
			if (value)
				lua_pushstring(L, value);
			else
				lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, const char** value)
		{
			if (lua_isnoneornil(L, pos))
			{
				*value = nullptr;
				return;
			}
            LUA_CHECK_PARAM(lua_isstring(L, pos), type::string, pos);
			*value = (const char*)luaL_checkstring(L, pos);
		}
		static bool try_get(lua_State * L, int pos, const char** value)
		{
			if (lua_isnoneornil(L, pos) || lua_isstring(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<unsigned char*>
	{
		static int push_stack(lua_State* L, unsigned char* value)
		{
			lua_pushlstring(L, (const char*)value, strlen((char*)value));
			return 1;
		}
		static void from_stack(lua_State* L, int pos, unsigned char** value)
		{
			if (lua_isnoneornil(L, pos))
			{
                *value = nullptr;
				return;
			}
            LUA_CHECK_PARAM(lua_isstring(L, pos), type::string, pos);
			size_t sz;
			(*value) = (unsigned char*)lua_tolstring(L, pos, &sz);
		}
		static bool try_get(lua_State * L, int pos, unsigned char** value)
		{
			if (lua_isnoneornil(L, pos) || lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<int N>
	struct lua_op_t<char[N]>
	{
		static int push_stack(lua_State* L, const char* value)
		{
			if (value)
				lua_pushstring(L, value);
			else
				lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, const char** value)
		{
			if (lua_isnoneornil(L, pos))
			{
				*value = nullptr;
				return;
			}
			LUA_CHECK_PARAM(lua_isstring(L, pos), type::string, pos);
			*value = (const char*)luaL_checkstring(L, pos);
		}
		static bool try_get(lua_State * L, int pos, const char** value)
		{
			if (lua_isnoneornil(L, pos) || lua_isstring(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<int N>
	struct lua_op_t<const char[N]>
	{
		static int push_stack(lua_State* L, const char* value)
		{
			if (value)
				lua_pushstring(L, value);
			else
				lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, const char** value)
		{
			if (lua_isnoneornil(L, pos))
			{
				*value = nullptr;
				return;
			}
			LUA_CHECK_PARAM(lua_isstring(L, pos), type::string, pos);
			*value = (const char*)luaL_checkstring(L, pos);
		}
		static bool try_get(lua_State * L, int pos, const char** value)
		{
			if (lua_isnoneornil(L, pos) || lua_isstring(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<bool>
	{
		static int push_stack(lua_State* L, bool value)
		{
			lua_pushboolean(L, value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, bool* value)
		{
            if(lua_isnoneornil(L, pos))
            {
                *value = false;
                return;
            }      
            LUA_CHECK_PARAM(lua_isnoneornil(L, pos) || lua_isboolean(L, pos), type::boolean, pos);
			*value = !!lua_toboolean(L, pos);
		}
		static bool try_get(lua_State * L, int pos, bool* value)
		{
			if (lua_isnoneornil(L, pos) || lua_isboolean(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<int>
	{
		static int push_stack(lua_State* L, int value)
		{
			lua_pushinteger(L, value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, int* value)
		{
			if (!lua_isnumber(L, pos) && lua_isstring(L, pos)) {
				int64 p;
				lua_op_t<int64>::from_stack(L, pos, &p);
				*value = (int)p;
				return;
			}
            LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (int)luaL_checkinteger(L, pos);
		}
		static bool try_get(lua_State * L, int pos, int* value)
		{
			if (!lua_isnumber(L, pos) && lua_isstring(L, pos)) {
				int64 p;
				bool b = lua_op_t<int64>::try_get(L, pos, &p);
				*value = (int)p;
				return b;
			}
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<unsigned int>
	{
		static int push_stack(lua_State* L, unsigned int value)
		{
			lua_pushinteger(L, value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, unsigned int* value)
		{
			if (!lua_isnumber(L, pos) && lua_isstring(L, pos)) {
				uint64 p;
				lua_op_t<uint64>::from_stack(L, pos, &p);
				*value = (unsigned int)p;
				return;
			}
            LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (unsigned int)luaL_checkinteger(L, pos);
		}
		static bool try_get(lua_State * L, int pos, unsigned int* value)
		{
			if (!lua_isnumber(L, pos) && lua_isstring(L, pos)) {
				uint64 p;
				bool b = lua_op_t<uint64>::try_get(L, pos, &p);
				*value = (unsigned int)p;
				return b;
			}
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<short>
	{
		static int push_stack(lua_State* L, short value)
		{
			lua_pushnumber(L, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, short* value)
		{
			LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (short)luaL_checknumber(L, pos);
		}
		static bool try_get(lua_State * L, int pos, short* value)
		{
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<unsigned short>
	{
		static int push_stack(lua_State* L, unsigned short value)
		{
			lua_pushnumber(L, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, unsigned short* value)
		{
			LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (unsigned short)luaL_checknumber(L, pos);
		}
		static bool try_get(lua_State * L, int pos, unsigned short* value)
		{
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<float>
	{
		static int push_stack(lua_State* L, float value)
		{
			lua_pushnumber(L, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, float* value)
		{
			LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (float)luaL_checknumber(L, pos);
		}
		static bool try_get(lua_State * L, int pos, float* value)
		{
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<double>
	{
		static int push_stack(lua_State* L, double value)
		{
			lua_pushnumber(L, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, double* value)
		{
			if (!lua_isnumber(L, pos) && lua_isstring(L, pos)) {
				int64 p;
				lua_op_t<int64>::from_stack(L, pos, &p);
				*value = (double)p;
				return;
			}
			LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (double)luaL_checknumber(L, pos);
		}
		static bool try_get(lua_State * L, int pos, double* value)
		{
			if (!lua_isnumber(L, pos) && lua_isstring(L, pos)) {
				int64 p;
				bool b = lua_op_t<int64>::try_get(L, pos, &p);
				*value = (double)p;
				return b;
			}
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<long>
	{
		static int push_stack(lua_State* L, long value)
		{
			lua_pushnumber(L, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, long* value)
		{
			if (!lua_isnumber(L, pos) && lua_isstring(L, pos)) {
				int64 p;
				lua_op_t<int64>::from_stack(L, pos, &p);
				*value = (long)p;
				return;
			}
			LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (long)luaL_checknumber(L, pos);
		}
		static bool try_get(lua_State * L, int pos, long* value)
		{
			if (!lua_isnumber(L, pos) && lua_isstring(L, pos)) {
				int64 p;
				bool b = lua_op_t<int64>::try_get(L, pos, &p);
				*value = (long)p;
				return b;
			}
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t<unsigned long>
	{
		static int push_stack(lua_State* L, unsigned long value)
		{
			lua_pushnumber(L, (lua_Number)value);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, unsigned long* value)
		{
			if (!lua_isnumber(L, pos) && lua_isstring(L, pos)) {
				uint64 p;
				lua_op_t<uint64>::from_stack(L, pos, &p);
				*value = (unsigned long)p;
				return;
			}
			LUA_CHECK_PARAM(lua_isnumber(L, pos), type::number, pos);
			*value = (unsigned long)luaL_checknumber(L, pos);
		}
		static bool try_get(lua_State * L, int pos, unsigned long* value)
		{
			if (!lua_isnumber(L, pos) && lua_isstring(L, pos)) {
				uint64 p;
				bool b = lua_op_t<uint64>::try_get(L, pos, &p);
				*value = (unsigned long)p;
				return b;
			}
			if (lua_isnumber(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};
    
	template<>
	struct lua_op_t<std::string>
	{
		static int push_stack(lua_State* L, const std::string& value)
		{
			lua_pushstring(L, value.c_str());
			return 1;
		}
		static void from_stack(lua_State* L, int pos, std::string* value)
		{
            LUA_CHECK_PARAM(lua_isstring(L, pos), type::string, pos);
			const char* str = luaL_checkstring(L, pos);
			*value = str;
		}
		static bool try_get(lua_State * L, int pos, std::string* value)
		{
			if (lua_isstring(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};
	
	template<>
	struct lua_op_t<const std::string>
	{
		static int push_stack(lua_State* L, const std::string& value)
		{
			return lua_op_t<std::string>::push_stack(L, value);
		}
		static void from_stack(lua_State* L, int pos, std::string* value)
		{
			lua_op_t<std::string>::from_stack(L, pos, value);
		}
		static bool try_get(lua_State * L, int pos, std::string* value)
		{
			return lua_op_t<std::string>::try_get(L, pos, value);
		}
	};

	template<>
	struct lua_op_t < void* >
	{
		static int push_stack(lua_State* L, void* value)
		{
			if (value)
				lua_pushlightuserdata(L, value);
			else
				lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, void** value)
		{
			if (lua_isnoneornil(L, pos))
			{
				*value = nullptr;
				return;
			}
            LUA_CHECK_PARAM(lua_isnoneornil(L, pos) || lua_islightuserdata(L, pos), type::lightuserdata, pos);
			*value = lua_touserdata(L, pos);
		}
		static bool try_get(lua_State * L, int pos, void** value)
		{
			if (lua_isnoneornil(L, pos) || lua_islightuserdata(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<>
	struct lua_op_t < lua_CFunction >
	{
		static int push_stack(lua_State* L, lua_CFunction value)
		{
			if (value)
				lua_pushcfunction(L, value);
			else
				lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, lua_CFunction* value)
		{
			if (lua_isnoneornil(L, pos))
			{
				*value = nullptr;
				return;
			}
            LUA_CHECK_PARAM(lua_isfunction(L, pos), type::function, pos);
			*value = (lua_CFunction)lua_tocfunction(L, pos);
		}
		static bool try_get(lua_State * L, int pos, lua_CFunction* value)
		{
			if (lua_isnoneornil(L, pos) || lua_isfunction(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};
	
	template<typename T>
	struct lua_op_t < std::vector<T> >
	{
		static int push_stack(lua_State* L, const std::vector<T>& value)
		{
			lua_newtable(L);
			typename std::vector<T>::const_iterator it = value.begin();
			for (int i = 1; it != value.end(); ++it, ++i)
			{
				lua_op_t<int>::push_stack(L, i);
				lua_op_t<T>::push_stack(L, *it);
				lua_settable(L, -3);
			}
			return 1;
		}
		static void from_stack(lua_State* L, int pos, std::vector<T>*& value)
		{
			if (lua_isnoneornil(L, pos))
			{
				return;
			}
            LUA_CHECK_PARAM(lua_istable(L, pos), type::table, pos);
			lua_pushnil(L);
			int real_pos = pos;
			if (pos < 0) real_pos = real_pos - 1;
			while (lua_next(L, real_pos) != 0)
			{
				T v = T();
				lua_op_t<T>::from_stack(L, -1, &v);
				value->push_back(v);
				lua_pop(L, 1);
			}
		}
		static bool try_get(lua_State * L, int pos, std::vector<T>*& value)
		{
			if (lua_isnoneornil(L, pos) || lua_istable(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<typename T>
	struct lua_op_t < std::list<T> >
	{
		static int push_stack(lua_State* L, const std::list<T>& value)
		{
			lua_newtable(L);
			typename std::list<T>::const_iterator it = value.begin();
			for (int i = 1; it != value.end(); ++it, ++i)
			{
				lua_op_t<int>::push_stack(L, i);
				lua_op_t<T>::push_stack(L, *it);
				lua_settable(L, -3);
			}
			return 1;
		}
		static void from_stack(lua_State* L, int pos, std::list<T>*& value)
		{
			if (lua_isnoneornil(L, pos))
			{
				return;
			}
			LUA_CHECK_PARAM(lua_istable(L, pos), type::table, pos);
			lua_pushnil(L);
			int real_pos = pos;
			if (pos < 0) real_pos = real_pos - 1;

			while (lua_next(L, real_pos) != 0)
			{
				T v = T();
				lua_op_t<T>::from_stack(L, -1, &v);
				value->push_back(v);
				lua_pop(L, 1);
			}
		}
		static bool try_get(lua_State * L, int pos, std::list<T>*& value)
		{
			if (lua_isnoneornil(L, pos) || lua_istable(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};
	
	template<typename T>
	struct lua_op_t < std::set<T> >
	{
		static int push_stack(lua_State* L, const std::set<T>& value)
		{
			lua_newtable(L);
			typename std::set<T>::const_iterator it = value.begin();
			for (int i = 1; it != value.end(); ++it, ++i)
			{
				lua_op_t<int>::push_stack(L, i);
				lua_op_t<T>::push_stack(L, *it);
				lua_settable(L, -3);
			}
			return 1;
		}

		static void from_stack(lua_State* L, int pos, std::set<T>*& value)
		{
			if (lua_isnoneornil(L, pos))
			{
				return;
			}
			LUA_CHECK_PARAM(lua_istable(L, pos), type::table, pos);
			lua_pushnil(L);
			int real_pos = pos;
			if (pos < 0) real_pos = real_pos - 1;
			while (lua_next(L, real_pos) != 0)
			{
				T val = T();
				lua_op_t<T>::from_stack(L, -1, &val);
				value->insert(val);
				lua_pop(L, 1);
			}
		}
		static bool try_get(lua_State * L, int pos, std::set<T>*& value)
		{
			if (lua_isnoneornil(L, pos) || lua_istable(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};

	template<typename K, typename V>
	struct lua_op_t < std::map<K, V> >
	{
		static int push_stack(lua_State* L, const std::map<K, V>& value)
		{
			lua_newtable(L);
			typename std::map<K, V>::const_iterator it = value.begin();
			for (; it != value.end(); ++it)
			{
				lua_op_t<K>::push_stack(L, it->first);
				lua_op_t<V>::push_stack(L, it->second);
				lua_settable(L, -3);
			}
			return 1;
		}

		static void from_stack(lua_State* L, int pos, std::map<K, V>*& value)
		{
			if (lua_isnoneornil(L, pos))
			{
				return;
			}
			LUA_CHECK_PARAM(lua_istable(L, pos), type::table, pos);
			lua_pushnil(L);
			int real_pos = pos;
			if (pos < 0) real_pos = real_pos - 1;
			while (lua_next(L, real_pos) != 0)
			{
				K key = K();
				V val = V();
				lua_op_t<K>::from_stack(L, -2, &key);
				lua_op_t<V>::from_stack(L, -1, &val);
				value->insert(make_pair(key, val));
				lua_pop(L, 1);
			}
		}
		static bool try_get(lua_State * L, int pos, std::map<K, V>*& value)
		{
			if (lua_isnoneornil(L, pos) || lua_istable(L, pos))
			{
				from_stack(L, pos, value);
				return true;
			}
			else
				return false;
		}
	};
}

//push
namespace lua 
{
    inline int push(lua_State* l)
	{
		return 0;
	}

	template<typename T>
	inline int push(lua_State* L, const T& value)
	{
		return lua_op_t<T>::push_stack(L, value);
	}

	template <typename T, typename ... Args>
	inline int push(lua_State* L, const T& value, const Args &... args)
	{
		return push(L, value) + push(L, args...);
	}

	template<typename T>
	inline void get(lua_State* L, int pos, T* value)
	{
		lua_op_t<T>::from_stack(L, pos, value);
	}

	template <typename T, typename ... Args>
	inline void get(lua_State* L, int pos, T* value, Args ... args)
	{
		get(L, pos, value);
		get(L, pos + 1, args...);
	}

	template<typename T>
	inline void pop(lua_State* L, T* value)
	{
		get(L, -1, value);
		lua_pop(L, -1);
	}

	template <typename T, typename ... Args>
	inline void pop(lua_State* L, T* value, Args ... args)
	{
		pop(L, value);
		pop(L, args...);
	}

	template<typename T>
	inline bool tryget(lua_State* L, int pos, T* value)
	{
		return lua_op_t<T>::try_get(L, pos, value);
	}
}

//utils 
namespace lua
{
	inline int errorFun_traceback(lua_State* L)
	{
		if (!lua_isstring(L, 1))
			return 1;
		lua_checkstack(L, 3);
		lua_getglobal(L, "debug"); //msg, debug
		lua_getfield(L, -1, "traceback"); //msg, debug,traceback
		lua_pushvalue(L, 1); //msg, debug,traceback, msg
		lua_pushnumber(L, 1); //msg, debug,traceback, msg, level
		lua_call(L, 2, 1);//msg, debug, tracebackstring

		return 1;
	}

	inline bool pcall(lua_State* L, int nArgs, int nResults, int nErrorsRef = LUA_NOREF)
	{
		int oldTop = lua_gettop(L) - nArgs - 1;
		int errorFuncIndex = 0;
		if (nErrorsRef != LUA_NOREF && nErrorsRef != LUA_REFNIL)
		{
			lua_rawgeti(L, LUA_REGISTRYINDEX, nErrorsRef);
			lua_insert(L, oldTop + 1);
			errorFuncIndex = oldTop + 1;
		}
		if (lua_pcall(L, nArgs, nResults, errorFuncIndex) == 0)
		{
			lua_remove(L, oldTop + 1);	//pop errorFunc
			return true;
		}
		else
		{
			lua_remove(L, oldTop + 1);	//pop errorFunc
			return false;
		}
	}

	template <typename ... Args>
	inline bool callFunction(lua_State* L, const char* method, const Args&... args)
	{
		lua_pushstdcallcfunction(L, errorFun_traceback);
		int errorfunc = luaL_ref(L, LUA_REGISTRYINDEX);

		lua_getglobal(L, method);
		if (!lua_isfunction(L, -1))
		{
			lua_pop(L, 1);
			luaL_unref(L, LUA_REGISTRYINDEX, errorfunc);
			luaL_error(L, "Error: function %s not found.\n", method);
			return false;
		}

		bool success = pcall(L, push(L, args...), -1, errorfunc);
		luaL_unref(l, LUA_REGISTRYINDEX, errorfunc);
		return success;
	}

	template <typename ... Args>
	inline bool callFunction(lua_State* L, int refFunc, const Args&... args)
	{
		lua_pushstdcallcfunction(L, errorFun_traceback);
		int errorfunc = luaL_ref(L, LUA_REGISTRYINDEX);

		lua_rawgeti(L, LUA_REGISTRYINDEX, refFunc);  //function
		if (!lua_isfunction(L, -1))
		{
			lua_pop(L, 1);
			luaL_unref(L, LUA_REGISTRYINDEX, errorfunc);
			luaL_error(L, "Error: registryindex %d is not a function\n", refFunc);
			return false;
		}

		bool success = pcall(L, push(L, args...), -1, errorfunc);
		luaL_unref(l, LUA_REGISTRYINDEX, errorfunc);
		return success;
	}

	template <typename ... Args>
	inline bool callTableFunction(lua_State* L, const char* module, const char* method, const Args&... args)
	{
		lua_pushstdcallcfunction(L, errorFun_traceback);
		int errorfunc = luaL_ref(L, LUA_REGISTRYINDEX);

		lua_getglobal(L, module);	//--> t
		if (!lua_istable(L, -1))
		{
			lua_pop(L, 1);
			luaL_unref(L, LUA_REGISTRYINDEX, errorfunc);
			luaL_error(L, "Error: module %s not found.\n", module);
			return false;
		}

		lua_getfield(L, -1, method);//t,func
		if (lua_isnoneornil(L, -1))
		{
			lua_pop(L, 1);
			luaL_unref(L, LUA_REGISTRYINDEX, errorfunc);
			luaL_error(L, "Error:  %s@%s not found.\n", method, module);
			return false;
		}

		bool success = pcall(L, push(L, args...), -1, errorfunc);
		luaL_unref(l, LUA_REGISTRYINDEX, errorfunc);
		return success;
	}

	template <typename ... Args>
	inline bool callTableFunction(lua_State* L, int refTable, const char* method, const Args&... args)
	{
		lua_pushstdcallcfunction(L, errorFun_traceback);
		int errorfunc = luaL_ref(L, LUA_REGISTRYINDEX);

		lua_rawgeti(L, LUA_REGISTRYINDEX, refTable); //t
		lua_getfield(L, -1, method);//t,func
		if (lua_isnoneornil(L, -1))
		{
			lua_pop(L, 1);
			luaL_unref(L, LUA_REGISTRYINDEX, errorfunc);
			luaL_error(L, "Error:  %s not found.\n", method);
			return false;
		}

		bool success = pcall(L, push(L, args...), -1, errorfunc);
		luaL_unref(L, LUA_REGISTRYINDEX, errorfunc);
		return success;
	}

	template <typename ... Args>
	inline bool runFile(lua_State* L, const char* filename, const Args&... args)
	{
		lua_pushstdcallcfunction(L, errorFun_traceback);
		int errorfunc = luaL_ref(L, LUA_REGISTRYINDEX);

		if (0 != luaL_loadfile(L, filename))
		{
			lua_pop(L, 1);
			luaL_unref(L, LUA_REGISTRYINDEX, errorfunc);
			luaL_error(L, "Error: loadfile %s error.\n", filename);
			return false;
		}

		bool success = pcall(L, push(L, args...), -1, errorfunc);
		luaL_unref(L, LUA_REGISTRYINDEX, errorfunc);
		return success;
	}

	template <typename ... Args>
	inline bool runString(lua_State* L, const char* buff, const Args&... args)
	{
		lua_pushstdcallcfunction(L, errorFun_traceback);
		int errorfunc = luaL_ref(L, LUA_REGISTRYINDEX);

		if (0 != luaL_loadstring(L, buff))
		{
			lua_pop(L, 1);
			luaL_unref(L, LUA_REGISTRYINDEX, errorfunc);
			luaL_error(L, "Error: loadstring error.\n");
			return false;
		}
		bool success = pcall(L, push(L, args...), -1, errorfunc);
		luaL_unref(L, LUA_REGISTRYINDEX, errorfunc);
		return success;
	}
}

//reference
namespace lua 
{
    class reference 
    {
    protected:
        lua_State* m_L = nullptr; // non-owning
        int m_ref = LUA_NOREF;

        int copy() const {
            getref();
            return luaL_ref(m_L, LUA_REGISTRYINDEX);
        }

    public:
        reference() noexcept = default;
        reference(lua_State* L, bool doref = false): m_L(L) {
            if (isref()) {
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
            }
            m_ref = doref ? luaL_ref(m_L, LUA_REGISTRYINDEX) : LUA_NOREF;
        }
        reference(lua_State* L, int index): m_L(L) {
            if (isref()) {
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
            }
            lua_pushvalue(m_L, index);
            m_ref = luaL_ref(m_L, LUA_REGISTRYINDEX);
        }
        reference(lua_State *L, const char *name) : m_L(L) {
            assert(name && "reference name valid");
            if (isref()) {
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
            }
            lua_getglobal(m_L, name);
            m_ref = luaL_ref(m_L, LUA_REGISTRYINDEX);
        }

        virtual ~reference() noexcept {
            if (isref()) {
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
            }
            m_ref = LUA_NOREF;
        }

        reference(reference&& o) noexcept {
            if (isref()) {
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
            }
            m_L = o.m_L;
            m_ref = o.m_ref;

            o.m_L = nullptr;
            o.m_ref = LUA_NOREF;
        }

        reference& operator=(reference&& o) noexcept {
            this->~reference(), new(this) reference(std::move(o));
            return *this;
        }

        reference(const reference& o) noexcept {
            m_L = o.m_L;
            m_ref = o.copy();
        }

        reference& operator=(const reference& o) noexcept {
            m_L = o.m_L;
            m_ref = o.copy();
            return *this;
        }

        bool isref() const {
            return m_ref != LUA_NOREF && m_ref != LUA_REFNIL;
        }

        void unref() {
            *this = reference();
        }

        void getref() const noexcept {
            lua_rawgeti(m_L, LUA_REGISTRYINDEX, m_ref);
        }

        int index() const noexcept {
            return m_ref;
        }
        
        lua_State* getL() const noexcept {
            return m_L;
        }

        operator lua_State* ()  const noexcept { 
            return getL(); 
        }

        int get_type() {
            getref();
            int result = lua_type(m_L, -1);
            lua_pop(m_L, 1);
            return result;
        }

        reference clone() const {
            getref();
            return reference(m_L, true);
        }

    };


    template<>
	struct lua_op_t<reference>
	{
		static int push_stack(lua_State* L, const reference& value)
		{
			value.isref() ? value.getref() : lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, reference* value)
		{
            assert(value);
            new(value) reference(L, pos);
		}
		static bool try_get(lua_State * L, int pos, reference* value)
		{
			from_stack(L, pos, value);
			return true;
		}
	};
}

//function
namespace lua 
{
    class function : public reference
    {
    protected:
    public:
        virtual ~function() noexcept {}
        function() = default;
        function(lua_State* L, bool doref = false): reference(L, doref) {
            if(doref) {
                LUA_CHECK_PARAM(get_type()== (int)type::function, type::function, -1);
            }
        }
        function(lua_State* L, int index): reference(L, index) {
            LUA_CHECK_PARAM(lua_isfunction(L, index), type::function, index);
        }
        function(lua_State* L, const char *name) : reference(L, name) {
            int tp = get_type();
            if(tp != (int)type::function) {
                luaL_error(m_L, "expected 'function', received %s", lua_typename(L, tp));
            }
        }
        
        function(function&& o) noexcept {
            if (isref()) {
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
            }
            m_L = o.m_L;
            m_ref = o.m_ref;

            o.m_L = nullptr;
            o.m_ref = LUA_NOREF;
        }

        function& operator=(function&& o) noexcept {
            this->~function(), new(this) function(std::move(o));
            return *this;
        }

        function(const function& o) noexcept {
            m_L = o.m_L;
            m_ref = o.copy();
        }

        function& operator=(const function& o) noexcept {
            m_L = o.m_L;
            m_ref = o.copy();
            return *this;
        }
    public:
        template<typename... Args>
        bool call(const Args &... args) const {
			return callFunction(m_L, m_ref, args...);
        }
    };

    template<>
	struct lua_op_t<function>
	{
		static int push_stack(lua_State* L, const function& value)
		{
			value.isref() ? value.getref() : lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, function* value)
		{
            assert(value);
            new(value) function(L, pos);
		}
		static bool try_get(lua_State * L, int pos, function* value)
		{
			from_stack(L, pos, value);
			return true;
		}
	};

}

//table
namespace lua
{
    class table : public reference
    {
    public:
        virtual ~table() noexcept {}
        table() = default;
        table(lua_State* L, bool doref = false): reference(L, doref) {
            if(doref) {
                LUA_CHECK_PARAM(get_type() == (int)type::table, type::table, -1);
            }
        }
        table(lua_State* L, int index): reference(L, index) {
            LUA_CHECK_PARAM(lua_istable(L, index), type::table, index);
        }
        table(lua_State* L, const char *name) : reference(L, false) {
            if(!name){
                lua_newtable(m_L);
                new(this) table(m_L, true);
            }
            else {
                lua_getglobal(m_L, name);
                if (lua_isnil(m_L, -1)) {
                    lua_pop(m_L, 1);
                    lua_newtable(m_L);
                    lua_pushvalue(m_L, -1);
                    lua_setglobal(m_L, name);
                }
                new(this) table(m_L, true);
            }
        }

        table(table&& o) noexcept {
            if (isref()) {
                luaL_unref(m_L, LUA_REGISTRYINDEX, m_ref);
            }
            m_L = o.m_L;
            m_ref = o.m_ref;

            o.m_L = nullptr;
            o.m_ref = LUA_NOREF;
        }

        table& operator=(table&& o) noexcept {
            this->~table(), new(this) table(std::move(o));
            return *this;
        }

        table(const table& o) noexcept {
            m_L = o.m_L;
            m_ref = o.copy();
        }

        table& operator=(const table& o) noexcept {
            m_L = o.m_L;
            m_ref = o.copy();
            return *this;
        }

        size_t len() const { 
            getref();
#ifdef LUA5_2
			int len = (int)lua_rawlen(m_L, -1);
#else
			int len = (int)lua_objlen(m_L, -1);
#endif
            lua_pop(m_L, 1);
            return len;
        }

        template<typename T>
		inline void get(const char* field, T* value) const
		{
			int oldTop = lua_gettop(m_L);
			getref(); //t
            lua::push(m_L, field);
			lua_gettable(m_L, -2); //value
            lua::get(m_L, -1, value);
			lua_settop(m_L, oldTop);
		}
		template<typename T>
		inline void get(int index, T* value) const
		{
			int oldTop = lua_gettop(m_L);
			getref(); //t
			lua_rawgeti(m_L, -1, index);
            lua::get(m_L, -1, value);
			lua_settop(m_L, oldTop);
		}
    
        template<typename T>
		inline bool tryget(const char* field, T* value) const
		{
			int oldTop = lua_gettop(m_L);
			getref(); //t
			lua::push(m_L, field);
			lua_gettable(m_L, -2); //value
			bool b = lua::tryget(m_L, -1, value); //value
			lua_settop(m_L, oldTop);
            return b;
		}
		template<typename T>
		inline bool tryget(int index, T* value) const
		{
			int oldTop = lua_gettop(m_L);
			getref(); //t
			lua_rawgeti(m_L, -1, index);
            bool b = lua::tryget(m_L, -1, value); //value
			lua_settop(m_L, oldTop);
            return b;
		}

        template<typename T>
		inline T get(const char* field) const
		{
			T v = T();
			get(field, &v);
			return v;
		}
		template<typename T>
		inline T get(int index) const
		{
			T v = T();
			get(index, &v);
			return v;
		}

        template<typename T>
		inline void set(const char* field, const T& value)
		{
			int oldTop = lua_gettop(m_L);
			getref(); //t
			lua::push(m_L, field);
            lua::push(m_L, value);
			lua_settable(m_L,-3);
			lua_settop(m_L, oldTop);
		}
		template<typename T>
		inline void set(int index, const T& value)
		{
            int oldTop = lua_gettop(m_L);
			getref(); //t
            lua::push(m_L, value);
			lua_rawseti(m_L, -2, index);
			lua_settop(m_L, oldTop);
		}
    
        inline bool has(const char* field)
		{
			int oldTop = lua_gettop(m_L);
			getref(); //t
			lua::push(m_L, field);
			lua_gettable(m_L, -2);
			bool b = lua_isnil(m_L, -1);
			lua_settop(m_L, oldTop);
			return !b;
		}

		template <typename ... Args>
		inline bool call(const char* method, const Args&... args)
		{
			return callTableFunction(m_L, m_ref, method, args ...);
		}
    };

    template<>
	struct lua_op_t<table>
	{
		static int push_stack(lua_State* L, const table& value)
		{
			value.isref() ? value.getref() : lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, table* value)
		{
            assert(value);
            new(value) table(L, pos);
		}
		static bool try_get(lua_State * L, int pos, table* value)
		{
			from_stack(L, pos, value);
			return true;
		}
	};
    
}

//functor
namespace lua
{
    ////////////////////////////////////////////////////////////
	////
	template < typename R, typename T, typename ...Args >
	struct object_wrapper_t
	{
		using return_t = R;
		using handler_t = R(T::*)(Args...);

		typedef std::tuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...> params_t;
		typedef std::tuple<Args...> tuple_t;

		T *obj_;
		handler_t handler_;

		object_wrapper_t(T *obj, handler_t handler)
			: obj_(obj)
			, handler_(handler)
		{}

		R operator()(const Args &...param) const
		{
			return (*obj_.*handler_)(param...);
		}
	};

	template < typename R, typename T, typename ...Args >
	struct lambda_wrapper_t
	{
		using return_t = R;
		using handler_t = R(T::*)(Args...);

		typedef std::tuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...> params_t;
		typedef std::tuple<Args...> tuple_t;

		T *obj_;

		lambda_wrapper_t(T *obj)
			: obj_(obj)
		{}

		return_t operator()(const Args &...args) const
		{
			return (*obj_)(args...);
		}
	};

	template < typename R, typename ...Args >
	struct function_wrapper_t
	{
		using return_t = R;
		using handler_t = R(*)(Args...);

		typedef std::tuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...> params_t;
		typedef std::tuple<Args...> tuple_t;

		handler_t handler_;

		function_wrapper_t(handler_t handler)
			: handler_(handler)
		{}

		R operator()(const Args &...param) const
		{
			return (*handler_)(param...);
		}
	};

	template < typename T, typename ...Args >
	struct constructor_wrapper_t
	{
		typedef void return_t;
		T **pobj_;

		typedef std::tuple<typename std::remove_cv<typename std::remove_reference<Args>::type>::type...> params_t;
		typedef std::tuple<Args...> tuple_t;

		constructor_wrapper_t(T **pobj)
			:pobj_(pobj)
		{}

		return_t operator()(const Args &...param) const
		{
			*pobj_ = new T(param...);
			//*pbbj_ = malloc(sizeof(T));
			//::new (*pbbj_) T(param...);
		}
	};
	////////////////////////////////////////////////////////////
	////
	template < typename R, typename T, typename ...Args >
	object_wrapper_t<R, T, Args...> make_obj(T *obj, R(T::*handler)(Args...))
	{
		return object_wrapper_t<R, T, Args...>(obj, handler);
	}

	template < typename R, typename T, typename ...Args >
	lambda_wrapper_t<R, T, Args...> make_obj(T *obj)
	{
		return lambda_wrapper_t<R, T, Args...>(obj);
	}

	template < typename R, typename ...Args >
	function_wrapper_t<R, Args...> make_obj(R(*handler)(Args...))
	{
		return function_wrapper_t<R, Args...>(handler);
	}

	template < typename T, typename ...Args >
	constructor_wrapper_t<T, Args...> make_obj(T** ppobj, const std::tuple<Args...> &)
	{
		return constructor_wrapper_t<T, Args...>(ppobj);
	}

    template < typename R, typename ...Args >
	struct free_function_t
	{
		const char *name_;

		typedef R(*function_t)(Args...);
		function_t function_;

		free_function_t(const char *name, function_t func)
			: name_(name)
			, function_(func)
		{}

		free_function_t &operator=(const free_function_t &) = delete;
	};
	
	template < typename R, typename V >
	struct field_function_t
	{
		const char *name_;
		typedef V(R::*field_t);
		field_t field_;
		field_function_t(const char *name, V R::*val)
			: name_(name)
			, field_(val)
		{}

		field_function_t &operator=(const field_function_t &) = delete;
	};

	template < typename V>
	struct staticfield_function_t
	{
		const char *name_;
		typedef V* field_t;
		field_t field_;
		staticfield_function_t(const char *name, V *val)
			: name_(name)
			, field_(val)
		{}

		staticfield_function_t &operator=(const staticfield_function_t &) = delete;
	};

	template < typename R, typename T, typename ...Args >
	struct class_function_t
	{
		const char *name_;

		typedef R(T::*function_t)(Args...);
		typedef R(T::*const_function_t)(Args...) const;
		typedef R(T::*volatile_function_t)(Args...) volatile;
		T *obj_;
		function_t function_;

		class_function_t(const char *name, T *obj, function_t func)
			: name_(name)
			, obj_(obj)
			, function_(func)
		{}
		class_function_t(const char *name, T *obj, const_function_t func)
			: name_(name)
			, obj_(obj)
			, function_(reinterpret_cast<function_t>(func))
		{}
		class_function_t(const char *name, T *obj, volatile_function_t func)
			: name_(name)
			, obj_(obj)
			, function_(reinterpret_cast<function_t>(func))
		{}

		class_function_t &operator=(const class_function_t &) = delete;
	};
	
	template < typename ...Args >
	struct lambda_function_t;

	template < typename HandlerT, typename R, typename ...Args >
	struct lambda_function_t<HandlerT, R, std::tuple<Args...>>
	{
		using function_t = R(*)(Args...);
		using this_t = lambda_function_t<HandlerT, R, std::tuple<Args...>>;

		const char *name_;
		HandlerT obj_;

		lambda_function_t(const char *name, HandlerT &&handler)
			: name_(name)
			, obj_(std::forward<HandlerT>(handler))
		{}

		lambda_function_t &operator=(const lambda_function_t &) = delete;

		R on_handler(Args &&...args)
		{
			return HandlerT(std::forward<Args>(args)...);
		}
	};

	template<typename T>
	struct function_traits;

	// 普通函数
	template<typename ReturnType, typename... Args>
	struct function_traits<ReturnType(Args...)>
	{
		enum { arity = sizeof...(Args) };
		using return_type = ReturnType;
		using function_type = ReturnType(Args...);
		using stl_function_type = std::function<function_type>;
		using pointer = ReturnType(*)(Args...);

		template<size_t I>
		struct args
		{
			static_assert(I < arity, "index is out of range, index must less than sizeof Args");
			using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
		};

		using tuple_type = std::tuple<std::remove_cv_t<std::remove_reference_t<Args>>...>;
		using bare_tuple_type = std::tuple<std::remove_const_t<std::remove_reference_t<Args>>...>;
	};

	// 函数指针
	template<typename ReturnType, typename... Args>
	struct function_traits<ReturnType(*)(Args...)> : function_traits<ReturnType(Args...)> {};

	// std::function
	template<typename ReturnType, typename... Args>
	struct function_traits<std::function<ReturnType(Args...)>> : function_traits<ReturnType(Args...)> {};

	// 成员函数
#define FUNCTION_TRAITS(...)\
template <typename ReturnType, typename ClassType, typename... Args>\
struct function_traits<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits<ReturnType(Args...)>{};\

	FUNCTION_TRAITS()
	FUNCTION_TRAITS(const)
	FUNCTION_TRAITS(volatile)
	FUNCTION_TRAITS(const volatile)

	//// 函数对象
	//template<typename Callable>
	//struct function_traits : function_traits<decltype(&Callable::operator())> {};

	template<typename Function>
	typename function_traits<Function>::stl_function_type to_function(const Function& lambda)
	{
		return static_cast<typename function_traits<Function>::stl_function_type>(lambda);
	}

	template<typename Function>
	typename function_traits<Function>::stl_function_type to_function(Function&& lambda)
	{
		return static_cast<typename function_traits<Function>::stl_function_type>(std::forward<Function>(lambda));
	}

	template<typename Function>
	typename function_traits<Function>::pointer to_function_pointer(const Function& lambda)
	{
		return static_cast<typename function_traits<Function>::pointer>(lambda);
	}
	
	////////////////////////////////////////////////////////////
	////
	template < typename ...Args >
	struct constructor_t {};

	struct destructor_t {};

	template < typename ...Args >
	inline constructor_t<Args...> constructor()
	{
		return constructor_t<Args...>();
	}

	inline destructor_t destructor()
	{
		return destructor_t();
	}

    template < typename R, typename ...Args >
	inline free_function_t<R, Args...> def(const char *name, R(*func)(Args...))
	{
		return free_function_t<R, Args...>(name, func);
	}

	//template < typename HandlerT >
	//inline auto def(const char *name, HandlerT &&handler)->
	// 	typename lambda_function_t<HandlerT, typename function_traits<HandlerT>::result_type, typename function_traits<HandlerT>::tuple_type>
	//{
	// 	typedef typename function_traits<HandlerT>::tuple_type	tuple_t;
	// 	typedef typename function_traits<HandlerT>::result_type	R;

	// 	return lambda_function_t<HandlerT, R, tuple_t>(name, std::forward<HandlerT>(handler));
	//}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, T *obj, R(T::*func)(Args...))
	{
		return class_function_t<R, T, Args...>(name, obj, func);
	}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, T *obj, R(T::*func)(Args...) const)
	{
		return class_function_t<R, T, Args...>(name, obj, func);
	}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, T *obj, R(T::*func)(Args...) volatile)
	{
		return class_function_t<R, T, Args...>(name, obj, func);
	}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, R(T::*func)(Args...))
	{
		return class_function_t<R, T, Args...>(name, nullptr, func);
	}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, R(T::*func)(Args...) const)
	{
		return class_function_t<R, T, Args...>(name, nullptr, func);
	}

	template < typename R, typename T, typename ...Args >
	inline class_function_t<R, T, Args...> def(const char *name, R(T::*func)(Args...) volatile)
	{
		return class_function_t<R, T, Args...>(name, nullptr, func);
	}

	template < typename R, typename V>
	inline field_function_t<R, V> def(const char* name, V R::*val)
	{
		return field_function_t<R, V>(name, val);
	}
	template <typename V>
	inline staticfield_function_t<V> def(const char* name, V *val)
	{
		return staticfield_function_t<V>(name, val);
	}

    struct lua_function_t
	{
		using function_t = int(*)(lua_State*);
		const char *name_;
		function_t func_;

		lua_function_t(const char *name, const function_t& func)
			: name_(name)
			, func_(func)
		{}
		lua_function_t &operator=(const lua_function_t &) = delete;
	};

	inline lua_function_t def(const char *name, int(*func)(lua_State*))
	{
		return lua_function_t(name, func);
	}


    ////////////////////////////////////////////////////////////
	////
	template<int ...> struct seq {};
	template<int N, int ...S> struct gens : gens<N - 1, N - 1, S...> {};
	template<int ...S> struct gens<0, S...> { typedef seq<S...> type; };
	template<int N> using seq_t = typename gens<N>::type;

	struct caller_t
	{
		template<typename T, typename ...Args, int ...S>
		static typename T::return_t call(const T &val, std::tuple<Args...> &args, seq<S...>)
		{
			return val(std::get<S>(args)...);
		}
	};

    ////////////////////////////////////////////////////////////
	////
	template < int N >
	struct args_parser_t
	{
		template < typename ...Args >
		static void parse(lua_State* L, int index, std::tuple<Args...> &args)
		{
			typedef std::tuple<Args...> tuple_t;
			typedef typename std::tuple_element<N, tuple_t>::type type;
			type v;
			lua::get(L, index, &v);
			std::get<N>(args) = std::move(v);
			args_parser_t<N - 1>::parse(L, index - 1, args);
		}
	};

	template < >
	struct args_parser_t<0>
	{
		template < typename ...Args >
		static void parse(lua_State* L, int index, std::tuple<Args...> &args)
		{
			typedef std::tuple<Args...> tuple_t;
			typedef typename std::tuple_element<0, tuple_t>::type type;
			type v;
			lua::get(L, index, &v);
			std::get<0>(args) = std::move(v);
		}

		static void parse(lua_State*, int, std::tuple<> &)
		{
		}
	};

    ////////////////////////////////////////////////////////////
	////
	template < typename TupleT, int N >
	struct tuple_helper_t
	{
		typedef TupleT tuple_t;
		typedef typename std::tuple_element<N, tuple_t>::type value_t;

		static void from(lua_State* L, int index, tuple_t &tuple_val)
		{
			value_t val;
			lua::get(L, index, &val);
			std::get<N>(tuple_val) = std::move(val);

			tuple_helper_t<tuple_t, N - 1>::from(L, index - 1, tuple_val);
		}

		static void push(lua_State* L, const tuple_t &val)
		{
			lua::push(L, std::get<N>(val));

			tuple_helper_t<tuple_t, N - 1>::to(L, val);
		}
	};
	
	template < typename TupleT >
	struct tuple_helper_t<TupleT, 0>
	{
		typedef TupleT tuple_t;
		typedef typename std::tuple_element<0, tuple_t>::type value_t;


		static void from(lua_State* L, int index, tuple_t &tuple_val)
		{
			value_t val;
			lua::get(L, index, &val);
			std::get<0>(tuple_val) = std::move(val);
		}

		static void push(lua_State* L, const tuple_t &val)
		{
			lua::push(L, std::get<0>(val));
		}
	};
	
	/*
    template < typename ...Args >
	struct lua_op_t< std::tuple<Args...> >
	{
		typedef std::tuple<Args...> tuple_t;

		static tuple_t from_stack(lua_State* L, int index)
		{
			tuple_t val;
			tuple_helper_t<tuple_t, std::tuple_size<tuple_t>::value - 1>::from(L, -1, val);

			return val;
		}

		static int push_stack(lua_State* L, const tuple_t &val)
		{
			tuple_helper_t<tuple_t, std::tuple_size<tuple_t>::value - 1>::push(L, val);

			return std::tuple_size<tuple_t>::value;
		}
	};
	*/

    ////////////////////////////////////////////////////////////
	////
	template < typename T >
	typename T::return_t call_impl(lua_State* L, const T &obj, int offset)
	{
		typedef typename T::params_t args_t;
		typedef typename T::tuple_t tuple_t;

		enum
		{
			TUPLE_SIZE = std::tuple_size<args_t>::value == 0 ? 0 : std::tuple_size<args_t>::value - 1
		};
		args_t args;
		args_parser_t<TUPLE_SIZE>::parse(L, std::tuple_size<args_t>::value + offset, args);

		return caller_t::call(obj, args, seq_t<std::tuple_size<args_t>::value>());
	}

	template < typename T, typename ...Args >
	inline void call(lua_State* L, T** ppobj, const std::tuple<Args...> &args)
	{
		call_impl(L, make_obj(ppobj, args), 1);
	}

	template < typename R, typename T, typename ...Args >
	int call(lua_State* L, T *obj, R(T::*handler)(Args...), int offset = 0,
		typename std::enable_if<!std::is_same<R, void>::value>::type * = nullptr)
	{
		using result_t = typename std::remove_cv<R>::type;
		return lua::push(L, call_impl(L, make_obj(obj, handler), offset));
	}

	template < typename R, typename T, typename ...Args >
	int call(lua_State* L, T *obj, R(T::*handler)(Args...), int offset = 0,
		typename std::enable_if<std::is_same<R, void>::value>::type * = nullptr)
	{
		call_impl(L, make_obj(obj, handler), offset);
		return 0;
	}

	template < typename R, typename ...Args >
	int call(lua_State* L, R(*handler)(Args...),
		typename std::enable_if<!std::is_same<R, void>::value>::type * = nullptr)
	{
		using result_t = typename std::remove_cv<R>::type;
		return lua::push(L, call_impl(L, make_obj(handler), 0));
	}

	template < typename R, typename ...Args >
	int call(lua_State* L, R(*handler)(Args...),
		typename std::enable_if<std::is_same<R, void>::value>::type * = nullptr)
	{
		call_impl(L, make_obj(handler), 0);
		return 0;
	}


	template < typename HandlerT, typename R, typename ...Args >
	int call(lua_State* L, HandlerT *handler,
		typename std::enable_if<!std::is_same<R, void>::value>::type * = nullptr)
	{
		using result_t = typename std::remove_cv<R>::type;
		return lua_op_t<result_t>::push_stack(L, call_impl(L, make_obj<R, HandlerT, Args...>(handler), 0));
	}

	template < typename HandlerT, typename R, typename ...Args >
	int call(lua_State* L, HandlerT *handler,
		typename std::enable_if<std::is_same<R, void>::value>::type * = nullptr)
	{
		call_impl(L, make_obj<R, HandlerT, Args...>(handler), 0);
		return 0;
	}
}

//traits
namespace lua
{
    template<typename A>
    struct base_type { typedef A type; };
    template<typename A>
    struct base_type<A*> { typedef A type; };
    template<typename A>
    struct base_type<A&> { typedef A type; };

    template<bool C, typename A, typename B> struct if_ {};
    template<typename A, typename B>		struct if_<true, A, B> { typedef A type; };
    template<typename A, typename B>		struct if_<false, A, B> { typedef B type; };

    template<typename A>
    struct is_ptr { static const bool value = false; };
    template<typename A>
    struct is_ptr<A*> { static const bool value = true; };

    template<typename A>
    struct is_ref { static const bool value = false; };
    template<typename A>
    struct is_ref<A&> { static const bool value = true; };

    template<typename C>
    struct void2val { static C invoke(void* input) { return *(C*)input; } };
    template<typename C>
    struct void2ptr { static C* invoke(void* input) { return (C*)input; } };
    template<typename C>
    struct void2ref { static C& invoke(void* input) { return *(C*)input; } };

    template<typename C>
    struct void2type
    {
        static C invoke(void* ptr)
        {
            return	if_<is_ptr<C>::value
                , void2ptr<typename base_type<C>::type>
                , typename if_<is_ref<C>::value
                , void2ref<typename base_type<C>::type>
                , void2val<typename base_type<C>::type>
                >::type
            >::type::invoke(ptr);
        }
    };

	template<typename T>
	struct lua2object {
		static T invoke(lua_State *L, int index) {
			return void2type<T>::invoke(lua_touserdata(L, index));
		}
	};

	template<typename T>
	struct lua2enum {
		static T invoke(lua_State *L, int index) {
			return static_cast<T>(lua_tointeger(L, index));
		}
	};

	template<typename T>
	struct lua2user {
		static T invoke(lua_State *L, int index) {
			if (!lua_isuserdata(L, index) && !lua_isnoneornil(L, index)) {
				luaL_error(L, "argument types do not match.");
			}
			return void2type<T>::invoke(lua_isnoneornil(L, index) ? nullptr :
					(lua_islightuserdata(L, index) ? lua_touserdata(L, index) :
						assert_userdata_alive(L, lua2object<user*>::invoke(L, index))));
		}
		static void *assert_userdata_alive(lua_State *L, user *puser) {
			if (puser->obj == nullptr) {
				luaL_error(L, "userdata[%p] is a nil value.", puser);
			}
			return puser->obj;
		}
	};

	template<typename T>
	struct lua_op_t < T* >
	{
		static int push_stack(lua_State* L, const T* value)
		{
			if (value)
			{
				if (!class_type<T>::is_registered())
					lua_pushlightuserdata(L, (void*)(value));
				else
				{
					push_object(L, (void*)value, class_type<T>::meta().c_str());
				}
			}
			else
				lua_pushnil(L);
			return 1;
		}
		static void from_stack(lua_State* L, int pos, T** value)
		{
			if (lua_isnoneornil(L, pos))
			{
				*value = nullptr;
				return;
			}
			LUA_CHECK_PARAM(lua_isuserdata(L, pos) || lua_islightuserdata(L, pos), type::userdata, pos);
			*value = check_type<T>(L, pos);
		}

		static bool try_get(lua_State* L, int pos, T** value)
		{
			if (lua_isnil(L, pos) || lua_isuserdata(L, pos) || lua_islightuserdata(L, pos))
			{
				if (lua_isnoneornil(L, pos))
				{
					*value = nullptr;
					return true;
				}
				*value = get_type<T>(L, pos);
				return true;
			}
			else
				return false;
		}
	};

	template<typename T>
	struct ptr2lua {
		static void invoke(lua_State* L, T* input) {
			if (input != nullptr) {
				if (class_type<T>::is_registered()) {
					//TODO:
				}
				else {
					lua_pushlightuserdata(L, void*(input));
				}
			}
			else {
				lua_pushnil(L);
			}
		}
	};
	template<typename T>
	struct ref2lua {
		static void invoke(lua_State* L, T& input) {
			if (class_type<T>::is_registered()) {
				//TODO:
			}
			else {
				lua_pushlightuserdata(L, void*(&input));
			}
		}
	};

	template<typename T>
	struct val2lua {
		static void invoke(lua_State* L, T&& input) {
			//TODO:
		}
	};

	template<typename T>
	struct enum2lua {
		static void invoke(lua_State* L, T val) {
			lua_pushinteger(L, static_cast<lua_Integer>(val));
		}
	};

	template<typename T>
	struct user2lua {
		static void invoke(lua_State* L, T val) {
			std::conditional<std::is_pointer<T>::value,
				ptr2lua<typename std::remove_pointer<T>::type>,
				typename std::conditional<std::is_lvalue_reference<T>::value,
				ref2lua<typename std::remove_reference<T>::type>,
				val2lua<typename std::remove_reference<T>::type>
				>::type
			>::type::invoke(L, std::forward<T>(val));
		}
	};


	// member variable
	struct var_base
	{
		virtual void get(lua_State *L) = 0;
		virtual void set(lua_State *L) = 0;
	};

	template<typename C, typename V>
	struct mem_var_readwrite : var_base
	{
		V C::*_var;
		mem_var_readwrite(V C::*val) : _var(val) {}
		void get(lua_State *L) {
			C* obj = NULL;
			lua::get(L, 1, &obj);
			lua::push(L, obj->*(_var));
		}
		void set(lua_State *L) {
			C* obj = NULL;
			lua::get(L, 1, &obj);
			V val = V();
			lua::get(L, 3, &val);
			obj->*(_var) = val;
		}
	};
	template<typename C, typename V>
	struct mem_var_read : var_base
	{
		V C::*_var;
		mem_var_read(V C::*val) : _var(val) {}
		void get(lua_State *L) {
			C* obj = NULL;
			lua::get(L, 1, &obj);
			lua::push(L, obj->*(_var));
		}
		void set(lua_State *L) {
			luaL_error(L, "can't set to a readonly variale '%s'", lua_tostring(L, 2));
		}
	};
	template<typename C, typename V>
	struct mem_var_write : var_base
	{
		V C::*_var;
		mem_var_write(V C::*val) : _var(val) {}
		void get(lua_State *L) {
			luaL_error(L, "can't access a writeonly variable '%s'", lua_tostring(L, 2));
		}
		void set(lua_State *L) {
			C* obj = NULL;
			::get(L, 1, &obj);
			V val = V();
			::get(L, 3, &val);
			obj->*(_var) = val;
		}
	};

	template<typename V>
	struct mem_var_static_readwrite : var_base
	{
		V *_var;
		mem_var_static_readwrite(V *val) : _var(val) {}
		void get(lua_State *L) {
			lua::push(L, *_var);
		}
		void set(lua_State *L) {
			V val = V();
			lua::get(L, 3, &val);
			*_var = val;
		}
	};
	template<typename V>
	struct mem_var_static_read : var_base
	{
		V *_var;
		mem_var_static_read(V *val) : _var(val) {}
		void get(lua_State *L) {
			lua::push(L, *_var);
		}
		void set(lua_State *L) {
			luaL_error(L, "can't set to a readonly variale '%s'", lua_tostring(L, 2));
		}
	};
	template<typename V>
	struct mem_var_static_write : var_base
	{
		V *_var;
		mem_var_static_write(V *val) : _var(val) {}
		void get(lua_State *L) {
			luaL_error(L, "can't access a writeonly variable '%s'", lua_tostring(L, 2));
		}
		void set(lua_State *L) {
			V val = V();
			lua::get(L, 3, &val);
			*_var = val;
		}
	};
	

 }

//register
namespace lua
{
	class lua_register_base_t
	{
	protected:
		lua_State* m_ls;
	public:
		lua_register_base_t(lua_State* l_) : m_ls(l_) {
		}
	protected:
		static bool __index_parent(lua_State* L)
		{//t, key, mt,
			bool found = false;
			lua_pushstring(L, "__parent");//k
			lua_rawget(L, -2);//parent
			if (lua_istable(L, -1)) {
				lua_pushvalue(L, 2);//parent, key
				lua_rawget(L, -2);//parent, v
				if (lua_type(L, -1) == LUA_TUSERDATA) {
					lua2object<var_base*>::invoke(L, -1)->get(L);
					found = true;
				}
				else if (lua_isnil(L, -1)) {
					lua_pop(L, 1);
					found = __index_parent(L);
				}
				else {
					found = true;
				}
			}
			else if (!lua_isnil(L, -1)) {
				luaL_error(L, "find 'parent' class variable. (the kind of variable is nonsupport.)");
			}

			return found;
		}

		static bool __newindex_parent(lua_State* L)
		{
			bool found = false;
			lua_pushliteral(L, "__parent");
			lua_rawget(L, -2);
			if (lua_istable(L, -1)) {
				lua_pushvalue(L, 2);
				lua_rawget(L, -2);
				if (lua_type(L, -1) == LUA_TUSERDATA) {
					lua2object<var_base*>::invoke(L, -1)->set(L);
					found = true;
				}
				else if (lua_isnil(L, -1)) {
					lua_pop(L, 1);
					found = __newindex_parent(L);
				}
				else {
					found = true;
				}
			}
			else if (!lua_isnil(L, -1)) {
				luaL_error(L, "find 'parent' class variable. (the kind of variable is nonsupport.)");
			}
			return found;
		}

		static int __index(lua_State* L, int index, bool strict = true)
		{//t, key
			lua_getmetatable(L, 1);//t, key, mt
			lua_pushvalue(L, 2);//t, key, mt, key
			lua_rawget(L, -2);//t, key, mt, value

			if (lua_isuserdata(L, -1)) {
				lua2object<var_base*>::invoke(L, -1)->get(L);//t, key, mt, value, val
			}
			else if (lua_isnil(L, -1))
			{
				lua_pop(L, 1);//t, key, mt
				bool b = __index_parent(L);//t, key, mt, v
				if (!b && strict) {
					bool has_mt = lua_getmetatable(L, 1);
					if (has_mt) {//t, key, mt, v, mt
						lua_pushstring(L, "__name");//t, key, mt, v, mt, key
						lua_rawget(L, -2); //t, key, mt, v, mt, mtname
						if (lua_isstring(L, -1)){
							const char* mtname = lua_tostring(L, -1);
							lua_pop(L, 2); //...
							luaL_error(L, "can't find '%s@%s:%p' class variable. (forgot registering class variable ?)", lua_tostring(L, 2), mtname, lua_topointer(L, 1));
						}
						else {
							lua_pop(L, 2); //...
							luaL_error(L, "can't find '%s:%p' class variable. (forgot registering class variable ?)", lua_tostring(L, 2), lua_topointer(L, 1));
						}
					}
					else {
						luaL_error(L, "can't find '%s:%p' class variable. (forgot registering class variable ?)", lua_tostring(L, 2), lua_topointer(L, 1));
					}
				}
			}

			lua_replace(L, 3);
			lua_settop(L, 3);

			return 1;
		}

		static int __newindex(lua_State* L)
		{//t,key,value
			lua_getmetatable(L, 1);//t,key,value,mt
			lua_pushvalue(L, 2);//t,key,value,mt,key
			lua_rawget(L, -2);//t,key,value, mt, val
			if (lua_isuserdata(L, -1)) {
				lua2object<var_base*>::invoke(L, -1)->set(L);
			}
			else if (lua_isnil(L, -1))
			{
				lua_pop(L, 1);//t, key, mt
				bool b = __newindex_parent(L);//t, key, mt, v
				if (!b) {
					luaL_error(L, "can't find '%s' class variable. (forgot registering class variable ?)", lua_tostring(L, 2));
				}
			}
			lua_settop(L, 3);
			return 0;
		}

		/*
		* 在chain为true的时候，会兼容父类mtname
		*/
		static void getmeta_chain(lua_State* L, int index, std::vector<std::string>& chain_list)
		{
			lua_stack_gurad g(L);
			lua_pushvalue(L, index);
			do
			{
				if (lua_isnoneornil(L, -1))
					break;
				lua_pushstring(L, "__mtname"); //mt, key
				lua_rawget(L, -2); //mt, name
				if (!lua_isstring(L, -1))
				{
					lua_remove(L, -1); //mt
					break;
				}
				const char* mtname = lua_tostring(L, -1);
				chain_list.push_back(mtname);
				lua_pop(L, 1);

				lua_pushstring(L, "__parent"); //mt, key
				lua_rawget(L, -2); //mt, parent
				lua_remove(L, -2); //parent
			} while (true);
		}
	};
	
	template < typename T >
	class lua_register_t : lua_register_base_t
	{
	protected:
		//成员函数
		template < typename R, typename U, typename ...Args >
		lua_register_t& def(const class_function_t<R, U, Args...>& func)
		{
			assert(class_type<T>::is_registered());
			typedef typename class_function_t<R, U, Args...>::function_t function_t;

			auto lambda = [](lua_State* l)->int
			{
				U* obj = NULL;
				lua::get(l, 1, &obj);
				if (!obj)
				{
					luaL_error(l, "class '%s' method on handler error,%s:%d", class_type<U>::name_.c_str(), __FILE__, __LINE__);
					return 0;
				}

				auto f = static_cast<function_t*>(lua_touserdata(l, lua_upvalueindex(1)));
				return call(l, obj, *f, 1);
			};

			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());
			lua_pushstring(m_ls, func.name_);
			new(lua_newuserdata(m_ls, sizeof(function_t))) decltype(func.function_)(func.function_);
			lua_pushcclosure(m_ls, lambda, 1);
			lua_rawset(m_ls, -3);

			return *this;
		}

		//静态以及全局函数
		template < typename R, typename ...Args >
		lua_register_t& def(const free_function_t<R, Args...>& func)
		{
			assert(class_type<T>::is_registered());
			auto lambda = [](lua_State* l)->int
			{
				typedef typename free_function_t<R, Args...>::function_t function_t;
				auto f = (function_t)(lua_touserdata(l, lua_upvalueindex(1)));

				return call(l, f);
			};
			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());
			lua_pushstring(m_ls, func.name_);
			lua_pushlightuserdata(m_ls, reinterpret_cast<void*>(func.function_));
			lua_pushcclosure(m_ls, lambda, 1);
			lua_rawset(m_ls, -3);

			return *this;
		}

		//lua函数
		lua_register_t& def(const lua_function_t& func)
		{
			assert(class_type<T>::is_registered());
			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());
			lua_pushstring(m_ls, func.name_);
			lua_pushcfunction(m_ls, func.func_);
			lua_rawset(m_ls, -3);

			return *this;
		}

		static int __delete(lua_State* L, bool gc)
		{
			user* ud = NULL;
			ud = get_userdata(L, 1);
			if (!gc && (!ud || !ud->obj || !ud->flag))
			{
				luaL_error(L, "%s __delete method handler error,%s:%d", class_type<T>::name_.c_str(), __FILE__, __LINE__);
				return 1;
			}
			ptr2user* pud = static_cast<ptr2user*>(ud);
			if (pud) {
				pud->unbind<T>(L, gc);
			}
			return 0;
		}

		void linkparent_method(const char* name)
		{
			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());//cls			
			do
			{
				lua_pushstring(m_ls, "__parent");
				lua_rawget(m_ls, -2);//cls, pcls
				if (!lua_istable(m_ls, -1))
					break;

				lua_pushstring(m_ls, name);
				lua_rawget(m_ls, -2);//cls, pcls, fun

				if (lua_isfunction(m_ls, -1)) {
					lua_pushstring(m_ls, name);//cls, pcls, fun, name
					lua_pushvalue(m_ls, -2);//cls, pcls, fun, name, func
					lua_rawset(m_ls, 1);//cls, pcls, fun
					lua_pop(m_ls, 2);//cls, 
					break;
				}
				lua_pop(m_ls, 1);//cls, pcls,
				lua_replace(m_ls, 1);//pcls
			} while (1);
			lua_pop(m_ls, 1);
		}

		bool hasmethod(const char* name)
		{
			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());//cls
			lua_pushstring(m_ls, name);
			lua_rawget(m_ls, -2);//cls, func
			bool b = !!lua_isfunction(m_ls, -1);
			lua_pop(m_ls, 2);
			return b;
		}

	public:
		explicit lua_register_t(lua_State* ls_, const char* name) : lua_register_base_t(ls_)
		{
			assert(!class_type<T>::is_registered());
			class_type<T>::name_ = name;
						
			std::string cls_name = class_type<T>::name_;
			std::string meta_name = class_type<T>::meta();

			lua_stack_gurad g(m_ls);
			
			luaL_newmetatable(m_ls, meta_name.c_str());//cls
			{
				lua_pushstring(m_ls, "__name");
				lua_pushstring(m_ls, cls_name.c_str());
				lua_rawset(m_ls, -3);//cls

				lua_pushstring(m_ls, "__mtname");
				lua_pushstring(m_ls, meta_name.c_str());
				lua_rawset(m_ls, -3);//cls

				lua_pushstring(m_ls, "tryget");
				lua_pushcclosure(m_ls, [](lua_State* l)->int
				{
					return __index(l, -1, false);
				}, 0);
				lua_rawset(m_ls, -3);//cls

				lua_pushliteral(m_ls, "__index");
				lua_pushcclosure(m_ls, [](lua_State* l)->int
				{
					const char* key = lua_tostring(l, -1);
					if (key && 0 == strcmp(key, "isnil"))
					{
						T* self = nullptr;
						bool isnil = true;
						if (lua::tryget(l, 1, &self))
							isnil = self == nullptr;
						lua::push(l, isnil);
						return 1;
					}

					return __index(l, -1, true);
				}, 0);
				lua_rawset(m_ls, -3);//cls

				lua_pushliteral(m_ls, "__newindex");
				lua_pushcclosure(m_ls, [](lua_State* l)->int
				{
					const char* key = lua_tostring(l, -1);
					if (0 == strcmp(key, "__name")
						|| 0 == strcmp(key, "__mtname")
						|| 0 == strcmp(key, "__index")
						|| 0 == strcmp(key, "__newindex")
						|| 0 == strcmp(key, "__gc")
						|| 0 == strcmp(key, "__typetable")
						|| 0 == strcmp(key, "__class")
						|| 0 == strcmp(key, "is"))
					{
						luaL_error(l, "key '%s' not support write, internal use. ", key);
						return 0;
					}

					return __newindex(l);
				}, 0);
				lua_rawset(m_ls, -3);//cls

				lua_pushliteral(m_ls, "__gc");
				lua_pushcclosure(m_ls, [](lua_State* l)->int
				{
					return __delete(l, true);
				}, 0);
				lua_rawset(m_ls, -3);//cls

				lua_pushstring(m_ls, "is");
				lua_pushcclosure(m_ls, [](lua_State* l)->int
				{//t, class
					std::vector<std::string> meta_list;
					getmeta_chain(l, 1, meta_list);
					bool ret = false;
					for (std::string& m : meta_list)
					{
						luaL_getmetatable(l, m.c_str());
						if (lua_istable(l, -1) && lua_equal(l, 2, -1) == 1){
							ret = true;
							lua_pop(l, 1);
							break;
						}
						lua_pop(l, 1);
					}
					lua_pushboolean(l, ret ? 1 : 0);
					return 1;
				}, 0);
				lua_rawset(m_ls, -3);//cls

				lua_pushstring(m_ls, "as");
				lua_pushcclosure(m_ls, [](lua_State* l)->int
				{//ud, class
					std::vector<std::string> meta_list;
					getmeta_chain(l, 2, meta_list);
					bool ret = false;
					for (std::string& m : meta_list)
					{
						luaL_getmetatable(l, m.c_str());//mt
						if (lua_istable(l, -1) && lua_equal(l, 2, -1) == 1) {
							lua_pushvalue(l, 1);//mt, t
							lua_pushvalue(l, -2);//mt,t,mt
							lua_setmetatable(l, -2);//mt,t
							lua_replace(l, 3);//t
							ret = true;
							break;
						}
						lua_pop(l, 1);
					}
					if(!ret) lua_pushnil(l);

					return 1;
				}, 0);
				lua_rawset(m_ls, -3);//cls

				lua_pushstring(m_ls, "cast");
				lua_pushcclosure(m_ls, [](lua_State* l)->int
				{//ud, class
					std::vector<std::string> meta_list;
					getmeta_chain(l, 2, meta_list);
					bool ret = false;
					for (std::string& m : meta_list)
					{
						luaL_getmetatable(l, m.c_str());
						if (lua_istable(l, -1) && lua_equal(l, 2, -1) == 1) {
							lua_pushvalue(l, 1);//mt, t
							lua_pushvalue(l, -2);//mt,t,mt
							lua_setmetatable(l, -2);//mt,t
							lua_replace(l, 3);//t
							ret = true;
							break;
						}
						lua_pop(l, 1);
					}
					if (!ret) {
						lua_pushstring(l, "__name");
						lua_rawget(l, 1);
						const char* srcname = luaL_checkstring(l, -1);
						lua_pop(l, 1);

						lua_pushstring(l, "__name");
						lua_rawget(l, 2);
						const char* dstname = luaL_checkstring(l, -1);
						lua_pop(l, 1);

						luaL_error(l, "bad cast from '%s' to '%s'", srcname, dstname);
					}
					return 1;
				}, 0);
				lua_rawset(m_ls, -3);//cls

				lua_pushliteral(m_ls, "__tostring");
				lua_pushcclosure(m_ls, [](lua_State* l)->int
				{
					lua_pushfstring(l, "class:%s#%s: %p", class_type<T>::name_.c_str(), luaL_typename(l, 1), lua_topointer(l, 1));
					return 1;
				}, 0);
				lua_rawset(m_ls, -3);//typeMeta
			}	

			create_table(m_ls, cls_name.c_str());//cls, typeMeta		
			{
				lua_pushstring(m_ls, "__name");
				lua_pushstring(m_ls, cls_name.c_str());
				lua_rawset(m_ls, -3);//typeMeta

				lua_pushstring(m_ls, "__mtname");
				lua_pushstring(m_ls, meta_name.c_str());
				lua_rawset(m_ls, -3);//typeMeta

				lua_pushstring(m_ls, "__class");
				luaL_getmetatable(m_ls, meta_name.c_str());
				lua_rawset(m_ls, -3);//typeMeta

				lua_pushstring(m_ls, "__index");
				lua_pushcclosure(m_ls, [](lua_State* l)->int
				{//t, k
					const char* key = luaL_checkstring(l, 2);
					if (0 == strcmp(key, "__parent")) {
						lua_rawget(l, -2);
						return 1;
					}

					lua_pushvalue(l, 1);//t,k,t
					lua_pushvalue(l, 2);//t,k,t,k
					do
					{
						if (!lua_istable(l, -2)) {
							break;
						}

						lua_rawget(l, -2);//t,k,t,v
						if (!lua_isnil(l, -1))
							break;
						else {
							lua_pop(l, 1); //t,k,t
							lua_pushstring(l, "__parent");//t,k,t,kp
							lua_rawget(l, -2);//t,k,t,tp
							lua_replace(l, 3);//t,k,tp
							lua_pushvalue(l, 2);//t,k,tp,k
						}
					} while (true);
					//t,k,t,k,v
					lua_replace(l, 3);
					lua_settop(l, 3);

					return 1;
				}, 0);
				lua_rawset(m_ls, -3);//typeMeta

				lua_pushstring(m_ls, "class");
				lua_pushcclosure(m_ls, [](lua_State* l)->int
				{//t
					luaL_getmetatable(l, class_type<T>::meta().c_str());
					return 1;
				}, 0);
				lua_rawset(m_ls, -3);//typeMeta
			}

			//cls.__metatable = typeMeta
			lua_setmetatable(m_ls, -2);//cls

			//__call
			lua_newtable(m_ls);
			{
				lua_pushstring(m_ls, "__index");
				lua_pushvalue(m_ls, -3);
				lua_rawset(m_ls, -3); // t.__index = cls

				lua_pushliteral(m_ls, "__tostring");
				lua_pushcclosure(m_ls, [](lua_State* l)->int
				{//t
					lua_pushfstring(l, "class_type:%s#%s: %p", class_type<T>::name_.c_str(), luaL_typename(l, 1), lua_topointer(l, 1));
					return 1;
				}, 0);
				lua_rawset(m_ls, -3);//typeMeta
			}
			get_table(m_ls, cls_name.c_str());//t, typeMeta
			lua_pushstring(m_ls, "__typetable");//t, typeMeta, k
			lua_pushvalue(m_ls, -3);//t, typeMeta, k, t
			lua_rawset(m_ls, -3);//t, typeMeta //typeMeta.__typetable = t
			lua_pushvalue(m_ls, -2);//t, typeMeta, t
			lua_setmetatable(m_ls, -2);//typeMeta.__metatable = t
		}
	public:
		template<typename P>
		lua_register_t& extend()
		{
			assert(class_type<P>::is_registered());
			return extend(class_type<P>::meta().c_str());
		}

		lua_register_t& extend(const char* parent)
		{
			assert(parent);
			assert(class_type<T>::is_registered());
			{
				lua_stack_gurad g(m_ls);
				push_metatable(m_ls, class_type<T>::meta());
				lua_pushstring(m_ls, "__parent");
				push_metatable(m_ls, parent);
				lua_rawset(m_ls, -3);
			}
			if(!hasmethod("__gc")) linkparent_method("__gc");
			if (!hasmethod("__tostring")) linkparent_method("__tostring");

			return (*this);
		}

		//构造函数
		template < typename ...Args >
		lua_register_t& def(const constructor_t<Args...>&)
		{
			typedef std::tuple<Args...> tuple_t;
			auto lambda = [](lua_State* l)->int
			{
				T* val = NULL;
				call(l, &val, tuple_t());
				lua::push(l, val);

				return 1;
			};
			lua_stack_gurad g(m_ls);

			get_table(m_ls, class_type<T>::name_.c_str());
			lua_pushstring(m_ls, "__typetable");
			lua_rawget(m_ls, -2);

			lua_pushliteral(m_ls, "__call");
			lua_pushcclosure(m_ls, lambda, 0);
			lua_rawset(m_ls, -3);//typeMeta,t
			
			return *this;
		}

		//析构函数
		lua_register_t& def(const destructor_t& func)
		{
			auto lambda = [](lua_State* l)->int
			{
				return __delete(l, false);
			};
			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());
			lua_pushstring(m_ls, "delete");
			lua_pushcfunction(m_ls, lambda);
			lua_rawset(m_ls, -3);

			return *this;
		}
		//静态以及全局函数
		template < typename R, typename ...Args >
		lua_register_t& def(const char* name, R(*func)(Args...))
		{
			return def(lua::def(name, func));
		}
		//lua函数
		lua_register_t& def(const char* name, int(*func)(lua_State*))
		{
			return def(lua::def(name, func));
		}
		//成员函数
		template < typename R, typename U, typename ...Args >
		lua_register_t& def(const char* name, R(U::* func)(Args...))
		{
			return def(lua::def(name, func));
		}
		template < typename R, typename U, typename ...Args >
		lua_register_t& def(const char* name, R(U::* func)(Args...) const)
		{
			return def(lua::def(name, func));
		}
		template < typename R, typename U, typename ...Args >
		lua_register_t& def(const char* name, R(U::* func)(Args...) volatile)
		{
			return def(lua::def(name, func));
		}

		//成员属性	
		template < typename R, typename V >
		lua_register_t& def(const char* name, V(R::* field))
		{
			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());
			lua_pushstring(m_ls, name);
			new(lua_newuserdata(m_ls, sizeof(mem_var_readwrite<R, V>))) mem_var_readwrite<R, V>(field);
			lua_rawset(m_ls, -3);

			return *this;
		}
		template < typename R, typename V >
		lua_register_t& readonly(const char* name, V(R::* field))
		{
			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());
			lua_pushstring(m_ls, name);
			new(lua_newuserdata(m_ls, sizeof(mem_var_read<R, V>))) mem_var_read<R, V>(field);
			lua_rawset(m_ls, -3);

			return *this;
		}
		template < typename R, typename V >
		lua_register_t& writeonly(const char* name, V(R::* field))
		{
			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());
			lua_pushstring(m_ls, name);
			new(lua_newuserdata(m_ls, sizeof(mem_var_write<R, V>))) mem_var_write<R, V>(field);
			lua_rawset(m_ls, -3);

			return *this;
		}
		//静态以及全局成员属性
		template <typename V >
		lua_register_t& def(const char* name, V* field)
		{
			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());
			lua_pushstring(m_ls, name);
			new(lua_newuserdata(m_ls, sizeof(mem_var_static_readwrite<V>))) mem_var_static_readwrite<V>(field);
			lua_rawset(m_ls, -3);

			return *this;
		}
		template <typename V >
		lua_register_t& readonly(const char* name, V* field)
		{
			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());
			lua_pushstring(m_ls, name);
			new(lua_newuserdata(m_ls, sizeof(mem_var_static_read<V>))) mem_var_static_read<V>(field);
			lua_rawset(m_ls, -3);

			return *this;
		}
		template <typename V >
		lua_register_t& writeonly(const char* name, V* field)
		{
			lua_stack_gurad g(m_ls);
			push_metatable(m_ls, class_type<T>::meta());
			lua_pushstring(m_ls, name);
			new(lua_newuserdata(m_ls, sizeof(mem_var_static_write<V>))) mem_var_static_write<V>(field);
			lua_rawset(m_ls, -3);

			return *this;
		}
	};

	template <>
	class lua_register_t<void> : lua_register_base_t
	{
		std::string _name;
	protected:
		//静态以及全局函数
		template < typename R, typename ...Args >
		lua_register_t& def(const free_function_t<R, Args...>& func)
		{
			auto lambda = [](lua_State* l)->int
			{
				typedef typename free_function_t<R, Args...>::function_t function_t;
				auto f = (function_t)(lua_touserdata(l, lua_upvalueindex(1)));

				return call(l, f);
			};
			lua_stack_gurad g(m_ls);
			get_table(m_ls, _name.c_str());
			lua_pushstring(m_ls, func.name_);
			lua_pushlightuserdata(m_ls, reinterpret_cast<void*>(func.function_));
			lua_pushcclosure(m_ls, lambda, 1);
			lua_rawset(m_ls, -3);

			return *this;
		}
		//lua函数
		lua_register_t& def(const lua_function_t& func)
		{
			lua_stack_gurad g(m_ls);
			get_table(m_ls, _name.c_str());
			lua_pushstring(m_ls, func.name_);
			lua_pushcfunction(m_ls, func.func_);
			lua_rawset(m_ls, -3);

			return *this;
		}
		
	public:
		~lua_register_t()
		{
		}
		explicit lua_register_t(lua_State* ls_, const char* name = "_G") : lua_register_base_t(ls_)
		{
			assert(name);
			_name = name;
			lua_stack_gurad g(m_ls);
			create_table(m_ls, name);//t
		}

		//静态以及全局函数
		template < typename R, typename ...Args >
		lua_register_t& def(const char* name, R(*func)(Args...))
		{
			return def(lua::def(name, func));
		}
		//lua函数
		lua_register_t& def(const char* name, int(*func)(lua_State*))
		{
			return def(lua::def(name, func));
		}

		//全局成员属性
		template <typename V >
		lua_register_t& def(const char* name, V* field)
		{
			lua_stack_gurad g(m_ls);
			get_table(m_ls, _name.c_str());
			lua_pushstring(m_ls, name);
			new(lua_newuserdata(m_ls, sizeof(mem_var_static_readwrite<V>))) mem_var_static_readwrite<V>(field);
			lua_rawset(m_ls, -3);

			return *this;
		}
		template <typename V >
		lua_register_t& readonly(const char* name, V* field)
		{
			lua_stack_gurad g(m_ls);
			get_table(m_ls, _name.c_str());
			lua_pushstring(m_ls, name);
			new(lua_newuserdata(m_ls, sizeof(mem_var_static_read<V>))) mem_var_static_read<V>(field);
			lua_rawset(m_ls, -3);

			return *this;
		}
		//这里必须是简单类型，复杂类型，只能用指针
		template <typename V >
		lua_register_t& readonly(const char* name, const V& val)
		{
			lua_stack_gurad g(m_ls);
			get_table(m_ls, _name.c_str());
			lua_pushstring(m_ls, name);
			lua::push(m_ls, val);
			lua_rawset(m_ls, -3);

			return *this;
		}

		template <typename V >
		lua_register_t& writeonly(const char* name, V* field)
		{
			lua_stack_gurad g(m_ls);
			get_table(m_ls, _name.c_str());
			lua_pushstring(m_ls, name);
			new(lua_newuserdata(m_ls, sizeof(mem_var_static_write<V>))) mem_var_static_write<V>(field);
			lua_rawset(m_ls, -3);

			return *this;
		}
	};
}

#endif//LUA_SCRIPTS_HPP