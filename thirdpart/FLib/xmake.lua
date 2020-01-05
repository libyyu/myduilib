
-- add target
target("FLib")
    -- set kind
    set_kind("shared")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")  	
	-- source files
    add_files("src/*.cpp") 

	-- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("src/*.h")
    end
	
	-- add defines
    add_defines("BUILD_AS_DLL", "FLIB_DLL_EXPORTS")

    add_rules("thirdpart.shared")

    set_values("vs.folder", "thirdpart/FStdLib")
    -- after build
    after_build(function (target)
        --os.cp(path.join(wdk.sdkdir, "Redist", "wdf", config.arch(), "*.dll"), target:targetdir())     
    end)

-- add target
target("libFLib")
    -- set kind
    set_kind("static")
    set_basename("FLib")
    -- set warning all and disable error
    set_warnings("all")
    -- set language:  c++11
    set_languages("cxx11")      
    -- source files
    add_files("src/*.cpp") 

    -- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("src/*.h")
    end
    
    -- add defines
    add_defines("FLIB_STATIC_EXPORT")

    add_rules("thirdpart.static")

    set_values("vs.folder", "thirdpart/FStdLib")

    -- after build
    after_build(function (target)
        --os.cp(path.join(wdk.sdkdir, "Redist", "wdf", config.arch(), "*.dll"), target:targetdir())     
    end)