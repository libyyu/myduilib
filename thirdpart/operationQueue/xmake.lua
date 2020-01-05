
target("operationQ")
    -- make as a static library
    set_kind("static")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")
    
    -- add the common source files
    add_files("*.cpp")
    add_defines("OPERATIONQUEUE_STATIC_EXPORT")

    -- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("operationQueue.h")
    end

    -- defines
    add_rules("boost")
	add_rules("thirdpart.static")

    set_values("vs.folder", "thirdpart")
    
