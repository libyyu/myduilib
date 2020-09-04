target("lpeg")
	add_defines("LPEG_DLL_EXPORT",  "__DLL_EXPORT_")
	set_kind("shared")
	add_files('*.c')

	if has_config("luajit") then
        add_rules("luajit.shared")
    else
        add_rules("lua.shared")
    end

    add_defines("BUILD_AS_DLL")
    add_rules("thirdpart.shared")

	set_values("vs.folder", "thirdpart")

	-- header files
	add_options("genproj")
    if has_config("genproj") then
        add_headerfiles('*.h')
    end