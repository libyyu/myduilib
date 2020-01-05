
-- add target
target("playerQ")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
	-- source files
    add_files("src/*.cpp")
    add_files("src/*.c")
    add_files("src/openal32/*.c")
    add_files("src/openal32/alc/*.c|*_inc.c|*_neon.c")
    add_files("src/openal32/alc/backends/dsound.c")
    add_files("src/openal32/alc/backends/loopback.c")
    add_files("src/openal32/alc/backends/mmdevapi.c")
    add_files("src/openal32/alc/backends/null.c")
    add_files("src/openal32/alc/backends/wave.c")
    add_files("src/openal32/alc/backends/winmm.c")

    add_includedirs("./", "src/openal32", "src/openal32/Include")

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("src/**.h|*mixer_defs.h")
        add_headerfiles("*.h")
    end
	
	-- add defines
    add_defines("BUILD_AS_DLL", "PLAYERQ_DLL_EXPORT", 
        "AL_ALEXT_PROTOTYPES",
        "AL_BUILD_LIBRARY",
        "AL_LIBTYPE_STATIC",
        "_CRT_NONSTDC_NO_DEPRECATE",
        "strcasecmp=_stricmp",
        "strncasecmp=_strnicmp")
    add_links("openal32")

    add_rules("thirdpart.shared")

    set_values("vs.folder", "thirdpart/play_plugins")
    -- after build
    after_build(function (target)
        --os.cp(path.join(wdk.sdkdir, "Redist", "wdf", config.arch(), "*.dll"), target:targetdir())     
    end)