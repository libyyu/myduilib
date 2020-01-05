
-- add target
target("mp3_plugin")
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

    add_deps("libmad", "libid3tag")
    add_links("libmad", "libid3tag")
    if not is_plat("iphoneos") and not is_plat("macosx") then
        add_deps("zlib")
    end
    add_includedirs("./", "../include")
    add_includedirs("../../zlib")
    add_includedirs("../libmad-0.15.1b")
    add_includedirs("../libid3tag-0.15.1b")

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("src/**.h")
        add_headerfiles("*.h")
    end
	
	-- add defines
    add_defines("BUILD_AS_DLL", "PLAYER_PLUGIN_DLL_EXPORT")

    add_rules("decoder.shared")

    set_values("vs.folder", "thirdpart/play_plugins")
    -- after build
    after_build(function (target)
        --os.cp(path.join(wdk.sdkdir, "Redist", "wdf", config.arch(), "*.dll"), target:targetdir())     
    end)