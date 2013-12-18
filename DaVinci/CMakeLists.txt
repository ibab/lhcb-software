CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(DaVinci v33r9
              USE Analysis v11r4
                  Stripping v7r4
                  Lbcom v14r4
              DATA AppConfig VERSION v3r*
                   FieldMap VERSION v5r*
                   ParamFiles VERSION v8r*
                   TCK/HltTCK)
