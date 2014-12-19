# Use the nightly builds search path if needed.
if(EXISTS $${CMAKE_CURRENT_SOURCE_DIR}/build.conf)
  file(STRINGS $${CMAKE_CURRENT_SOURCE_DIR}/build.conf build_conf)
  foreach(l $${build_conf})
    if(l MATCHES "^ *([a-zA-Z_][a-zA-Z0-9_]*)=([^ ]*) *$$")
      set($${CMAKE_MATCH_1} "$${CMAKE_MATCH_2}")
    endif()
  endforeach()
endif()

if(nightly_slot)
  if(EXISTS $$ENV{LHCBNIGHTLIES}/$${nightly_slot}/$${nightly_day}/searchPath.cmake)
    include($$ENV{LHCBNIGHTLIES}/$${nightly_slot}/$${nightly_day}/searchPath.cmake)
  endif()
  set(CMAKE_PREFIX_PATH $$ENV{LHCBNIGHTLIES}/$${nightly_slot}/$${nightly_day} $${CMAKE_PREFIX_PATH})
endif()
# Use command line search path, if defined.
if(EXISTS $${CMAKE_CURRENT_SOURCE_DIR}/searchPath.cmake)
  include($${CMAKE_CURRENT_SOURCE_DIR}/searchPath.cmake)
endif()

if(CMAKE_PREFIX_PATH)
  list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)
endif()

include($$ENV{LBUTILSROOT}/data/toolchain.cmake)
