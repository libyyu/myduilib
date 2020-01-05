
-- add target
target("sqlite3cpp")
    -- set kind
    set_kind("static")
	-- set warning all and disable error
	set_warnings("all")	
	-- add includes directory
    add_includedirs(
        "../sqlite3/src"
	)
	-- links
	add_deps("sqlite3")
	add_links("sqlite3")

	-- source files
    add_files("src/*.cpp|CodingConv.cpp") 
    if is_plat("windows") then
        add_files("src/CodingConv.cpp")
    end
	
	-- add defines
    add_defines("SQLITE3CPP_EXPORTS")
    if is_kind("shared") then
        add_defines("SQLITE3CPP_DLL")
    end

    add_rules("thirdpart.static")

    -- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("src/**.h")
    end

    set_values("vs.folder", "thirdpart/sql")