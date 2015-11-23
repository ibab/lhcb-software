################################################################################
# Package: RecEvent
################################################################################
gaudi_subdir(RecEvent v2r55)

gaudi_depends_on_subdirs(Event/DigiEvent
                         Event/EventBase
                         Event/TrackEvent
                         GaudiObjDesc
                         Kernel/LHCbMath
                         Kernel/PartProp)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(RecEvent
                  src/*.cpp
                  PUBLIC_HEADERS Event
                  INCLUDE_DIRS Event/DigiEvent Event/EventBase
                  LINK_LIBRARIES TrackEvent LHCbMathLib PartPropLib)

god_build_dictionary(xml/*.xml
                     EXTEND dict/lcgDict.h
                     INCLUDE_DIRS Event/DigiEvent Event/EventBase
                     LINK_LIBRARIES TrackEvent LHCbMathLib PartPropLib RecEvent)

