################################################################################
# Package: GiGa
################################################################################
gaudi_subdir(GiGa v20r2p1)

gaudi_depends_on_subdirs(GaudiAlg)

find_package(CLHEP COMPONENTS Random Vector)

FindG4libs(digits_hits event geometry global graphics_reps materials
           particles processes run tracking track)

message(WARNING "option -pedantic disabled because it causes compile time errors")
string(REPLACE "-pedantic" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

gaudi_add_library(GiGaLib
                  src/Lib/*.cpp
                  PUBLIC_HEADERS GiGa
                  INCLUDE_DIRS CLHEP Geant4
                  LINK_LIBRARIES GaudiAlgLib CLHEP
                                 ${GEANT4_LIBS})

gaudi_add_module(GiGa
                 src/component/*.cpp
                 LINK_LIBRARIES GaudiAlgLib GiGaLib)

gaudi_env(SET GIGAOPTS \${GIGAROOT}/options)
