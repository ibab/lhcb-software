################################################################################
# Package: HltSys
################################################################################
gaudi_subdir(HltSys v23r7p2)

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
						 Hlt/HltDAQ
						 DAQ/DAQSys
						 Rec/RecAlgs)

gaudi_add_test(QMTest QMTEST)
