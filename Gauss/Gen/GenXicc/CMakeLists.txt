################################################################################
# Package: GenXicc
################################################################################
gaudi_subdir(GenXicc v2r1p1)

gaudi_depends_on_subdirs(Gen/GENSER)

include_directories(include)

string(REPLACE "-pedantic" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wall" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wextra" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Werror=return-type" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
gaudi_add_library(GenXicc
                  src/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES pythia6forgauss)

