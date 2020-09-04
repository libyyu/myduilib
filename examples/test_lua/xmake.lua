target("test_lua")
	set_kind("binary")
	add_files("*.cpp")
	--c++ 11
    set_languages("cxx11")

	if has_config("luajit") then
        add_rules("luajit.shared")
    else
        add_rules("lua.shared")
    end
    set_values("vs.folder", "examples")