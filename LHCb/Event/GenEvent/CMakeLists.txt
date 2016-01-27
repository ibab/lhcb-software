################################################################################
# Package: GenEvent
################################################################################
gaudi_subdir(GenEvent v7r0)

gaudi_depends_on_subdirs(Event/EventBase
                         GaudiObjDesc)

find_package(HepMC)
find_package(ROOT COMPONENTS GenVector)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(GenEvent
                  src/*.cpp
                  PUBLIC_HEADERS GenEvent
                  INCLUDE_DIRS HepMC ROOT Event/EventBase
                  LINK_LIBRARIES HepMC ROOT GaudiObjDescLib)

god_build_dictionary(xml/*.xml
                     EXTEND xml/lcgdict/lcg_selection.h
                     INCLUDE_DIRS HepMC ROOT Event/EventBase
                     LINK_LIBRARIES HepMC ROOT GaudiObjDescLib GenEvent)

