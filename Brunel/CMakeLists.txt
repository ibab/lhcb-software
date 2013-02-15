CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(Brunel v44r2
              USE Lbcom v13r4
                  Rec v14r4
              DATA AppConfig VERSION v3r*
                   FieldMap VERSION v5r*
                   ParamFiles VERSION v8r*)
