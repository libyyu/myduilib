
-----------------------------------------------------
-- add target
target("lua51")
    -- disable c99(/TP) for windows
    set_languages("c89")
    -- set warning all and disable error
    set_warnings("all")

    -- make as a shared library
    set_kind("shared")
    -- add the common source files
    add_files("src/*.c|lua.c|luac.c|print.c") 
    if is_plat("windows") then
        add_defines("WIN32_LEAN_AND_MEAN", "LUA_BUILD_AS_DLL")
    end

    add_defines("LUA_DLL_EXPORT")

    add_rules("thirdpart.shared")

    -- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("src/*.h")
    end

    set_values("vs.folder", "thirdpart/lua")

-- add target
-- target("liblua51")
--     set_basename("lua51")
--     -- disable c99(/TP) for windows
--     set_languages("c89")
--     -- set warning all and disable error
--     set_warnings("all")

--     -- make as a shared library
--     set_kind("static")
--     -- add the common source files
--     add_files("src/*.c|lua.c|luac.c|print.c") 
--     if is_plat("windows") then
--         add_defines("WIN32_LEAN_AND_MEAN")
--     end
--     -- add defines
--     add_defines("LUA_STATIC_EXPORT")

--     add_rules("thirdpart.static")

--     -- header files
--     add_options("genproj")
--     if has_config("genproj") then
--         add_headerfiles("src/*.h")
--     end

--     set_values("vs.folder", "thirdpart/lua")

-- target("luac")
--     -- disable c99(/TP) for windows
--     set_languages("c89")
--     -- set warning all and disable error
--     set_warnings("all")

--     add_deps("lua51")
--     set_kind("binary")
--     add_files("src/luac.c","src/print.c")
--     add_links("lua51")
    
--     add_defines("WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "LUA_BUILD_AS_DLL")

-- target("lua")
--     -- disable c99(/TP) for windows
--     set_languages("c89")
--     -- set warning all and disable error
--     set_warnings("all")
    
--     add_deps("lua51")
--     set_kind("binary")
--     add_files("src/lua.c")
--     add_links("lua51")
--     add_defines("WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "LUA_BUILD_AS_DLL")
