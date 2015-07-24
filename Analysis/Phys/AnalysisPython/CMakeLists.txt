################################################################################
# Package: AnalysisPython
################################################################################
gaudi_subdir(AnalysisPython v2r37)

gaudi_depends_on_subdirs(GaudiPython
                         Kernel/LHCbMath)

find_package(ROOT)
if(ROOT_VERSION_MAJOR EQUAL 5)
  find_package(ROOT COMPONENTS Core Reflex Tree PyROOT RooFitCore Hist TreePlayer)
else()
  find_package(ROOT COMPONENTS Core Tree PyROOT RooFitCore Hist TreePlayer)
endif()
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

# Ensure that the library and the dictionary are built in the right order and
# not multiple times.
add_dependencies(AnalysisPythonLib AnalysisPythonGen)

# Required to work with ROOT 6
# (see https://sft.its.cern.ch/jira/browse/ROOT-6233)
target_link_libraries(AnalysisPythonDict -Wl,--no-as-needed AnalysisPythonLib -Wl,--as-needed)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
