set_project("AirEngine")

add_rules("mode.debug", "mode.release")

add_requires("boost", {alias = "boost", configs = {all = true}})
add_requires("vulkansdk", {alias = "vulkansdk"})
add_requires("vk-bootstrap", {alias = "vk-bootstrap"})
add_requires("vcpkg::cityhash", {alias = "cityhash"})
add_requires("vcpkg::rttr", {alias = "rttr"})
add_requires("vcpkg::glm", {alias = "glm"})

add_packages("boost")
add_packages("vulkansdk")
add_packages("vk-bootstrap")
add_packages("cityhash")
add_packages("rttr")
add_packages("glm")

set_languages("cxx17")
add_defines("NOMINMAX")
set_arch("x64")
add_ldflags("/SUBSYSTEM:CONSOLE")

includes("Runtime", "Demo")