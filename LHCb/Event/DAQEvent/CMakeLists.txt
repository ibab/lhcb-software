################################################################################
# Package: DAQEvent
################################################################################
gaudi_subdir(DAQEvent v9r20)

gaudi_depends_on_subdirs(GaudiKernel
                         GaudiObjDesc)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(DAQEventLib
                  src/*.cpp
                  PUBLIC_HEADERS Event
                  LINK_LIBRARIES GaudiKernel)

gaudi_add_dictionary(DAQEvent
                     xml/lcgdict/lcg_headers.h
                     xml/lcgdict/lcg_selection.xml
                     LINK_LIBRARIES GaudiKernel DAQEventLib
                     OPTIONS --comments)

