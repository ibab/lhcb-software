################################################################################
# Package: LinkerEvent
################################################################################
gaudi_subdir(LinkerEvent v3r11)

gaudi_depends_on_subdirs(GaudiObjDesc
                         Kernel/LHCbKernel)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(LinkerEvent
                  src/*.cpp
                  PUBLIC_HEADERS Linker
                  LINK_LIBRARIES LHCbKernel)

god_build_dictionary(xml/*.xml
                     LINK_LIBRARIES LHCbKernel LinkerEvent)

