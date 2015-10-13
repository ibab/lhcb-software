################################################################################
# Package: MuonTrackAlign
################################################################################
gaudi_subdir(MuonTrackAlign v1r10p1)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/TrackEvent
                         Tr/TrackInterfaces
                         GaudiAlg
                         Muon/MuonDAQ)

find_package(AIDA)

gaudi_add_module(MuonTrackAlign
                 src/*.cpp
                 INCLUDE_DIRS AIDA Muon/MuonDAQ Tr/TrackInterfaces
                 LINK_LIBRARIES MuonDetLib LinkerEvent MCEvent TrackEvent GaudiAlgLib)

