################################################################################
# Package: MuonTrackMonitor
################################################################################
gaudi_subdir(MuonTrackMonitor v2r7)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/HltEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         Tf/TrackSys
                         Tr/TrackFitter
                         Tr/TrackTools)

find_package(AIDA)

gaudi_add_module(MuonTrackMonitor
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES MuonDetLib HltEvent LinkerEvent MCEvent RecEvent TrackEvent GaudiAlgLib)

gaudi_install_python_modules()

