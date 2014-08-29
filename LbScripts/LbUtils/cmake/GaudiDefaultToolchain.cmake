set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_MODULE_PATH})

include(GaudiToolchainMacros)

init()
find_projects(projects tools ${CMAKE_SOURCE_DIR}/CMakeLists.txt)

if(heptools_version)
  include(UseHEPTools)
  use_heptools(${heptools_version})
else()
  include(InheritHEPTools)
  inherit_heptools()
endif()

set_paths_from_projects(${tools} ${projects})
