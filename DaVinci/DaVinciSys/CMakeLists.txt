################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v33r8)

gaudi_depends_on_subdirs(DaVinciTests
                         Phys/DaVinci
                         Det/DDDB)

gaudi_add_test(QMTest QMTEST)
