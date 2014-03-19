CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(Boole v27r1
              USE Lbcom v15r1
              DATA AppConfig VERSION v3r*
                   FieldMap VERSION v5r*
                   ParamFiles VERSION v8r*)
