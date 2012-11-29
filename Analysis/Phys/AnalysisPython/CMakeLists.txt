################################################################################
# Package: AnalysisPython
################################################################################
gaudi_subdir(AnalysisPython v2r16)

gaudi_depends_on_subdirs(GaudiPython
                         Kernel/LHCbMath)

find_package(ROOT COMPONENTS Core Reflex Tree PyROOT RooFitCore Hist)
find_package(pyanalysis)
find_package(pytools)

# Fake build command to tell CMake that AnalysisPython_dict_classdef.cpp
# id a byproduct of AnalysisPythonGen (not declared in FindROOT.cmake)
add_custom_command(OUTPUT AnalysisPython_dict_classdef.cpp
                   COMMAND true
                   DEPENDS AnalysisPythonGen)

gaudi_add_library(AnalysisPythonLib
                  src/*.cpp
                  ${CMAKE_CURRENT_BINARY_DIR}/AnalysisPython_dict_classdef.cpp
                  PUBLIC_HEADERS Analysis
                  INCLUDE_DIRS ROOT
                  LINK_LIBRARIES ROOT GaudiPythonLib LHCbMathLib)

gaudi_add_dictionary(AnalysisPython
                     dict/AnalysisPythonDict.h
                     dict/AnalysisPython.xml
                     INCLUDE_DIRS ROOT
                     LINK_LIBRARIES ROOT GaudiPythonLib LHCbMathLib AnalysisPythonLib
                     OPTIONS -U__MINGW32__ --split=classdef)

gaudi_install_python_modules()
gaudi_install_scripts()

