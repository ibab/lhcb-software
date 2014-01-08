################################################################################
# Package: HltRawData
################################################################################
gaudi_subdir(HltRawData v4r0)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/HltEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         GaudiObjDesc
                         Hlt/HltBase
                         Kernel/HltInterfaces
                         Kernel/Relations)

gaudi_add_module(HltRawData
                 src/*.cpp
                 LINK_LIBRARIES DAQEventLib HltEvent TrackEvent GaudiAlgLib GaudiKernel HltBase HltInterfaces RelationsLib)


gaudi_add_test(QMTest QMTEST)
