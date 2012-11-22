################################################################################
# Package: MuonTrackAlign
################################################################################
gaudi_subdir(MuonTrackAlign v1r8p1)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/TrackEvent
                         GaudiAlg
                         Muon/MuonDAQ)

find_package(AIDA)

gaudi_add_module(MuonTrackAlign
                 src/*.cpp
                 INCLUDE_DIRS AIDA Muon/MuonDAQ
                 LINK_LIBRARIES MuonDetLib LinkerEvent MCEvent TrackEvent GaudiAlgLib)

