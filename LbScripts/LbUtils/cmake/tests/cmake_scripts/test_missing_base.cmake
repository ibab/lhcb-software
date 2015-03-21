set(CMAKE_MODULE_PATH .. .)

include(GaudiToolchainMacros)

init()
set(CMAKE_PREFIX_PATH data/projects/NOBASE/NOBASE_HEAD ${CMAKE_PREFIX_PATH})
find_projects(projects tools data/projects/NOBASE/NOBASE_HEAD/CMakeLists.txt)
