CMAKE_MINIMUM_REQUIRED(VERSION 2.8.9)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(Lovell v2r0
              USE Gaudi v26r4)
