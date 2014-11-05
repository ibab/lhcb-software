################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v36r1)

gaudi_depends_on_subdirs(DaVinciTests
                         Phys/DaVinci
			 Phys/KaliCalo
                         Phys/Tesla
                         Calo/CaloPIDs)

gaudi_add_test(QMTest QMTEST)
