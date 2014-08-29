set(CMAKE_MODULE_PATH .. .)

include(GaudiToolchainMacros)

init()
find_projects(projects tools data/projects/PICKVERSIONS/PICKVERSIONS_v5r6/CMakeLists.txt)

include(TestMacros)

assert_strequal(projects "PICKVERSIONS;WITHVERSIONS")

get_filename_component(base_dir data/projects ABSOLUTE)
assert_strequal(PICKVERSIONS_ROOT_DIR "${base_dir}/PICKVERSIONS/PICKVERSIONS_v5r6")
assert_strequal(WITHVERSIONS_ROOT_DIR "${base_dir}/WITHVERSIONS/WITHVERSIONS_v1r0")

assert_strequal(tools "VersionedTool")
assert_strequal(VersionedTool_DIR "${base_dir}/VersionedTool/2.2")
