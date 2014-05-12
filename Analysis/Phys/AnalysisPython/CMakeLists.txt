################################################################################
# Package: AnalysisPython
################################################################################
gaudi_subdir(AnalysisPython v2r29)

gaudi_depends_on_subdirs(GaudiPython
                         Kernel/LHCbMath)

find_package(ROOT COMPONENTS Core Reflex Tree PyROOT RooFitCore Hist TreePlayer)
find_package(pyanalysis)
find_package(pytools)

gaudi_add_library(AnalysisPythonLib
                  src/*.cpp
                  AnalysisPythonDict_classdef.cpp
                  PUBLIC_HEADERS Analysis
                  INCLUDE_DIRS ROOT
                  LINK_LIBRARIES ROOT GaudiPythonLib LHCbMathLib)

gaudi_add_dictionary(AnalysisPython
                     dict/AnalysisPythonDict.h
                     dict/AnalysisPython.xml
                     INCLUDE_DIRS ROOT
                     LINK_LIBRARIES ROOT GaudiPythonLib LHCbMathLib AnalysisPythonLib
                     SPLIT_CLASSDEF
                     OPTIONS -U__MINGW32__)

gaudi_install_python_modules()
