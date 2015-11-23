################################################################################
# Package: FTEvent
################################################################################
gaudi_subdir(FTEvent v1r8)

gaudi_depends_on_subdirs(Event/EventBase
                         Event/MCEvent
                         GaudiObjDesc)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

god_build_dictionary(xml/*.xml
                     EXTEND dict/lcgDict.h xml/lcgdict/lcg_selection.xml
                     INCLUDE_DIRS Event/EventBase
                     LINK_LIBRARIES MCEvent GaudiObjDescLib)

