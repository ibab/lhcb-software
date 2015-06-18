################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v36r7p4)

gaudi_depends_on_subdirs(DaVinciTests
########################################################
###  ADDED JUST FOR THIS PATCH RELEASE !!!
                         Event/EventPacker
                         Hlt/HltDAQ
                         Phys/DaVinciMonitors
                         Phys/TeslaTools
                         Phys/DecayTreeTupleTrigger
                         Det/DetCond
                         Det/DDDB
########################################################
                         Phys/DaVinci
			 Phys/KaliCalo
                         Phys/Tesla)

gaudi_add_test(QMTest QMTEST)
