################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v36r0)

gaudi_depends_on_subdirs(DaVinciTests
                         Phys/DaVinci
			 Phys/KaliCalo
                         DAQ/RawEventCompat
                         DAQ/DAQSys
                         Velo/VeloDAQ)

gaudi_add_test(QMTest QMTEST)
