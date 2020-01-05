-- define target
target("test")

    -- set kind
    set_kind("binary")

    -- add files
    add_files("test.cpp")

    -- set lanagures
    set_languages("cxx11")
    
    --links
    add_links("pthread")

