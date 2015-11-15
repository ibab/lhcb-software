################################################################################# Package: LPair
################################################################################
gaudi_subdir(LPair v1r0)

gaudi_depends_on_subdirs(Gen/Generators)

string(REPLACE "-Werror=return-type" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")

gaudi_add_library(LPair
                  src/*.[fF]
                  PUBLIC_HEADERS LPair
                  LINK_LIBRARIES GeneratorsLib)

