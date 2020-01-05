
target("zlib")
    -- make as a static library
    set_kind("static")
	-- set warning all and disable error
	set_warnings("all")

    -- add the common source files
    add_files("*.c|minigzip.c")
	add_rules("thirdpart.static")

	set_values("vs.folder", "thirdpart")
    
