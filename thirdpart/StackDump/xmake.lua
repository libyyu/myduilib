
target("StackDump")
	add_defines("WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS", "WIN32")
	set_kind("shared")
    add_files("*.cpp")
    add_rules("thirdpart.shared")
    add_options("genproj")
    if has_config("genproj") then
        add_headerfiles("*.h", "*.hpp")
    end

    set_values("vs.folder", "thirdpart")