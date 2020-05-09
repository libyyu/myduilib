target("test_taglib_plugin")
    -- set kind
    set_kind("binary")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
    -- add includes directory
    add_includedirs("$(projectdir)/thirdpart/taglib")
	add_includedirs("$(projectdir)/thirdpart/taglib_plugin/taglib_plugin/src")
	
	add_deps("taglib_plugin", {inherit = true})

	add_files("test.cpp")

	set_values("vs.folder", "examples")

	-- if is_plat("macosx") then
	-- 	add_cxflags(" -install_name @rpath/libtaglib_plugin.dylib")
	-- 	add_ldflags(" -install_name @rpath/libtaglib_plugin.dylib")
	-- 	add_cxflags(" -Xlinker -rpath -Xlinker @loader_path -install_name @rpath/libtaglib_plugin.dylib")
	-- 	add_ldflags(" -Xlinker -rpath -Xlinker @loader_path -install_name @rpath/libtaglib_plugin.dylib")
	-- end