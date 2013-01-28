# use_heptools(heptools_version)
#
# Look for the required version of the HEPTools toolchain and replace the
# current toolchain file with it.
#
# WARNING: this macro must be called from a toolchain file
#
macro(use_heptools heptools_version)
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

  message(STATUS "Using heptools version ${heptools_version}")
  include(${CMAKE_TOOLCHAIN_FILE})
endmacro()
