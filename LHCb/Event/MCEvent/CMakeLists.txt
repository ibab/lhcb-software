################################################################################
# Package: MCEvent
################################################################################
gaudi_subdir(MCEvent v3r4)

gaudi_depends_on_subdirs(Event/EventBase
                         GaudiObjDesc
                         Kernel/LHCbKernel
                         Kernel/PartProp)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(MCEvent
                  src/*.cpp
                  PUBLIC_HEADERS Event
                  INCLUDE_DIRS Event/EventBase
                  LINK_LIBRARIES LHCbKernel PartPropLib)

god_build_dictionary(xml/*.xml
                     EXTEND dict/lcgDict.h
                     INCLUDE_DIRS Event/EventBase
                     LINK_LIBRARIES LHCbKernel PartPropLib MCEvent)

