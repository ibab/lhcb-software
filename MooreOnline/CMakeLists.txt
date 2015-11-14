CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# RootCnv exists in two copies:
#  - in Gaudi as RootCnv
#  - in Online as Online/RootCnv
# We need to ignore one of them, the one in Gaudi
set(override_subdirs RootCnv)

# Declare project name and version
gaudi_project(MooreOnline v24r3
              USE Moore v24r3
                  Online v5r25
              DATA AppConfig VERSION v3r*
                   FieldMap VERSION v5r*
                   PRConfig
                   ParamFiles VERSION v8r*
                   RawEventFormat
                   TCK/HltTCK VERSION v3r*
                   TCK/L0TCK VERSION v4r*)
