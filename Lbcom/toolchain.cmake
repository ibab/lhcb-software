# Special wrapper to load the declared version of the heptools toolchain.
set(heptools_version 64b)

# Remove the reference to this file from the cache.
unset(CMAKE_TOOLCHAIN_FILE CACHE)

# Find the actual toolchain file.
find_file(CMAKE_TOOLCHAIN_FILE
          NAMES heptools-${heptools_version}.cmake
          HINTS ENV CMTPROJECTPATH
          PATHS ${CMAKE_CURRENT_LIST_DIR}/cmake/toolchain
          PATH_SUFFIXES toolchain)

if(NOT CMAKE_TOOLCHAIN_FILE)
  message(FATAL_ERROR "Cannot find heptools-${heptools_version}.cmake.")
endif()

# Reset the cache variable to have proper documentation.
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_TOOLCHAIN_FILE}
    CACHE FILEPATH "The CMake toolchain file" FORCE)

include(${CMAKE_TOOLCHAIN_FILE})
