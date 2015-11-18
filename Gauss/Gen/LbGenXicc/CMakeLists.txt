################################################################################
# Package: LbGenXicc
################################################################################
gaudi_subdir(LbGenXicc v5r0p1)

gaudi_depends_on_subdirs(Gen/GenXicc
                         Gen/LbHard)

string(REPLACE "-pedantic" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wall" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wextra" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Werror=return-type" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
gaudi_add_library(LbGenXiccLib
                  src/Lib/*.cpp
                  src/Lib/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES GenXicc LbPythiaLib)

gaudi_add_module(LbGenXicc
                 src/component/*.cpp
                 LINK_LIBRARIES GenXicc LbHardLib LbPythiaLib LbGenXiccLib)

