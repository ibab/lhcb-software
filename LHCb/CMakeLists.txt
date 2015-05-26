CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

if(EXISTS ${CMAKE_SOURCE_DIR}/Online/RootCnv)
  set(override_subdirs RootCnv)
endif()

# Declare project name and version
gaudi_project(LHCb v38r7
              USE Gaudi v26r2
              DATA Gen/DecFiles
                   Det/SQLDDDB VERSION v7r*
                   FieldMap
                   ParamFiles
                   PRConfig
                   RawEventFormat
                   TCK/HltTCK
		   TCK/L0TCK VERSION v4r*)
