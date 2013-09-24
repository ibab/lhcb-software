message(WARNING "FIXME: the Pythia6 headers in GENSER must be installed")

if(NOT genser_include)
  #message(STATUS "used_gaudi_projects -> ${used_gaudi_projects}")
  set(_hints)
  foreach(_gp ${used_gaudi_projects})
    set(_hints ${${_gp}_DIR} ${_hints})
  endforeach()
  #message(STATUS "_hints -> ${_hints}")

  find_file(genser_include Gen/GENSER/src/pythia6
            HINTS ${CMAKE_SOURCE_DIR} ${_hints}
            PATH_SUFFIXES ../..
            NO_DEFAULT_PATH
            NO_CMAKE_ENVIRONMENT_PATH
            NO_CMAKE_PATH
            NO_SYSTEM_ENVIRONMENT_PATH
            NO_CMAKE_SYSTEM_PATH)
endif()
if(genser_include)
  message(STATUS "using GENSER headers from ${genser_include}")
else()
  message(FATAL_ERROR "cannot find GENSER headers directory")
endif()

include_directories(${genser_include})
