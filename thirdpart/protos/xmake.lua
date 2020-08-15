
-- add target
target("protos")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
	-- add includes directory
    add_includedirs(
        ".."
		, "../protobuf"
        )
        
    if has_config("luajit") then
        add_rules("luajit.shared")
    else
        add_rules("lua.shared")
    end
	-- links
	add_deps("protobuf")
	add_links("protobuf")

	-- source files
    add_files("*.cpp|main.cpp") 

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("**.h")
    end
	
	-- add defines
    add_defines("BUILD_AS_DLL")
    
    add_rules("thirdpart.shared")

    set_values("vs.folder", "thirdpart/pb")