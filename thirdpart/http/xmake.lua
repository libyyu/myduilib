
-- add target
target("http_plugin")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
	-- add includes directory
	add_includedirs("$(projectdir)/thirdpart")
	-- links
	if is_plat("windows") then
		add_syslinks("ws2_32", "wldap32", "Crypt32")
		add_links('curl_a', 'ssleay32', 'libeay32')
	elseif is_plat("android") then
	elseif is_plat("macosx") then
		add_links("ldap", "curl")
	end

	-- source files
    add_files("*.cpp|Test.cpp|demo.cpp") 

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("*.h")
    end
	
	-- add defines
    add_defines("CURL_STATICLIB", "HTTP_DLL_EXPORT", "BUILD_AS_DLL")

	add_rules("thirdpart.shared")

	set_values("vs.folder", "thirdpart/networks")

target("http_plugin_test")
    -- set kind
    set_kind("binary")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
	-- add includes directory
	add_includedirs("$(projectdir)/thirdpart")
	
	add_deps("http_plugin", {inherit = false})

	add_files("demo.cpp")

	set_values("vs.folder", "thirdpart/networks")