################################################################################
# Package: HltLuminosity
################################################################################
gaudi_subdir(HltLuminosity v4r4)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Event/DAQEvent
                         Event/HltEvent
                         Event/L0Event
                         Event/LumiEvent
                         Event/TrackEvent
                         GaudiAlg
                         Hlt/HltBase
                         Tr/TrackInterfaces)

find_package(AIDA)
find_package(Boost)

gaudi_add_module(HltLuminosity
                 src/*.cpp
                 INCLUDE_DIRS AIDA Boost Event/LumiEvent Tr/TrackInterfaces
                 LINK_LIBRARIES Boost CaloUtils DAQEventLib HltEvent L0Event TrackEvent GaudiAlgLib HltBase)


gaudi_add_test(QMTest QMTEST)
