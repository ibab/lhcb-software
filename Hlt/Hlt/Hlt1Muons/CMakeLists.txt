################################################################################
# Package: Hlt1Muons
################################################################################
gaudi_subdir(Hlt1Muons v1r14p1)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/TrackEvent
                         GaudiAlg
                         Muon/MuonID
                         Tr/TrackInterfaces)

gaudi_add_module(Hlt1Muons
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES MuonDetLib TrackEvent GaudiAlgLib MuonIDLib)

gaudi_add_dictionary(Hlt1MuonsDict dict/Hlt1MuonsDict.h dict/Hlt1MuonsDict.xml
                     LINK_LIBRARIES MuonDetLib TrackEvent GaudiAlgLib)

gaudi_install_headers(Hlt1Muons)

gaudi_add_test(QMTest QMTEST)
