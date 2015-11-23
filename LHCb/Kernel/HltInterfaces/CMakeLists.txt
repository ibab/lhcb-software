################################################################################
# Package: HltInterfaces
################################################################################
gaudi_subdir(HltInterfaces v6r3)

gaudi_depends_on_subdirs(Event/DAQEvent
                         GaudiKernel
                         Kernel/LHCbMath)

find_package(Boost COMPONENTS regex)

if(Boost_VERSION STREQUAL "105900")
  # See https://svn.boost.org/trac/boost/ticket/11743
  add_definitions(-UNDEBUG)
endif()

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

