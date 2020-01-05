
target("sqlite3")
    -- make as a static library
    set_kind("static")
	-- set warning all and disable error
	set_warnings("all")
    
    -- add the common source files
    if is_plat("windows") then
        add_defines("SQLITE_HAS_CODEC")
        add_files("src/*.c|fts3*|pager.c")
    else
        add_files("src/*.c|fts3*|crypt.c")
    end
	add_rules("thirdpart.static")

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        --add_headerfiles("src/*.h")
    end

    set_values("vs.folder", "thirdpart/sql")
    
