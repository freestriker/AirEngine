
target("Runtime")
    set_kind("shared")
    add_files("**.cpp")
    add_headerfiles(("**.hpp"))
    add_defines("DLL_EXPORTS")