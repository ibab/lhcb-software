CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_subdir(B2DDphi v1r0)

gaudi_add_module(B2DDphi
                src/*.cpp)

gaudi_add_dictionary(B2DDphi
                    dict/B2DDphiDict.h
                    dict/B2DDphiDict.xml
                    OPTIONS "-U__MINGW32__")

gaudi_install_python_modules()
