################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v36r5)

gaudi_depends_on_subdirs(DaVinciTests
                         Phys/DaVinci
			 Phys/KaliCalo
                         Phys/Tesla)

gaudi_add_test(QMTest QMTEST)
