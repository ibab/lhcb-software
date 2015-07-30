CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version and dependencies
gaudi_project(AlignmentOnline v10r5
              USE Online v5r23
                  Alignment v10r5)
