################################################################################
# Package: LumiEvent
################################################################################
gaudi_subdir(LumiEvent v3r5)

gaudi_depends_on_subdirs(Event/DAQEvent
                         Event/EventBase
                         GaudiAlg
                         GaudiKernel
                         GaudiObjDesc
                         Kernel/LHCbKernel)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_module(LumiEvent
                 src/*.cpp
                 INCLUDE_DIRS Event/EventBase
                 LINK_LIBRARIES DAQEventLib GaudiAlgLib GaudiKernel LHCbKernel)

god_build_dictionary(xml/*.xml
                     EXTEND dict/lcgDict.h
                     INCLUDE_DIRS Event/EventBase
                     LINK_LIBRARIES DAQEventLib GaudiAlgLib GaudiKernel LHCbKernel)

