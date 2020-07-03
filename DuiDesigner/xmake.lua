target("DuiDesigner")
    -- set kind
    set_kind("binary")
	-- set warning all and disable error
	set_warnings("all")
	-- set language: c99, c++11
	set_languages("cxx11")
	
	-- add includes directory
    add_includedirs(
        "./"
    	,"../DuiLib"
    	,"./DuiDesigner/third_party/tinyxml"
    )

    add_files(
        "DuiDesigner/**.cpp"
        ,"DuiDesigner/DuiDesigner.rc"
    )

    add_deps("DuiLib")

    add_ldflags("-SUBSYSTEM:Windows")

    add_defines("UILIB_EXPORTS", "_USEIMM", 
    	"_AFXDLL", 
    	"COMPILE_DEFINITIONS", 
    	"_BIND_TO_CURRENT_CRT_VERSION", 
    	"_BIND_TO_CURRENT_MFC_VERSION")
	
    --添加头文件
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles(
            "DuiDesigner/**.h", "DuiDesigner/**.hpp"
        )
    end

    set_values("vs.folder", "dui")