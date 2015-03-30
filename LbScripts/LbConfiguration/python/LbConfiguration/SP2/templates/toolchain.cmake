# Use lb-dev command line search path, if defined.
if(EXISTS $${CMAKE_CURRENT_SOURCE_DIR}/searchPath.cmake)
  include($${CMAKE_CURRENT_SOURCE_DIR}/searchPath.cmake)
endif()

if(CMAKE_PREFIX_PATH)
  list(REMOVE_DUPLICATES CMAKE_PREFIX_PATH)
endif()

include($$ENV{LBUTILSROOT}/data/toolchain.cmake)
