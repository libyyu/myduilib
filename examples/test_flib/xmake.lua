target("test_flib")
	set_kind("binary")
	add_files("*.cpp")

	add_rules("flib.static")
    set_values("vs.folder", "examples")