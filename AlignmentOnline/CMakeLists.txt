CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version and dependencies (LHCB v36r0 given as example)
gaudi_project(AlignmentOnline v10r1
              USE Alignment v10r1
              USE Online v5r13)
