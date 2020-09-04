#include "lua_script.hpp"

class base_t
{
public:
	base_t() {
		printf("C++ construct base_t\n");
	}
	virtual const char* getname() {
		return "base_t";
	}
	virtual ~base_t() {
		printf("C++ base_t::~base_t... %p\n", this);
	}
	void print() {
		printf("C++ base_t::print...\n");
	}

	static void print2() {
		printf("C++ static base_t::print2...\n");
	}

	const char* tostring() {
		printf("C++ call by __tostring\n");
		return "base_t::tostring";
	}

	void base_print()
	{
		printf("C++ base_t::base_print...%s\n", getname());
	}
};

class foo_t : public base_t
{
	std::string _name;
public:
	const int flag = 0;
	int num;
	static int counter;
	foo_t() {
		_name = "class foot";
		num = 0;
		counter++;
	}
	virtual ~foo_t() {
		printf("~foo_t\n");
	}
	void setname(const char* name) {
		_name = name;
	}
	void fprint() {
		printf("foo_t::print..... name = %s, num = %d, counter = %d\n", _name.c_str(), num, counter);
	}
	static void fprint2() {
		printf("static foo_t::print2.....,counter = %d\n", counter);
	}

	virtual const char* getname() {
		return "foo_t";
	}
};

class boo_t : public foo_t
{
public:
	virtual const char* getname() {
		return "boo_t";
	}
};

int foo_t::counter = 0;

int writeonly = 8;

int createfoo(lua_State* l)
{
	foo_t* pf = new foo_t();
	lua::push(l, pf);
	return 1;
}

int createfoo2(lua_State* l)
{
	base_t* pf = new boo_t();
	lua::push(l, pf);
	return 1;
}

int printwriteonly(lua_State*)
{
	printf("writeonly = %d\n", writeonly);
	return 0;
}

int deletebase(lua_State* l)
{
	base_t* b = nullptr;
	lua::get(l, 1, &b);
	printf("deletebase %p\n", b ? b : 0);
	try {
		if (b) {
			bool ret = lua::remove_from_weaktable(l, b);
			if (ret) delete b;
		}
	}
	catch (std::exception& e)
	{
		printf("%s\n", e.what());
	}
	catch (...)
	{

	}
	return 0;
}

static std::string on_print_handler(lua_State* l)
{
	std::string s;
	int n = lua_gettop(l);
	lua_getglobal(l, "tostring");

	for (int i = 1; i <= n; ++i)
	{
		lua_pushvalue(l, -1); // function to be called
		lua_pushvalue(l, i);  // value to print
		lua_pcall(l, 1, 1, 0);

		const char* ret = lua_tostring(l, -1);
		if (ret)
			s.append(ret);

		if (i < n)
			s.append("\t");

		lua_pop(l, 1); //pop result
	}

	return s;
}

static int panic(lua_State* l)
{
	std::string reason = "";
	reason += "unprotected error in call to Lua API (";
	const char* s = lua_tostring(l, -1);
	reason += s;
	reason += ")\n";

	throw reason;
	return 0;
}
static int print(lua_State* l)
{
	std::string s = "[LUA]" + on_print_handler(l);
	std::cout << s.c_str() << std::endl;
	return 0;
}
static int warn(lua_State* l)
{
	std::string s = "[LUA][Warn]" + on_print_handler(l);
	std::cout << s.c_str() << std::endl;
	return 0;
}
static int printerror(lua_State* l)
{
	std::string s = "[LUA][Error]" + on_print_handler(l);
	std::cout << s.c_str() << std::endl;
	return 0;
}
static int error_traceback(lua_State* l)
{
	if (!lua_isstring(l, 1))
		return 1;
	lua_checkstack(l, 3);
	lua_getglobal(l, "debug"); //msg, debug
	lua_getfield(l, -1, "traceback"); //msg, debug,traceback
	lua_pushvalue(l, 1); //msg, debug,traceback, msg
	lua_pushnumber(l, 1); //msg, debug,traceback, msg, level
	lua_pcall(l, 2, 1, 0);//msg, debug, tracebackstring

	return 1;
}


int main()
{
	lua_State* m_L = luaL_newstate();
	luaL_openlibs(m_L);
	lua_atpanic(m_L, panic);

	lua_pushcfunction(m_L, print);
	lua_setfield(m_L, LUA_GLOBALSINDEX, "print");

	lua_pushcfunction(m_L, warn);
	lua_setfield(m_L, LUA_GLOBALSINDEX, "warn");

	lua_pushcfunction(m_L, printerror);
	lua_setfield(m_L, LUA_GLOBALSINDEX, "printerror");

	lua_pushcfunction(m_L, error_traceback);
	int m_errRef = luaL_ref(m_L, LUA_REGISTRYINDEX);

	lua::lua_register_t<base_t>(m_L, "base_t")
		.def(lua::constructor<>())
		.def(lua::destructor())
		.def("print", &base_t::print)
		.def("print2", &base_t::print2)
		.def("base_print", &base_t::base_print)
		.def("__tostring", &base_t::tostring);

	lua::lua_register_t<foo_t>(m_L, "foo_t")
		.extend<base_t>()
		.def(lua::constructor<>())
		.def(lua::destructor())
		.def("fprint", &foo_t::fprint)
		.def("fprint2", &foo_t::fprint2)
		.def("setname", &foo_t::setname)
		.def("num", &foo_t::num)
		.readonly("flag", &foo_t::flag)
		.writeonly("writeonly", &writeonly)
		.def("counter", &foo_t::counter);

	lua::lua_register_t<boo_t>(m_L, "boo_t")
		.extend<foo_t>()
		.def(lua::constructor<>())
		.def(lua::destructor())
		.def("getname", &foo_t::getname);

	lua::lua_register_t<void>(m_L)
		.def("foo", createfoo)
		.def("foo2", createfoo2)
		.def("delbase", deletebase)
		.def("debug_log", [](lua_State* L)->int {
				printf("debug:%s\n", lua_tostring(L, 1));
				return 0;
			})
		.def("printwriteonly", printwriteonly);



	bool ret = lua::runFile(m_L, "D:\\Workspace\\MyDuiLib\\examples\\test_lua\\test.lua");
	if (!ret) {
		const char* s = lua_tostring(m_L, -1);
		std::cout << s << std::endl;
	}
	else {
		lua::table player;
		lua::pop(m_L, &player);
		player.call("speak", player, "hello lua.");
		player.call("speak", player, "hello C++.");
	}
	return 0;
}