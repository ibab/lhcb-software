################################################################################
# Package: HltSys
################################################################################
gaudi_subdir(HltSys v24r4)

gaudi_depends_on_subdirs(Hlt/Hlt1Lines
                         Hlt/Hlt1Muons
                         Hlt/Hlt2Lines
                         Hlt/Hlt2SharedParticles
                         Hlt/HltBase
                         Hlt/HltBeamGasAlley
                         Hlt/HltCommon
                         Hlt/HltConf
                         Hlt/HltCosmics
                         Hlt/HltDisplVertices
                         Hlt/HltGlobalMonitor
                         Hlt/HltL0Conf
                         Hlt/HltLine
                         Hlt/HltLuminosity
                         Hlt/HltRawData
                         Hlt/HltRecChecker
                         Hlt/HltSettings
                         Hlt/HltTracking
                         Hlt/TCKUtils
                         Phys/LoKiTrigger
                         ## Packages from other projects
                         Event/HltEvent
                         Hlt/HltDAQ
                         Hlt/HltServices
                         Phys/RelatedInfoTools
                         Phys/DaVinciTypes)

gaudi_add_test(QMTest QMTEST)
