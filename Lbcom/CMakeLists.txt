CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(Lbcom v13r2
              USE LHCb v35r2
              DATA ParamFiles
                   TCK/HltTCK
                   TCK/L0TCK VERSION v4r*)
