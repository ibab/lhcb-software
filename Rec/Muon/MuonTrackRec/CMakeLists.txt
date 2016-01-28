################################################################################
# Package: MuonTrackRec
################################################################################
gaudi_subdir(MuonTrackRec v4r0)

gaudi_depends_on_subdirs(Det/MuonDet
                         GaudiAlg
                         Muon/MuonDAQ
                         Muon/MuonInterfaces)

gaudi_add_module(MuonTrackRec
                 src/*
                 src/component/*.cpp
                 INCLUDE_DIRS Muon/MuonDAQ
                 LINK_LIBRARIES MuonDetLib GaudiAlgLib MuonInterfacesLib)

