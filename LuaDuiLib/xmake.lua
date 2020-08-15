target("LuaDuiLib")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")
	-- set language: c99, c++11
	set_languages("cxx11")
    --添加链接库
    add_deps("DuiLib")
    add_links("DuiLib")
	-- add includes directory
    add_includedirs(
    	"../DuiLib"
        ,"../"
    )
    if has_config("luajit") then
        add_rules("luajit.shared")
    else
        add_rules("lua.shared")
    end

    add_files("*.cpp", "*.c")

    add_defines("UILIB_STACK_DUMP", "BUILD_AS_DLL", "LUA_OBJECT_EXTERN", "USE_DLL_EXPORT")
    add_rules("app.dll")

    add_options("stack_dump")
    if has_config("stack_dump") then
        add_defines("UILIB_STACK_DUMP")
        add_deps("StackDump")
    end

    --添加头文件
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("*.h", "*.hpp")
        add_headerfiles("../base/lua_*.hpp")
        add_headerfiles("../base/LuaEnv.hpp")
    end

    set_values("vs.folder", "dui")