################################################################################
# Package: HltEvent
################################################################################
gaudi_subdir(HltEvent v9r6)

gaudi_depends_on_subdirs(Event/PhysEvent
                         GaudiKernel
                         GaudiObjDesc)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(HltEvent
                  src/*.cpp
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES PhysEvent GaudiKernel)

god_build_dictionary(xml/*.xml
                     EXTEND dict/lcgDict.h
                     LINK_LIBRARIES PhysEvent GaudiKernel HltEvent)

