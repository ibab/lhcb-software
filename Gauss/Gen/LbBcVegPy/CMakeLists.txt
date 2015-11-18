################################################################################
# Package: LbBcVegPy
################################################################################
gaudi_subdir(LbBcVegPy v6r0)

gaudi_depends_on_subdirs(Gen/BcVegPy
                         Gen/LbHard)

string(REPLACE "-pedantic" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wall" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wextra" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Werror=return-type" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
gaudi_add_library(LbBcVegPyLib
                  src/Lib/*.cpp
                  src/Lib/*.F
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES BcVegPy LbPythiaLib)

gaudi_add_module(LbBcVegPy
                 src/component/*.cpp
                 LINK_LIBRARIES BcVegPy LbHardLib LbPythiaLib LbBcVegPyLib)

