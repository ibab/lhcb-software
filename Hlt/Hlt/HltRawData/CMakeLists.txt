################################################################################
# Package: HltRawData
################################################################################
gaudi_subdir(HltRawData v5r12)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/HltEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         GaudiObjDesc
                         Hlt/HltBase
                         Hlt/HltDAQ
                         Kernel/HltInterfaces
                         Kernel/Relations)

gaudi_add_module(HltRawData
                 src/*.cpp
                 LINK_LIBRARIES DAQEventLib HltEvent TrackEvent GaudiAlgLib GaudiKernel HltBase HltInterfaces RelationsLib)


gaudi_add_test(QMTest QMTEST)
