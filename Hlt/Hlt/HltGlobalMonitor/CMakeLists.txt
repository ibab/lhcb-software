################################################################################
# Package: HltGlobalMonitor
################################################################################
gaudi_subdir(HltGlobalMonitor v5r0p1)

gaudi_depends_on_subdirs(Event/HltEvent
                         Event/L0Event
                         Event/TrackEvent
                         GaudiAlg
                         GaudiUtils
                         Hlt/HltBase
                         Tr/TrackInterfaces)

find_package(AIDA)
find_package(Boost)

gaudi_add_module(HltGlobalMonitor
                 src/*.cpp
                 INCLUDE_DIRS AIDA Boost Tr/TrackInterfaces
                 LINK_LIBRARIES Boost HltEvent L0Event TrackEvent GaudiAlgLib GaudiUtilsLib HltBase)


gaudi_add_test(QMTest QMTEST)
