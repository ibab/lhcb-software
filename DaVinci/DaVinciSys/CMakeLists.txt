################################################################################
# Package: DaVinciSys
################################################################################
gaudi_subdir(DaVinciSys v39r1)

gaudi_depends_on_subdirs(DaVinciTests
                         ##############################
                         # Patch just for this release!
                         Event/EventPacker
                         Phys/LoKiCore
                         ##############################
                         # Temporarily moved in from Stripping, for S23 release
                         Phys/StrippingCache
                         ##############################
                         Phys/DaVinci
                         Phys/KaliCalo
                         Phys/Tesla)

gaudi_add_test(QMTest QMTEST)
