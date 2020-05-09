-- add target
target("playcenter")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")	
	-- add includes directory
    add_includedirs(
        "./include"
	)

	-- source files
    add_files("src/*.cpp") 
    add_rules("flib.static")
	
	-- add defines
    add_defines("PLAYCENTER_EXPORTS")
    add_defines("BUILD_AS_DLL")

    add_rules("thirdpart.shared")

    -- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("src/*.h")
    end

    set_values("vs.folder", "thirdpart/play_plugins")