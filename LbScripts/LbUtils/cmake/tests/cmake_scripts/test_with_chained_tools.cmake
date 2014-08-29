set(CMAKE_MODULE_PATH .. .)

include(GaudiToolchainMacros)

init()
find_projects(projects tools data/projects/CHAINTOOL/CHAINTOOL_v1r0/CMakeLists.txt)

include(TestMacros)

assert_strequal(projects "CHAINTOOL;TOOLUSER")

get_filename_component(base_dir data/projects ABSOLUTE)
assert_strequal(TOOLUSER_ROOT_DIR "${base_dir}/TOOLUSER/TOOLUSER_v1r0")
assert_strequal(CHAINTOOL_ROOT_DIR "${base_dir}/CHAINTOOL/CHAINTOOL_v1r0")

assert_strequal(tools "ThirdTool;ATool;AnotherTool")
assert_strequal(ATool_DIR "${base_dir}/ATool/1.5.7")
assert_strequal(AnotherTool_DIR "${base_dir}/AnotherTool/0.0.0/${BINARY_TAG}")
assert_strequal(ThirdTool_DIR "${base_dir}/ThirdTool/3.3")
