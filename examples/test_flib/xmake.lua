target("test_flib")
	set_kind("binary")
	add_files("*.cpp")
	--c++ 11
    set_languages("cxx11")

	add_rules("flib.static")
    set_values("vs.folder", "examples")