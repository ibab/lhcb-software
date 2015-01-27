CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------
##find_package(tcmalloc)
##find_package(Boost COMPONENTS system filesystem thread regex)

gaudi_project(OnlineBrunel  v5r15
              USE OnlineDev v5r15
                  Brunel    v47r4
                  DaVinci   v36r4p1
             DATA AppConfig VERSION v3r*
                  FieldMap VERSION v5r*
                  ParamFiles VERSION v8r*
                  PRConfig VERSION v1r*
		  TCK/HltTCK)
