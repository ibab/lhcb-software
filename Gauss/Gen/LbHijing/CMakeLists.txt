################################################################################
# Package: LbHijing
################################################################################
gaudi_subdir(LbHijing v10r0p1)

gaudi_depends_on_subdirs(Gen/GENSER
                         Gen/LbPythia)

string(REPLACE "-pedantic" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wall" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wextra" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Werror=return-type" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
gaudi_add_library(LbHijingLib
                 src/Lib/*.cpp src/Lib/*.F
                 PUBLIC_HEADERS LbHijing
                 LINK_LIBRARIES pythia6forgauss LbPythiaLib)

gaudi_add_module(LbHijing
                 src/component/*.cpp
                 LINK_LIBRARIES LbHijingLib)

