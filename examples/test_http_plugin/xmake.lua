target("test_http_plugin")
    -- set kind
    set_kind("binary")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
	-- add includes directory
	add_includedirs("$(projectdir)/thirdpart")
	
	add_deps("http_plugin", {inherit = false})

	add_files("demo.cpp")

	set_values("vs.folder", "examples")