
includes('libmad-0.15.1b', 'libid3tag-0.15.1b', 'wav_plugin')
if is_plat("windows") then
    includes('mp3_plugin', 'general_plugin')
    includes('playerQ')
end

if is_plat("windows") then
-- add target
target("play_plugin")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
	-- source files
    add_files("src/**.cpp|test.cpp") 

    add_includedirs("include")

    add_rules("flib.static")

    add_deps("mp3_plugin", {inherit = false})
    add_deps("wav_plugin", {inherit = false})
    add_deps("general_plugin", {inherit = false})
    add_deps("operationQ")
    add_includedirs("./","../", "../../")

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("src/**.h")
        add_headerfiles("include/*.h")
    end
	
	-- add defines
    add_defines("BUILD_AS_DLL", "PLAYER_PLUGIN_DLL_EXPORT", "OPERATIONQUEUE_STATIC_EXPORT")

    add_rules("thirdpart.shared")

    set_values("vs.folder", "thirdpart/play_plugins")
    -- after build
    after_build(function (target)
        --os.cp(path.join(wdk.sdkdir, "Redist", "wdf", config.arch(), "*.dll"), target:targetdir())     
    end)

target("play_plugin_test")
    set_kind("binary")
    add_deps("play_plugin", {inherit = false})
    add_files("test.cpp")

    -- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("test.h")
    end

    set_values("vs.folder", "thirdpart/play_plugins")

end