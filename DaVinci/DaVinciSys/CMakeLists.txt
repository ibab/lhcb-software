################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v37r0)

gaudi_depends_on_subdirs(DaVinciTests
                         Phys/DaVinci
			 Phys/KaliCalo
                         Phys/Tesla)

gaudi_add_test(QMTest QMTEST)
