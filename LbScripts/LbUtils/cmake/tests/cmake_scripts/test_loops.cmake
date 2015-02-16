set(CMAKE_MODULE_PATH .. .)

include(GaudiToolchainMacros)

init()
find_projects(projects tools data/projects/LOOP/LOOP_v1r0/CMakeLists.txt)
