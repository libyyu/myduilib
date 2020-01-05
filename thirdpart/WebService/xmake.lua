target("webservice")
	add_defines("WEBSERVICE_DLL_EXPORT",  "__DLL_EXPORT_", "NOMINMAX")
	set_kind("shared")
	add_files('md5_checksum.cpp', 'UrlService.cpp', 'WebService.cpp', 'WebServiceCatch.cpp', 'ResponseParser.cpp')
	add_rules("thirdpart.shared")
	add_options("genproj")
	add_includedirs("$(projectdir)/thirdpart", "$(projectdir)")
	add_deps("network")
	add_links("network")
	add_rules("zlib.static")
	add_rules("flib.static")

	set_values("vs.folder", "thirdpart/networks")

	-- header files
    if has_config("genproj") then
        add_headerfiles('std.h', 'md5_checksum.h', 'UrlService.h', 'WebService.h', 'WebServiceCatch.h', 'ResponseParser.h')
    end