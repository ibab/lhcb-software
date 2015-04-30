CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_project(DaVinci v36r6
              USE Analysis v13r5
                  Stripping v9r6
                  Lbcom v16r5
              DATA AppConfig VERSION v3r*
                   FieldMap VERSION v5r*
                   ParamFiles VERSION v8r*
                   QMTestFiles VERSION v1r*
                   PRConfig VERSION v1r*
                   TurboStreamProd VERSION v*
                   TCK/HltTCK)
