
target("json")
    -- make as a static library
    set_kind("static")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")
	
    -- add the common source files
    add_files("lib_json/*.cpp")
	add_rules("thirdpart.static")

	set_values("vs.folder", "thirdpart")
    
