
-- add target
target("general_plugin")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
	-- source files
    add_files("src/**.cpp") 

    add_rules("boost")
    add_rules("flib.static")

    add_rules("ffmpeg.shared")
	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("src/**.h")
    end

    add_includedirs("./", "../include")
	
	-- add defines
    add_defines("BUILD_AS_DLL", "PLAYER_PLUGIN_DLL_EXPORT")

    add_rules("decoder.shared")
    add_rules("flib.static")

    set_values("vs.folder", "thirdpart/play_plugins")
    -- after build
    after_build(function (target)
        --os.cp(path.join(wdk.sdkdir, "Redist", "wdf", config.arch(), "*.dll"), target:targetdir())     
    end)