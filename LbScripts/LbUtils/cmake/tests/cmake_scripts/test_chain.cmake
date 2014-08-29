set(CMAKE_MODULE_PATH .. .)

include(GaudiToolchainMacros)

init()
find_projects(projects tools data/projects/CHAIN/CHAIN_v2r3/CMakeLists.txt)

include(TestMacros)

assert_strequal(projects "CHAIN;DERIVED;BASE")

get_filename_component(base_dir data/projects ABSOLUTE)
assert_strequal(BASE_ROOT_DIR "${base_dir}/BASE/BASE_v1r0")
assert_strequal(DERIVED_ROOT_DIR "${base_dir}/DERIVED/DERIVED_v1r2")
assert_strequal(CHAIN_ROOT_DIR "${base_dir}/CHAIN/CHAIN_v2r3")
