################################################################################
# Package: MuonID
################################################################################
gaudi_subdir(MuonID v9r18)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/RecEvent
                         Event/TrackEvent
                         Muon/MuonInterfaces
                         Muon/MuonTrackRec
                         Tr/TrackInterfaces
                         Tr/TrackFitEvent
                         GaudiAlg)

find_package(ROOT COMPONENTS Hist Gpad Graf Matrix)
find_package(VDT)

gaudi_add_library(MuonIDLib
                 src/lib/*.cpp
                 PUBLIC_HEADERS MuonID
                 INCLUDE_DIRS ROOT Tr/TrackInterfaces VDT
                 LINK_LIBRARIES ROOT MuonDetLib RecEvent TrackEvent)

gaudi_add_module(MuonID
                 src/component/*.cpp
                 INCLUDE_DIRS ROOT Tr/TrackInterfaces VDT
                 LINK_LIBRARIES ROOT MuonDetLib RecEvent TrackEvent MuonInterfacesLib MuonTrackRec TrackFitEvent GaudiAlgLib MuonIDLib)

gaudi_install_python_modules()
