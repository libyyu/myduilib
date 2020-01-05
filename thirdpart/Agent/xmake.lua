
target("Agent")
	add_defines("AGENT_DLL_EXPORT", "__DLL_EXPORT_")
	set_kind("shared")
	add_includedirs("$(projectdir)/thirdpart")
	
	add_rules("boost")
	add_rules("thirdpart.shared")
	add_deps("json")
	add_links("json")
	add_rules("flib.static")
	add_files("src/**.cpp")


	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("src/**.h")
    end

    set_values("vs.folder", "thirdpart")