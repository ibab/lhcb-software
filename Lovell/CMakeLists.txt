CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(lovell v1r0
	USE Gaudi v26r0)
