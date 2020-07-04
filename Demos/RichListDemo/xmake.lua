target("RichListDemo")
    -- set kind
    set_kind("binary")
    -- set warning all and disable error
    set_warnings("all")
    -- set language: c99, c++11
    set_languages("cxx11")
    --添加链接库
    add_deps("DuiLib")
    add_links("DuiLib")
    -- add includes directory
    add_includedirs(
        "../../DuiLib"
        ,"../../"
    )

    add_files("*.cpp")

    add_ldflags("-SUBSYSTEM:Windows")

    --添加头文件
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("*.h", "*.hpp")
    end

    set_values("vs.folder", "demos")