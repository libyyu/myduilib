
target("libmad")
    -- make as a static library
    set_kind("static")
	-- set warning all and disable error
	set_warnings("all")
    -- add the common source files
    add_files("*.c|minimad.c")
    if is_plat("windows") then
        add_includedirs("msvc++")
    else
        add_includedirs("gcc")
    end
    add_defines("FPM_DEFAULT", "HAVE_CONFIG_H", "ASO_ZEROCHECK")

	add_rules("thirdpart.static")

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        if is_plat("windows") then
            add_headerfiles("*.h|mad.h")
            add_headerfiles("msvc++/config.h")
        else
            add_headerfiles("*.h")
            add_headerfiles("gcc/config.h")
	    end
    end

	set_values("vs.folder", "thirdpart/play_plugins")
    
