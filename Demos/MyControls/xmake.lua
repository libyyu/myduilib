target("MyControls")
    -- set kind
    set_kind("shared")
    -- set warning all and disable error
    set_warnings("all")
    -- set language: c99, c++11
    set_languages("cxx11")
    --添加链接库
    add_deps("DuiLib")
    -- add includes directory
    add_includedirs(
        "../",
        "../../DuiLib",
        "../../"
    )

    add_files("*.cpp|LuaMyControls.cpp")

    add_defines("_USEIMM", "MYCONTROLS_EXPORTS", "LUA_OBJECT_EXTERN")
    add_rules("app.dll")

    --添加头文件
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("*.h", "*.hpp")
    end

    set_values("vs.folder", "demos")

target("LuaMyControls")
    -- set kind
    set_kind("shared")
    -- set warning all and disable error
    set_warnings("all")
    -- set language: c99, c++11
    set_languages("cxx11")
    --添加链接库
    add_deps("LuaDuiLib", "MyControls")
    -- add includes directory
    add_includedirs(
        "../",
        "../../DuiLib",
        "../../thirdpart/lua51/src",
        "../../"
    )

    add_files("LuaMyControls.cpp")

    add_defines("LUA_BUILD_AS_DLL", "BUILD_AS_DLL", "_USEIMM", "LUA_OBJECT_EXTERN")
    add_rules("app.dll")

    --添加头文件
    add_options("genproj")

    set_values("vs.folder", "demos")