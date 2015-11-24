CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(Boole v29r12
              USE Lbcom v17r2
              DATA AppConfig VERSION v3r*
                   FieldMap VERSION v5r*
                   ParamFiles VERSION v8r*)
