# this check is needed because the toolchain is called when checking the
# compiler (without the proper cache)
if(NOT CMAKE_SOURCE_DIR MATCHES "CMakeTmp")

  find_file(default_toolchain NAMES GaudiDefaultToolchain.cmake
            HINTS ${CMAKE_SOURCE_DIR}/cmake)
  if(default_toolchain)
    include(${default_toolchain})
  else()
    message(FATAL_ERROR "Cannot find GaudiDefaultToolchain.cmake")
  endif()

endif()
