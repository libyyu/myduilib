
-- add target
target("MainThreadTask")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
	
	-- source files
    add_files("*.cpp") 

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("**.h")
    end
	
	-- add defines
    add_defines("BUILD_AS_DLL")
    add_defines("MAINTHREADTASK_DLL_EXPORT", "__MAINTHREADTASK_EXPORT_")

    add_rules("thirdpart.shared")

    set_values("vs.folder", "thirdpart")