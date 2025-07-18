
target("basics")
  set_kind("binary")
  add_files("basics/src/**.cpp")
  add_includedirs("basics/include/")

target("emplace", {kind = "binary", files = "emplace/src/**.cpp",add_includedirs = "emplace/include/"})
target("hierarchy", {kind = "binary", files = "hierarchy/src/**.cpp",add_includedirs = "hierarchy/include/"})

