CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(Alignment v10r1
              USE Online v5r13
                  Phys v19r2p1
              DATA AppConfig
                   FieldMap
                   ParamFiles)
