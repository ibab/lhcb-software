################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v37r1p2)

gaudi_depends_on_subdirs(DaVinciTests
                         ##############################
                         # Patch just for this version
                         Det/DDDB
                         Phys/TeslaTools
                         ##############################
                         Phys/DaVinci
                         Phys/KaliCalo
                         Phys/Tesla)

gaudi_add_test(QMTest QMTEST)
