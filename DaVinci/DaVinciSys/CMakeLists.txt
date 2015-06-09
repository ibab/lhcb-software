################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v36r7p1)

gaudi_depends_on_subdirs(DaVinciTests
########################################################
###  ADDED JUST FOR THIS PATCH RELEASE NEEDED FOR DQ !!!
                         Phys/DaVinciMonitors
########################################################
                         Phys/DaVinci
			 Phys/KaliCalo
                         Phys/Tesla)

gaudi_add_test(QMTest QMTEST)
