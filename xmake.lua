set_project("AirEngine")

add_rules("mode.debug", "mode.release")

add_requires("boost", {alias = "boost", configs = {all = true}})
add_requires("vulkansdk", {alias = "vulkansdk"})
add_requires("vk-bootstrap", {alias = "vk-bootstrap"})

add_packages("boost")
add_packages("vulkansdk")
add_packages("vk-bootstrap")

set_languages("cxx17")
add_cxxflags("cl::/Zc:__cplusplus /permissive-")
add_defines("NOMINMAX")
add_ldflags("/SUBSYSTEM:CONSOLE")
set_arch("x64")

includes("Runtime", "Demo")