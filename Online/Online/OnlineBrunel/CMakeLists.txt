CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------
gaudi_project(OnlineBrunel  v5r21
              USE OnlineDev v5r21
                  Brunel    v47r8
             DATA AppConfig VERSION v3r*
                  FieldMap VERSION v5r*
                  ParamFiles VERSION v8r*
                  PRConfig VERSION v1r*
                  TCK/HltTCK VERSION v3r*)
