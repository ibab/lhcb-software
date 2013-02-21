################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v33r2)

gaudi_depends_on_subdirs(DaVinciTests
                         Phys/DaVinci
                         Sim/SimComponents)

gaudi_add_test(QMTest QMTEST)
