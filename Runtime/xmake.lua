add_requires("vulkansdk", {system = true})
add_requires("vk-bootstrap")

target("Runtime")
    add_rules("qt.shared")
    add_files("**.cpp")
    add_headerfiles(("**.hpp"))
    add_packages("vulkansdk")
    add_packages("vk-bootstrap")
    add_defines("DLL_EXPORTS")