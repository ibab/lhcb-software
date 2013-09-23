CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

macro(FindG4libs)
    # FIXME: this should be an imported target
    foreach(name ${ARGN})
        #message(STATUS "looking for Geant4 ${name} in ${Geant4_LIBRARY_DIRS}")
        find_library(GEANT4_${name}_LIBRARY
                     G4${name}
                     HINTS ${Geant4_DIR}/lib
                     PATHS ${Geant4_LIBRARY_DIRS})
        #message(STATUS "GEANT4_${name}_LIBRARY -> ${GEANT4_${name}_LIBRARY}")
        list(APPEND GEANT4_LIBS ${GEANT4_${name}_LIBRARY})
    endforeach()
    link_directories(${Geant4_LIBRARY_DIRS})
endmacro()

# Declare project name and version
gaudi_project(Gauss v46r4
              FORTRAN
              USE LHCb HEAD
                  Geant4 HEAD
              DATA AppConfig VERSION v3r*
                   BcVegPyData VERSION v2r*
                   Det/GDMLData VERSION v1r*
                   FieldMap VERSION v5r*
                   Geant4Files VERSION v95r*
                   GenXiccData VERSION v2r*
                   Gen/DecFiles VERSION v27r*
                   LHAPDFSets VERSION v1r0
                   MIBData VERSION v3r*
                   ParamFiles VERSION v8r*)
