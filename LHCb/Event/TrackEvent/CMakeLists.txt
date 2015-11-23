################################################################################
# Package: TrackEvent
################################################################################
gaudi_subdir(TrackEvent v6r4)

gaudi_depends_on_subdirs(GaudiObjDesc
                         Kernel/LHCbKernel)

find_package(Boost)
find_package(GSL)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(TrackEvent
                  src/*.cpp
                  PUBLIC_HEADERS Event
                  INCLUDE_DIRS GSL Boost
                  LINK_LIBRARIES GSL Boost LHCbKernel)

god_build_dictionary(xml/*.xml
                     EXTEND dict/lcgDict.h
                     INCLUDE_DIRS GSL Boost
                     LINK_LIBRARIES GSL Boost LHCbKernel TrackEvent)

