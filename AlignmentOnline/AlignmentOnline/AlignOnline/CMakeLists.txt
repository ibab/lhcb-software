CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#---------------------------------------------------------------
# Load macros and functions for Gaudi-based projects
find_package(GaudiProject)
#---------------------------------------------------------------

# Declare project name and version
gaudi_subdir(AlignOnline v1r0)

gaudi_add_module(AlignOnline
                src/*.cpp)
#                INCLUDE_DIRS CLHEP GSL AIDA Alignment/AlignmentInterfaces Tr/TrackInterfaces
#                LINK_LIBRARIES CLHEP GSL AlignEvent AlignKernel MuonDetLib STDetLib LinkerEvent PhysEvent RecEvent TrackEvent GaudiAlgLib GaudiKernel LHCbKernel LHCbMathLib LoKiCoreLib TrackFitEvent TrackKernel)

gaudi_add_dictionary(AlignOnline
                    dict/AlignOnlineDict.h
                    dict/AlignOnlineDict.xml
#INCLUDE_DIRS CLHEP GSL PythonLibs Alignment/AlignmentInterfaces Tr/TrackInterfaces
#LINK_LIBRARIES CLHEP GSL PythonLibs GaudiPythonLib AlignEvent AlignKernel MuonDetLib STDetLib LinkerEvent PhysEvent RecEvent TrackEvent GaudiAlgLib GaudiKernel LHCbKernel LHCbMathLib LoKiCoreLib TrackFitEvent TrackKernel
                    OPTIONS "-U__MINGW32__")

gaudi_install_python_modules()