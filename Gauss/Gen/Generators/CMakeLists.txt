################################################################################
# Package: Generators
################################################################################
gaudi_subdir(Generators v15r0)

gaudi_depends_on_subdirs(Event/GenEvent
                         GaudiAlg
                         Gen/EvtGen
                         Gen/EvtGenExtras
                         Kernel/MCInterfaces
                         Kernel/PartProp)

find_package(Boost COMPONENTS filesystem system)

string(REPLACE "-pedantic" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wall" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Wextra" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
string(REPLACE "-Werror=return-type" "" CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS}")
gaudi_add_library(GeneratorsLib
                  src/Lib/*.cpp src/Lib/*.F
                  PUBLIC_HEADERS Generators
                  INCLUDE_DIRS Kernel/MCInterfaces
                  LINK_LIBRARIES GenEvent GaudiAlgLib EvtGen EvtGenExtras PartPropLib)

gaudi_add_module(Generators
                 src/component/*.cpp
                 INCLUDE_DIRS Boost Kernel/MCInterfaces
                 LINK_LIBRARIES Boost GenEvent GaudiAlgLib EvtGen EvtGenExtras GeneratorsLib)

