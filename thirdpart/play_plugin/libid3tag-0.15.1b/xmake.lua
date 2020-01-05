
target("libid3tag")
    -- make as a static library
    set_kind("static")
	-- set warning all and disable error
	set_warnings("all")
    -- add the common source files
    add_files("*.c")
    if is_plat("windows") then
    	add_includedirs("msvc++")
        add_includedirs("../../zlib")
    else
        
    end
    
    add_defines("HAVE_CONFIG_H")

	add_rules("thirdpart.static")

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("*.h")
        if is_plat("windows") then
	    	add_headerfiles("msvc++/config.h")
	    end
    end

	set_values("vs.folder", "thirdpart/play_plugins")
    
