################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v33r4)

gaudi_depends_on_subdirs(DaVinciTests
                         Phys/DaVinci)

gaudi_add_test(QMTest QMTEST)
