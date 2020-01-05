#ifndef _LUA_OBJECT_HPP
#define _LUA_OBJECT_HPP
#pragma once
#include "lua_script.hpp"

namespace lua
{
	namespace internal
	{
		struct LuaUserData
		{
			void * object;
			LuaUserData * stamp;
			int	flag;
		};

		class ObjectsInLuaRec
		{
			typedef LuaUserData *  StampT;
			struct ObjectRec
			{
				void * object;
				StampT stamp;

				ObjectRec(void * obj, StampT stamp_) : object(obj), stamp(stamp_)
				{}
			};

			struct Comp_Rec
			{
				bool operator() (const ObjectRec& lhs, const ObjectRec& rhs) const
				{
					//return (static_cast<char*>(lhs.object) - static_cast<char*>(rhs.object)) < 0;
					uint64 lhsl = (uint64)(lhs.object);
					uint64 lhsr = (uint64)(rhs.object);
					return lhsl < lhsr;
				}
			};

			std::set<ObjectRec, Comp_Rec> ObjectsInLua;

		public:
			ObjectsInLuaRec() {}
			inline void AddObject(void * object, StampT stamp)
			{
				ObjectRec rec = ObjectRec(object, stamp);
				auto found = ObjectsInLua.find(rec);
				if (found != ObjectsInLua.end())
				{
					const ObjectRec & rec2 = *found;
					if (rec2.stamp != stamp)
					{
						ObjectsInLua.erase(found);
						ObjectsInLua.insert(rec);
					}
				}
				else
				{
					ObjectsInLua.insert(rec);
				}
			}
			inline bool RemoveObject(void * object, StampT stamp, bool force = false)
			{
				if (!object) return false;
				if (ObjectsInLua.size() <= 0) return false;
				if (!force)
				{
					auto found = ObjectsInLua.find(ObjectRec(object, 0));
					if (found != ObjectsInLua.end())
					{
						const ObjectRec & rec = *found;
						if (rec.stamp == stamp)
						{
							ObjectsInLua.erase(found);
							return true;
						}
					}
				}
				else
				{
					for (auto it=ObjectsInLua.begin(); it != ObjectsInLua.end(); ++it)
					{
						const ObjectRec & rec = *it;
						if (rec.object == object)
						{
							ObjectsInLua.erase(it);
							return true;
						}
					}
				}
				return false;
			}
			inline bool FindObject(void * object, StampT stamp)
			{
				if (!object) return false;
				if (ObjectsInLua.size() <= 0) return false;
				auto found = ObjectsInLua.find(ObjectRec(object, stamp));
				if (found != ObjectsInLua.end())
				{
					const ObjectRec&  rec = *found;
					return rec.stamp == stamp;
				}
				return false;
			}
		};

		class LuaObjectWrapper
		{
		public:
			LuaObjectWrapper()
			{
			}
			~LuaObjectWrapper()
			{
			}
			static LuaObjectWrapper& Get()
			{
				static LuaObjectWrapper wrapper;
				return wrapper;
			}
			inline int PushObjectToLua(lua_State* l, const void* const_obj, const char* mtname, int flag = 0)
			{
				if (!const_obj)
				{
					lua_pushnil(l);
					return 1;
				}
				void* obj = const_cast<void*>(const_obj);
				return PushObjectToLua(l, obj, mtname);
			}

			inline int PushObjectToLua(lua_State* l, void* obj, const char* mtname, int flag = 0)
			{
				if (!obj)
				{
					lua_pushnil(l);
					return 1;
				}
				tryGetUserdataFromWeakTable(l, obj);
				if (lua_isnil(l, -1) || ((LuaUserData*)lua_touserdata(l, -1) == nullptr))
				{
					lua_pop(l, 1); //t
					if(mtname)
					{
						LuaUserData* userdata = (LuaUserData*)lua_newuserdata(l, sizeof(LuaUserData)); //t,ud
						userdata->object = obj;
						userdata->flag = flag;
						userdata->stamp = userdata;
						GetObjectRec().AddObject(obj, userdata->stamp);

						luaL_getmetatable(l, mtname);//t,ud,mt
						if (lua_isnil(l, -1))
						{
							printf("metatable %s not register!\n", mtname);
						}
						lua_setmetatable(l, -2);//t,ud
						lua_pushlightuserdata(l, obj); //t,ud,obj
						lua_pushvalue(l, -2); //t,ud,obj,ud
						lua_settable(l, -4); //t,ud
					}
					else
					{
						lua_pushlightuserdata(l, obj);
					}
				}
				else
				{
					LuaUserData * userdata = (LuaUserData*)lua_touserdata(l, -1);
					userdata->object = obj;
					assert(userdata->flag == flag);
				}

				lua_remove(l, -2); //ud
				if (mtname)
				{
					AddUObject2LuaGlobalTable(l, obj);
				}
				return 1;
			}
			
			inline void* GetObject(lua_State * L, int ParamIndex, const char* mtname, bool poperror=true)
			{
				if(mtname)
				{
					LuaUserData * userdata = GetObjectUserData(L, ParamIndex, mtname);
					if (userdata && !userdata->object)
					{
						if(poperror)
						{
							lua_pushstring(L, "Content of LuaUserData has been removed!");
							lua_error(L);
						}
						return nullptr;
					}
					if (userdata && !userdata->stamp && !GetObjectRec().FindObject(userdata->object, userdata->stamp))
					{
						userdata->object = nullptr;
						if(poperror)
						{
							lua_pushstring(L, "Content of LuaUserData has been disappeared!");
							lua_error(L);
						}
						return nullptr;
					}
					return userdata != nullptr ? userdata->object : nullptr;
				}
				else
				{
					if (!lua_isuserdata(L, ParamIndex))
					{
						if(poperror)
						{
							lua_pushstring(L, "GetObject #1 must be userdata");
							lua_error(L);
						}
						return nullptr;
					}
					return lua_touserdata(L, ParamIndex);
				}
			}

			inline void* GetObject(lua_State* L, int ParamIndex, bool poperror=true)
			{
				if (!lua_isuserdata(L, ParamIndex))
				{
					if(poperror)
					{
						lua_pushstring(L, "GetObject #1 must be userdata");
						lua_error(L);
					}
					return nullptr;
				}
				bool has_mt = lua_getmetatable(L, ParamIndex);//ud, mt,ʧ�ܲ�ѹջ
				if(has_mt)
				{
					lua_pushstring(L, "__mtname");//ud, mt, key
					lua_rawget(L, -2); //ud, mt, mtname
					if (lua_isstring(L, -1))
					{
						const char* mtname = lua_tostring(L, -1);
						lua_pop(L, 2); //ud
						return GetObject(L, ParamIndex, mtname, poperror);
					}
					else
					{
						lua_pop(L, 2); //ud
						return GetObject(L, ParamIndex, nullptr, poperror);
					}
				}
				else
				{
					return GetObject(L, ParamIndex, nullptr, poperror);
				}
			}

			bool RemoveObjectFromLua(lua_State* l, int* flag = nullptr)
			{
				LuaUserData* ud = GetObjectUserData(l, 1);
				if (ud && ud->object != nullptr)
				{
					RemoveUObjectFromLuaGlobalTable(l, ud->object);
					bool  owner = GetObjectRec().RemoveObject(ud->object, ud->stamp);
					if (owner)
					{
						if(flag) *flag = ud->flag;
						removeUserdataFromWeakTable(l, ud->object);
					}
					ud->object = nullptr;
				}
				return true;
			}

			bool RemoveObject(lua_State* l, void* ptr)
			{
				if(!ptr) return true;
				LuaUserData ud;
				ud.flag = 0;
				ud.object = ptr;
				ud.stamp = &ud;
				bool owner = GetObjectRec().RemoveObject(ud.object, ud.stamp, true);
				if (owner && l)
				{
					removeUserdataFromWeakTable(l, ptr);
				}
				return true;
			}

			template<typename T>
			T* CheckObject(lua_State* l, int index, bool poperror=true)
			{
				void* obj = GetObject(l, index, poperror);
				T* ud = static_cast<T *>(obj);
				if (!ud)
				{
					char err[100] = { 0 };
					sprintf(err, "param #%d must be a userdata", index);
					lua_pushstring(l, err);
					lua_error(l);
					return NULL;
				}
				return ud;
			}
		protected:
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			inline LuaUserData* GetObjectUserData(lua_State * L, int ParamIndex, const char* mtname)
			{
				if (!lua_isuserdata(L, ParamIndex))
				{
					lua_pushstring(L, "GetObject #1 must be userdata");
					lua_error(L);
					return nullptr;
				}
				LuaUserData * userdata = nullptr;
				if(mtname)
					userdata = (LuaUserData*)luaL_checkudata(L, ParamIndex, mtname);
				else
					userdata = (LuaUserData *)lua_touserdata(L, ParamIndex);
				return userdata;
			}

			inline LuaUserData* GetObjectUserData(lua_State * L, int ParamIndex)
			{
				if (!lua_isuserdata(L, ParamIndex))
				{
					lua_pushstring(L, "GetObject #1 must be userdata");
					lua_error(L);
					return nullptr;
				}
				bool has_mt = lua_getmetatable(L, ParamIndex);//ud,mt,ʧ�ܲ�ѹջ
				if(has_mt)
				{
					lua_pushstring(L, "__mtname");//ud,mt,key
					lua_rawget(L, -2); //ud,mt, mtname
					if (lua_isstring(L, -1))
					{
						const char* mtname = lua_tostring(L, -1);
						lua_pop(L, 2); //ud
						return GetObjectUserData(L, ParamIndex, mtname);
					}
					else
					{
						lua_pop(L, 2); //ud
						return GetObjectUserData(L, ParamIndex, nullptr);
					}
				}
				else//ud
				{
					return GetObjectUserData(L, ParamIndex, nullptr);
				}
			}

			void tryGetUserdataFromWeakTable(lua_State * L, void * Obj)
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
				lua_pushlightuserdata(L, Obj); //ref,obj
				lua_gettable(L, -2); //ref,ud
			}

			void removeUserdataFromWeakTable(lua_State * L, void * Obj)
			{
				lua_getglobal(L, "g_udref"); //ref
				lua_pushlightuserdata(L, Obj);
				lua_pushnil(L);
				lua_settable(L, -3);
				lua_pop(L, 1);
			}

			//Obj has in stack -1
			bool AddUObject2LuaGlobalTable(lua_State* L, void* Obj)
			{

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

			//Obj has in stack -1
			bool RemoveUObjectFromLuaGlobalTable(lua_State* L, void* Obj)
			{
				if (lua_isnil(L, -1))
					return false;

				//UE_LOG(LogAzure, Warning, TEXT("------Remove : %x"), Obj);

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
		protected:
			virtual ObjectsInLuaRec& GetObjectRec()
			{
				static ObjectsInLuaRec inst;
				return inst;
			}
		};
	}

#ifdef LUA_OBJECT_EXTERN
	class LuaObjectContainer : public internal::LuaObjectWrapper
	{
	public:
		LuaObjectContainer(): internal::LuaObjectWrapper()
		{}
	};
#ifdef _MSC_VER
#ifdef USE_DLL_EXPORT
	__declspec(dllexport) extern LuaObjectContainer& get_luaobj_container();
#else
	__declspec(dllimport) extern LuaObjectContainer& get_luaobj_container();
#endif
#else
	extern LuaObjectContainer& get_luaobj_container();
#endif
#else
	static internal::LuaObjectWrapper& get_luaobj_container()
	{
		static internal::LuaObjectWrapper lo;
		return lo;
	}
#endif

    inline int pushobject(lua_State* l, const void* const_obj, const char* meta, int flag = 0)
    {
        return get_luaobj_container().PushObjectToLua(l, const_obj, meta, flag);
    }
    inline int pushobject(lua_State* l, void* obj, const char* meta, int flag = 0)
    {
        return get_luaobj_container().PushObjectToLua(l, obj, meta, flag);
    }

	inline bool removeobject(lua_State* l, int* flag = NULL)
	{
		return get_luaobj_container().RemoveObjectFromLua(l, flag);
	}

	inline bool removeobject(void* ptr, lua_State* l)
	{
		return get_luaobj_container().RemoveObject(l, ptr);
	}

	template<typename T>
	inline T* checkobject(lua_State* l, int pos, bool poperror = true)
	{
		return get_luaobj_container().CheckObject<T>(l, pos, poperror);
	}

	inline void* getobject(lua_State* l, int pos)
	{
		return get_luaobj_container().GetObject(l, pos);
	}
}
#endif//_LUA_OBJECT_HPP