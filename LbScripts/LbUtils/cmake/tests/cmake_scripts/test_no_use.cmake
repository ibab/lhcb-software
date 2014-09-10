set(CMAKE_MODULE_PATH .. .)

include(GaudiToolchainMacros)

init()
find_projects(projects tools data/projects/BASE/BASE_v1r0/CMakeLists.txt)
set_paths_from_projects(${tools} ${projects})

include(TestMacros)

assert_strequal(projects "BASE")

get_filename_component(base_dir data/projects ABSOLUTE)
assert_strequal(BASE_ROOT_DIR "${base_dir}/BASE/BASE_v1r0")

assert_strequal(CMAKE_PREFIX_PATH "${BASE_ROOT_DIR}/cmake;${BASE_ROOT_DIR};${base_dir};${base_dir}_cmt")
assert_strequal(CMAKE_MODULE_PATH "${BASE_ROOT_DIR}/cmake;${BASE_ROOT_DIR};${CMAKE_CURRENT_LIST_DIR};..;.")
