################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v38r0)

gaudi_depends_on_subdirs(DaVinciTests
                         ##############################
                         # Temporarily moved in from Stripping, for S23 release
                         Phys/StrippingCache
                         ##############################
                         Phys/DaVinci
                         Phys/KaliCalo
                         Phys/Tesla)

gaudi_add_test(QMTest QMTEST)
