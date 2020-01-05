
target("protobuf")
    -- make as a static library
    set_kind("static")
	-- set warning all and disable error
	set_warnings("all")
	-- set language:  c++11
	set_languages("cxx11")
    -- defines
    add_defines("_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS")

    -- add the common source files
    add_files("google/protobuf/*.cc|testing/*.*|testdata/*.*|compiler/*.*|*test*")
	add_files("google/protobuf/io/*.cc|*test*")
	add_files("google/protobuf/compiler/parser.cc")
	add_files("google/protobuf/compiler/importer.cc")
	if is_plat("windows") then
		add_files("google/protobuf/stubs/*.cc|*test*|atomicops_*")
		add_files("google/protobuf/stubs/atomicops_internals_x86_msvc.cc")
	elseif is_plat("android") then
		add_files("google/protobuf/stubs/*.cc|*test*|atomicops_*")
		add_files("google/protobuf/stubs/atomicops_internals_x86_gcc.cc")
	else
		add_files("google/protobuf/stubs/*.cc|*test*")
	end

	add_includedirs("../", "../protobuf")
	add_rules("thirdpart.static")

	set_values("vs.folder", "thirdpart/pb")
    
