set_project("AirEngine")

add_rules("mode.debug", "mode.release")

add_requires("boost", {configs = {all = true}})

add_packages("boost")

set_languages("cxx17")
add_cxxflags("cl::/Zc:__cplusplus /permissive-")
add_defines("NOMINMAX")

set_arch("x64")

includes("Runtime", "Demo")