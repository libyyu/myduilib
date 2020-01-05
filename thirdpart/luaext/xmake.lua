-- add target
target("luaext")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")	
	-- add includes directory
    add_includedirs("../database", "../sqlite3cpp/src")
    add_includedirs("../taglib", "../taglib_plugin/taglib_plugin/src")
    add_includedirs("../vlc/src")
	-- links
	add_deps("database", "playcenter",  "taglib_plugin")
    add_links("database", "playcenter",  "taglib_plugin")
	if has_config("luajit") then
        add_includedirs("../luajit/src")
        add_deps("luajit")
    else
        add_includedirs("../lua51/src")
        add_deps("lua51")
    end

	-- source files
    add_files("*.cpp") 
	
	-- add defines
    add_defines("BUILD_AS_DLL")
    if is_plat("windows") then
        add_defines("LUA_BUILD_AS_DLL")
    end
    add_rules("flib.static")
    add_rules("thirdpart.shared")

    -- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("*.h")
    end

    set_values("vs.folder", "thirdpart")