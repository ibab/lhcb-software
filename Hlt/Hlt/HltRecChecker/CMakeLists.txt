################################################################################
# Package: HltRecChecker
################################################################################
gaudi_subdir(HltRecChecker v2r0p1)

gaudi_depends_on_subdirs(Event/LinkerEvent
                         Event/MCEvent
                         GaudiKernel
                         Hlt/HltBase)

gaudi_add_module(HltRecChecker
                 src/*.cpp
                 LINK_LIBRARIES LinkerEvent MCEvent GaudiKernel HltBase)


gaudi_add_test(QMTest QMTEST)
