################################################################################
# Package: PhysConf
################################################################################
gaudi_subdir(PhysConf v1r28)

gaudi_depends_on_subdirs(Calo/CaloPIDs
                         Calo/CaloReco
                         GaudiConf
                         Kernel/LHCbAlgs
                         Muon/MuonDAQ
                         Muon/MuonID
                         Phys/LoKiCore
                         Rec/GlobalReco)

gaudi_install_python_modules()

