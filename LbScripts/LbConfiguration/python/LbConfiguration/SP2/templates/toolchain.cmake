# Use command line search path, if defined.
if(EXISTS $${CMAKE_CURRENT_SOURCE_DIR}/searchPath.cmake)
  include($${CMAKE_CURRENT_SOURCE_DIR}/searchPath.cmake)
endif()
# Use the nightly builds search path if needed.
if(EXISTS $${CMAKE_CURRENT_SOURCE_DIR}/nightly.cmake)
  include($${CMAKE_CURRENT_SOURCE_DIR}/nightly.cmake)
  if(EXISTS $$ENV{LHCBNIGHTLIES}/$${nightly_slot}/$${nightly_day}/searchPath.cmake)
    include($$ENV{LHCBNIGHTLIES}/$${nightly_slot}/$${nightly_day}/searchPath.cmake)
  endif()
  set(CMAKE_PREFIX_PATH $$ENV{LHCBNIGHTLIES}/$${nightly_slot}/$${nightly_day} $${CMAKE_PREFIX_PATH})
endif()
if(CMAKE_PREFIX_PATH)
  list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)
endif()

# Special toolchain file that inherits the same heptools version as the
# used projects.
find_file(inherit_heptools_module InheritHEPTools.cmake)
# this check is needed because the toolchain seem to be called a second time
# without the proper cache
if(inherit_heptools_module)
  include($${inherit_heptools_module})
  inherit_heptools()
else()
  message(ERROR "Cannot detect the version of HEPTools required.")
endif()
