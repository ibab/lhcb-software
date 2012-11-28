CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(Rec v14r3
              USE LHCb v35r3
              DATA ChargedProtoANNPIDParam
                   TCK/HltTCK)
