target("test_play_plugin")
    set_kind("binary")
    add_deps("play_plugin", {inherit = false})
    add_files("test.cpp")

    -- header files
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("test.h")
    end

    set_values("vs.folder", "examples")