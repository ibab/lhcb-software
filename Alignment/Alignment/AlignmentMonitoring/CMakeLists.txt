################################################################################
# Package: AlignmentMonitoring
################################################################################
gaudi_subdir(AlignmentMonitoring v0r1)

gaudi_depends_on_subdirs(GaudiAlg
					GaudiPython)

find_package(GSL)
find_package(PythonLibs) 
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(ROOT COMPONENTS Core Tree RIO Hist TreePlayer Graf Graf3d Postscript
                             Gpad Matrix MathCore MathMore RooFitCore RooFit
                             RooStats TMVA)

#include_directories(${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
#link_directories(${Boost_LIBRARY_DIR})

#gaudi_install_headers(AlignmentMonitoring)
gaudi_add_library(AlignmentMonitoringLib
                  src/*.cpp
                  PUBLIC_HEADERS AlignmentMonitoring
                  INCLUDE_DIRS Boost GSL ROOT
                  LINK_LIBRARIES Boost GSL ROOT GaudiAlgLib)

#gaudi_add_module(AlignmentMonitoring
#                src/component/*.cpp
#               INCLUDE_DIRS Boost GSL ROOT
#                 LINK_LIBRARIES Boost GSL ROOT GaudiAlgLib AlignmentMonitoringLib)

gaudi_add_dictionary(AlignmentMonitoring
                     dict/AlignmentMonitoringDict.h
                     dict/AlignmentMonitoringDict.xml
                     INCLUDE_DIRS ROOT PythonLibs
                     LINK_LIBRARIES Boost ROOT GSL PythonLibs GaudiPythonLib AlignmentMonitoringLib
                     OPTIONS "-U__MINGW32__")

gaudi_install_python_modules()
