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
gaudi_project(Gauss v48r2
              FORTRAN
              USE LHCb v38r7
                  Geant4 v96r4p1
              DATA AppConfig VERSION v3r*
                   BcVegPyData VERSION v2r*
                   Det/GDMLData VERSION v1r*
                   FieldMap VERSION v5r*
                   GenXiccData VERSION v3r*
                   Gen/PGunsData VERSION v1r*
                   Gen/DecFiles VERSION v27r*
                   LHAPDFSets VERSION v2r*
                   MIBData VERSION v3r*
                   ParamFiles VERSION v8r*)
