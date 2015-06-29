################################################################################
# Package: HltInterfaces
################################################################################
gaudi_subdir(HltInterfaces v6r1)

gaudi_depends_on_subdirs(Event/DAQEvent
                         GaudiKernel
                         Kernel/LHCbMath)

find_package(Boost COMPONENTS regex)

gaudi_add_library(HltInterfaces
                  src/*.cpp
                  PUBLIC_HEADERS Kernel
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost DAQEventLib GaudiKernel LHCbMathLib)

gaudi_add_dictionary(HltInterfaces
                     dict/HltInterfacesDict.h
                     dict/HltInterfacesDict.xml
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost DAQEventLib GaudiKernel LHCbMathLib HltInterfaces
                     OPTIONS "-U__MINGW32__")

