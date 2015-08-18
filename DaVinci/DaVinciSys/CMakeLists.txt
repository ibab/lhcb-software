################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v37r2)

gaudi_depends_on_subdirs(DaVinciTests
                         ##############################
                         # Patch just for this version
                         Det/DDDB
                         Phys/TeslaTools
                         Hlt/HltDAQ
                         Phys/LoKiCore
                         Phys/LoKiAlgo
                         Phys/LoKiPhys
                         Phys/RelatedInfoTools
                         GaudiConf
                         ##############################
                         # Temporarily moved in from Stripping, for S23 release
                         Phys/StrippingCache
                         ##############################
                         Phys/DaVinci
                         Phys/KaliCalo
                         Phys/Tesla)

gaudi_add_test(QMTest QMTEST)
