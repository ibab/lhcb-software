################################################################################
# Package: L0Event
################################################################################
gaudi_subdir(L0Event v19r8)

gaudi_depends_on_subdirs(GaudiObjDesc
                         Kernel/LHCbKernel)

find_package(Boost)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(L0Event
                  src/*.cpp
                  PUBLIC_HEADERS Event
                  INCLUDE_DIRS Boost
                  LINK_LIBRARIES Boost LHCbKernel)

god_build_dictionary(xml/*.xml
                     EXTEND xml/lcgdict/lcg_selection.h
                     INCLUDE_DIRS Boost
                     LINK_LIBRARIES Boost LHCbKernel L0Event)

