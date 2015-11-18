################################################################################
# Package: LbOniaPairs
################################################################################
gaudi_subdir(LbOniaPairs v2r1p1)

gaudi_depends_on_subdirs(Gen/GENSER
                         Gen/LbPythia)

find_package(Boost)

string(REPLACE "-pedantic" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wall" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wextra" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Werror=return-type" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
gaudi_add_library(LbOniaPairsLib
                  src/Lib/*.cpp
                  src/Lib/*.F
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost pythia6forgauss LbPythiaLib)

gaudi_add_module(LbOniaPairs
                 src/Component/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost pythia6forgauss LbPythiaLib LbOniaPairsLib)

gaudi_env(SET LBONIAPAIRSOPTS \${LBONIAPAIRSROOT}/options)
