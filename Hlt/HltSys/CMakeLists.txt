################################################################################
# Package: HltSys
################################################################################
gaudi_subdir(HltSys v23r0)

gaudi_depends_on_subdirs(GaudiConf # temporary
                         Kernel/LHCbAlgs # temporary
                         Hlt/HltDAQ   # temporary
                         DAQ/DAQSys   # temporary
                         DAQ/RawEventCompat   # temporary
                         Event/DAQEvent   # temporary
                         Velo/VeloDAQ   # temporary
                         Det/DDDB   # temporary
                         Kernel/Relations   # temporary
                         Event/EventPacker   # temporary
                         Hlt/Hlt1Lines
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
                         Hlt/HltTisTosTobbing
                         Hlt/HltTracking
                         Hlt/TCKUtils
                         Phys/LoKiTrigger)


gaudi_add_test(QMTest QMTEST)
