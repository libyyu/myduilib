
target("taglib")
    -- make as a static library
    set_kind("static")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")
    -- defines
    add_defines("TAGLIB_STATIC")

    if not is_plat("windows") then
        add_defines("HAVE_SNPRINTF")
    end 

    -- add the common source files
    add_files("taglib/**.cpp|stdafx.cpp")

	add_rules("thirdpart.static")

    set_values("vs.folder", "thirdpart/tag")
    
