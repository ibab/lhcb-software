################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v35r1)

gaudi_depends_on_subdirs(DaVinciTests
                         Phys/DaVinci
			 Phys/KaliCalo)

gaudi_add_test(QMTest QMTEST)
