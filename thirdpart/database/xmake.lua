
-- add target
target("database")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")	
	-- add includes directory
    add_includedirs(
        "../sqlite3cpp/src"
	)
	-- links
	add_deps("sqlite3cpp")
    add_links("sqlite3cpp")

	-- source files
    add_files("*.cpp|LuaBindDatabase.cpp") 
	
	-- add defines
    add_defines("DATABASE_EXPORTS")
    add_defines("BUILD_AS_DLL")

    add_rules("thirdpart.shared")

    -- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("*.h")
    end

    set_values("vs.folder", "thirdpart/sql")