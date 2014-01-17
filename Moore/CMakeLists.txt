CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(Moore v20r4
              USE Hlt HEAD
              DATA AppConfig VERSION v3r*
                   FieldMap VERSION v5r*
                   PRConfig
                   ParamFiles VERSION v8r*
                   RawEventFormat
                   TCK/HltTCK
                   TCK/L0TCK)
