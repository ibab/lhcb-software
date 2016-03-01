CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version and dependencies
gaudi_project(AlignmentOnline v10r6
              USE Online v5r29
                  Alignment v10r6)
