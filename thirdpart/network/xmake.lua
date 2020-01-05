target("network")
	add_defines("WIN32_LEAN_AND_MEAN", "NETWORK_DLL_EXPORT", "__DLL_EXPORT_")
	set_kind("shared")
	add_files("*.cpp", "src/*.cpp")
	add_rules("thirdpart.shared")
	add_includedirs("$(projectdir)/thirdpart")
	set_languages("c99", "cxx11")
	
	add_rules("boost")
	add_rules("flib.static")

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("**.h")
    end

    set_values("vs.folder", "thirdpart/networks")