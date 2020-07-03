-- project
set_project("DuiLib")

-- version
set_version("2.2.3")

-- set xmake min version
set_xmakever("2.2.3")

set_warnings("all")

-- for the windows platform (msvc)
if is_plat("windows") then 
    -- add some defines only for windows
    add_cxflags(
    "-EHsc"
    , "-nologo"
    )

    -- no msvcrt.lib
    add_ldflags(
    "-NOD"
    , "-nologo"
    , "-nodefaultlib:\"LIBCMT.lib\""
    , "-nodefaultlib:\"LIBC.lib\""
    , "-nodefaultlib:\"msvcrt.lib\""
    )
    if is_mode("debug") then
        add_cxflags("-ZI", "-MDd", "-FS")
        -- if is_arch("x86") then
        --     add_cxflags("-ZI")
        -- else
        --     add_cxflags("-Zi")
        -- end
        add_defines("_DEBUG","DEBUG")
        --if not is_kind("static") then
            set_symbols("debug")
    -- end
    else
        add_cxflags("-Zi", "-MD", "-FS")
        add_defines("_NDEBUG", "NDEBUG")
        --if not is_kind("static") then
            --set_symbols("debug")
    -- end
    end
    --添加全局宏
    add_defines(
        "_CRT_SECURE_NO_WARNINGS", "_WIN32"
    )
    if is_arch("x64") then
        add_defines("_WIN64")
    end

    add_syslinks("ws2_32",
        "user32",
        "kernel32",
        "gdi32",
        "winspool",
        "comdlg32",
        "advapi32",
        "shell32",
        "ole32",
        "oleaut32",
        "uuid",
        "odbc32",
        "odbccp32")
elseif is_plat("android") then
    add_cxflags("-fomit-frame-pointer")
    add_defines("__ANDROID__")
    add_syslinks("m", "c", "android") 
elseif is_plat("iphoneos") then
    add_cxflags("-fomit-frame-pointer")
    add_defines("__IOS__")
    add_ldflags("-ObjC")
    add_arflags("-ObjC")
    add_shflags("-ObjC")
    add_syslinks("pthread", "dl", "m", "c")
elseif is_plat("macosx") then
    add_cxflags("-fomit-frame-pointer")
    add_defines("__MACOSX__")
    add_ldflags("-ObjC")
    add_arflags("-ObjC")
    add_shflags("-ObjC")
    add_ldflags("-all_load")
    add_arflags("-all_load")
    add_shflags("-all_load")
    add_defines("FIXED_POINT", "USE_KISS_FFT", "HAS_PTHREAD", "HAVE_PTHREAD")
    add_syslinks("pthread", "dl", "m", "c")
end

--设置最终文件目录
if is_plat("windows") then
    add_defines("ROOT_DIR=\"" .. os.projectdir():gsub("\\", "\\\\") .. "\"")
    set_targetdir("$(projectdir)/$(mode)/$(plat)/$(arch)")
else
    add_defines("ROOT_DIR=\"" .. os.projectdir() .. "\"")
    set_targetdir("$(projectdir)/$(mode)/$(plat)")
end
--设置中间输出目录
set_objectdir("$(buildir)/.objs/$(plat)/$(arch)/$(mode)")
--设置链接目录
add_linkdirs("$(projectdir)/libs/$(plat)/$(mode)/$(arch)", --第一优先级
    "$(projectdir)/libs/$(plat)/$(mode)/$(arch)", 
    "$(projectdir)/libs/$(plat)/$(mode)",
    "$(projectdir)/libs/$(plat)", 
    "$(projectdir)/$(mode)/$(plat)/$(arch)",
    "$(projectdir)/$(mode)/$(plat)",
    "$(projectdir)/$(mode)/$(plat)/$(arch)/plugins",
    "$(projectdir)/$(mode)/$(plat)/plugins")

option("genproj")
    set_default(false)
    set_showmenu(true)
option_end()
option("custom_memory")
    set_default(false)
    set_showmenu(true)
option_end()

option("stack_dump")
    set_default(true)
    set_showmenu(true)
option_end()

option("boost_dir")
    set_default("")
    set_showmenu(true)
option_end()

rule("boost")
	after_load(function (target)
		local boost_dir = get_config("boost_dir")
		target:add("includedirs", boost_dir)
        target:add("defines", "HAVE_BOOST")
		if is_plat("windows") then
			target:add("linkdirs", boost_dir.."/stage/msvc/$(arch)/lib")
		else
			target:add("linkdirs", boost_dir.."/stage/$(plat)/lib")
			target:add("links", 'boost_thread')
			target:add("links", 'boost_system')
			target:add("links", 'boost_exception')
		end
    end)
rule_end()

rule("app.dll")
    after_load(function (target)
        if is_plat("windows") then
            target:set("targetdir", "$(projectdir)/$(mode)/$(plat)/$(arch)")
        else
            target:set("targetdir", "$(projectdir)/$(mode)/$(plat)")
        end
    end)
rule_end()

rule("thirdpart.shared")
    after_load(function (target)
        if is_plat("windows") then
            target:set("targetdir", "$(projectdir)/$(mode)/$(plat)/$(arch)")
        else
            target:set("targetdir", "$(projectdir)/$(mode)/$(plat)")
        end
    end)
rule_end()

rule("thirdpart.static")
    after_load(function (target)
        if is_plat("windows") then
            target:set("targetdir", "$(projectdir)/libs/$(plat)/$(mode)/$(arch)")
        else
            target:set("targetdir", "$(projectdir)/libs/$(plat)/$(mode)")
        end
    end)
rule_end()

rule("flib.shared")
    after_load(function (target)
    	target:add("deps", "FLib")
    	target:add("links", 'FLib')
    	target:add("includedirs", "$(projectdir)/thirdpart/FLib/src")
    end)
rule_end()

rule("flib.static")
    after_load(function (target)
    	target:add("deps", "libFLib")
    	target:add("links", 'FLib')
    	target:add("includedirs", "$(projectdir)/thirdpart/FLib/src")
    end)
rule_end()

rule("ffmpeg.shared")
    after_load(function (target)
    	target:add("includedirs", "$(projectdir)/thirdpart/play_plugin/general_plugin/ext/ffmpeg")
    	target:add("linkdirs", "$(projectdir)/thirdpart/play_plugin/general_plugin/ext/$(mode)")
    	target:add("links", 'avcodec', 'avdevice', 'avfilter', 'avformat', 'avutil', 'swresample', 'swscale')
    end)
rule_end()

rule("ffmpeg.static")
    after_load(function (target)
    	target:add("includedirs", "$(projectdir)/thirdpart/ffmpeg/include")
    	target:add("linkdirs", "$(projectdir)/thirdpart/ffmpeg/bin")
    	target:add("links", 'avcodec', 'avdevice', 'avfilter', 'avformat', 'avutil', 'swresample', 'swscale')
    end)
rule_end()

rule("decoder.shared")
    after_load(function (target)
        if is_plat("windows") then
            target:set("targetdir", "$(projectdir)/$(mode)/$(plat)/$(arch)/plugins")
        else
            target:set("targetdir", "$(projectdir)/$(mode)/$(plat)/plugins")
        end
    end)
rule_end()

rule("zlib.static")
    after_load(function (target)
        if not is_plat("iphoneos") and not is_plat("macosx") then
            target:add("deps", "zlib")
            target:add("links", "zlib")
        end
    end)
rule_end()

if is_plat("windows") then
    includes(
        "thirdpart"
        , "DuiLib"
        , "DuiDesigner"
        , "LuaDuiLib"
        , "Demos"
    )
else
    includes(
        "thirdpart"
        , "Demos"
    )
end

includes("examples")