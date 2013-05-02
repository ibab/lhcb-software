################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v33r3p3)

gaudi_depends_on_subdirs(DaVinciTests
                         Phys/DaVinci
                         Sim/SimComponents)

gaudi_add_test(QMTest QMTEST)
