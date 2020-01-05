target("webservicedemo")
	set_kind("binary")
	add_files("WebService/*.cpp")
	add_includedirs("$(projectdir)/thirdpart")
	add_deps('webservice', {inherit = false})

	set_values("vs.folder", "thirdpart/networks")
target_end()