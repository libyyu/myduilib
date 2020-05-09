target("test_playcenter")
    -- set kind
    set_kind("binary")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
	-- add includes directory
	add_includedirs("$(projectdir)/thirdpart")
	
	add_deps("playcenter", {inherit = false})

	add_files("test.cpp")

	set_values("vs.folder", "examples")