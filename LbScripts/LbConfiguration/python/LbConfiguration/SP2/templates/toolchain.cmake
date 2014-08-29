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

include($$ENV{LBUTILSROOT}/data/toolchain.cmake)
