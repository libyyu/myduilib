target("YMusic")
    set_kind("binary")
    -- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
    set_languages("cxx11")
    
    add_includedirs("$(projectdir)/thirdpart")
    add_includedirs("$(projectdir)/thirdpart/sqlite3cpp/src")
    add_includedirs("$(projectdir)/thirdpart/vlc/include")
    add_includedirs("$(projectdir)/Demos")
    add_includedirs("$(projectdir)/DuiLib")
    add_includedirs("src")
    add_includedirs("./")

    add_files('*.cpp')
    add_files('src/**.cpp')
    --add_files('YMusic.rc')

    add_defines("BUILD_AS_DLL", "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "CURL_STATICLIB")  
    add_defines("_AFXDLL", "_USEIMM")

    add_ldflags("-SUBSYSTEM:Windows")--, "-SAFESEH:NO")

    add_deps("DuiLib", "webservice", "database", "playcenter", "taglib_plugin", "http_plugin", "MyControls")

    add_options("stack_dump")
    if has_config("stack_dump") then
        add_defines("UILIB_STACK_DUMP")
        add_deps("StackDump")
    end

	--添加头文件
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("*.h", "*.hpp")
        add_headerfiles("src/**.h", "src/**.hpp")
    end

    add_rules("boost")
    set_values("vs.folder", "demos")
target_end()