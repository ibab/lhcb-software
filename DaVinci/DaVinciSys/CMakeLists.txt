################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v36r7p1)

gaudi_depends_on_subdirs(DaVinciTests
########################################################
###  ADDED JUST FOR THIS PATCH RELEASE !!!
                         Hlt/HltDAQ
                         Phys/DaVinciMonitors
                         Phys/TeslaTools
                         Phys/DecayTreeTupleTrigger
########################################################
                         Phys/DaVinci
			 Phys/KaliCalo
                         Phys/Tesla)

gaudi_add_test(QMTest QMTEST)
