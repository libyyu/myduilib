target("DuiLib")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")
	-- set language: c99, c++11
	set_languages("cxx11")
	
	-- add includes directory
    add_includedirs(
        "./"
    	,"DuiLib"
        ,"../"
    )

    add_files(
        "Control/**.cpp" ,"Control/**.c"
        ,"Core/**.cpp" ,"Core/**.c"
        ,"Layout/**.cpp" ,"Layout/**.c"
        ,"Utils/**.cpp" ,"Utils/**.c"
        ,"*.cpp"
    )

    add_defines("UILIB_EXPORTS", "LUA_BUILD_AS_DLL", "BUILD_AS_DLL", "_USEIMM")

    add_options("stack_dump")
    if has_config("stack_dump") then
        add_defines("UILIB_STACK_DUMP")
        add_deps("StackDump")
    end

    add_rules("app.dll")

    --添加头文件
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles(
            "Control/**.h"
            ,"Core/**.h"
            ,"Layout/**.h"
            ,"Utils/**.h"
            ,"*.h"
        )
    end

    set_values("vs.folder", "dui")