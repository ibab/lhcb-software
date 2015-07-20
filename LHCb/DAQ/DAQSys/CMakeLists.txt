################################################################################
# Package: DAQSys
################################################################################
gaudi_subdir(DAQSys v8r9)

gaudi_depends_on_subdirs(Event/LumiEvent
			 Calo/CaloDAQ
                         L0/L0Calo
                         L0/L0DU
                         L0/L0Muon
                         Muon/MuonDAQ
                         OT/OTDAQ
                         Rich/RichDAQ
                         Si/SiDAQ
                         ST/STDAQ
                         FT/FTDAQ
                         Velo/VeloDAQ
                         VP/VPDAQ
                         Kernel/LHCbAlgs
                         GaudiConf
                         DAQ/RawEventCompat)

gaudi_install_python_modules()

gaudi_add_test(QMTest QMTEST)
