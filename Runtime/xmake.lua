
target("Runtime")
    add_rules("qt.shared")
    add_files("**.cpp")
    add_headerfiles(("**.hpp"))
    add_defines("DLL_EXPORTS")