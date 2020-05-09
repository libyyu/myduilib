target("test_webservice")
	set_kind("binary")
	add_files("*.cpp")
	add_includedirs("$(projectdir)/thirdpart")
	add_deps('webservice', {inherit = false})

	set_values("vs.folder", "examples")
target_end()