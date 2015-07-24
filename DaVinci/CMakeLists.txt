CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(DaVinci v37r0
              USE Analysis v14r0
                  Stripping v10r0
              DATA AppConfig VERSION v3r*
                   FieldMap VERSION v5r*
                   ParamFiles VERSION v8r*
                   QMTestFiles VERSION v1r*
                   PRConfig VERSION v1r*
                   TurboStreamProd VERSION v*
                   TCK/HltTCK)
