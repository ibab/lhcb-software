################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v38r1p1)

gaudi_depends_on_subdirs(DaVinciTests
                         ##############################
                         # Patched packages just for this version
                         Hlt/HltDAQ
                         Event/HltEvent
                         Muon/MuonID
                         Tr/TrackExtrapolators
                         ##############################
                         # Temporarily moved in from Stripping, for S23 release
                         Phys/StrippingCache
                         ##############################
                         Phys/DaVinci
                         Phys/KaliCalo
                         Phys/Tesla)

gaudi_add_test(QMTest QMTEST)
