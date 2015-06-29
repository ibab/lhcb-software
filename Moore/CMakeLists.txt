CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(Moore v23r7p2
              USE Hlt v23r7p2
              DATA AppConfig VERSION v3r*
                   FieldMap VERSION v5r*
                   PRConfig
                   ParamFiles VERSION v8r*
                   RawEventFormat
                   TCK/HltTCK VERSION v3r*
                   TCK/L0TCK  VERSION v4r*)
