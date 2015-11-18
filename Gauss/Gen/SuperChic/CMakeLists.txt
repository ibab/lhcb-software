################################################################################
# Package: SuperChic
################################################################################
gaudi_subdir(SuperChic v1r0)

gaudi_depends_on_subdirs(Gen/Generators)

string(REPLACE "-pedantic" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wall" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wextra" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Werror=return-type" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
gaudi_add_library(SuperChic
                  src/*.f
                  PUBLIC_HEADERS SuperChic
                  LINK_LIBRARIES GeneratorsLib)

