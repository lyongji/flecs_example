add_rules("mode.debug", "mode.release")

set_encodings("utf-8")
set_languages("c++23")
add_requires("flecs")
add_packages("flecs")

includes("**/xmake.lua")--搜索目录下所有子构建

