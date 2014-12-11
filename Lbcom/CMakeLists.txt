CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(Lbcom v16r2p1
              USE LHCb v38r3p1
              DATA ParamFiles
                   TCK/HltTCK)
