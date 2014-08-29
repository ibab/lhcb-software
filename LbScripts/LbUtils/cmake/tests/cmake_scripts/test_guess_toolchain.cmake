set(CMAKE_MODULE_PATH .. .)

include(InheritHEPTools)

set(CMAKE_SOURCE_DIR data/projects/DERIVED/DERIVED_v1r2)
guess_heptools_version(ht_vers)

include(TestMacros)

assert_strequal(ht_vers "69")
