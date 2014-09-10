set(CMAKE_MODULE_PATH .. .)

include(GaudiToolchainMacros)

init()
find_projects(projects tools data/projects/SpecialConventions/CMakeLists.txt)
set_paths_from_projects(${tools} ${projects})

include(TestMacros)

assert_strequal(projects "SPECIALCONVENTIONS;DEVPROJECT;NOVERSION")

get_filename_component(base_dir data/projects ABSOLUTE)
assert_strequal(NOVERSION_ROOT_DIR "${base_dir}/NoVersion")
assert_strequal(DEVPROJECT_ROOT_DIR "${base_dir}/DevProject_v3r9")
assert_strequal(SPECIALCONVENTIONS_ROOT_DIR "${base_dir}/SpecialConventions")

assert_strequal(CMAKE_PREFIX_PATH "${SPECIALCONVENTIONS_ROOT_DIR};${DEVPROJECT_ROOT_DIR};${NOVERSION_ROOT_DIR}/InstallArea/${BINARY_TAG}/cmake;${NOVERSION_ROOT_DIR};${base_dir};${base_dir}_cmt")
assert_strequal(CMAKE_MODULE_PATH "${SPECIALCONVENTIONS_ROOT_DIR};${DEVPROJECT_ROOT_DIR};${NOVERSION_ROOT_DIR}/InstallArea/${BINARY_TAG}/cmake;${NOVERSION_ROOT_DIR};${CMAKE_CURRENT_LIST_DIR};..;.")
