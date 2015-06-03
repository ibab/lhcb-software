################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v36r7)

gaudi_depends_on_subdirs(DaVinciTests
####################################################
###  ADDED JUST FOR THIS RELEASE !!!
                         Event/EventPacker
####################################################
                         Phys/DaVinci
			 Phys/KaliCalo
                         Phys/Tesla)

gaudi_add_test(QMTest QMTEST)
