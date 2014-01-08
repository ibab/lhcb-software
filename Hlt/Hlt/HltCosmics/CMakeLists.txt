################################################################################
# Package: HltCosmics
################################################################################
gaudi_subdir(HltCosmics v1r1p1)

gaudi_depends_on_subdirs(GaudiKernel
                         Hlt/HltBase
                         OT/OTDAQ)

gaudi_add_module(HltCosmics
                 src/*.cpp
                 INCLUDE_DIRS OT/OTDAQ
                 LINK_LIBRARIES GaudiKernel HltBase)


gaudi_add_test(QMTest QMTEST)
