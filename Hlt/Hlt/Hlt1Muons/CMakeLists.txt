################################################################################
# Package: Hlt1Muons
################################################################################
gaudi_subdir(Hlt1Muons v1r6)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/TrackEvent
                         GaudiAlg
                         Tr/TrackInterfaces)

gaudi_add_module(Hlt1Muons
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES MuonDetLib TrackEvent GaudiAlgLib)


gaudi_add_test(QMTest QMTEST)
