################################################################################
# Package: PhysEvent
################################################################################
gaudi_subdir(PhysEvent v11r24)

gaudi_depends_on_subdirs(Event/RecEvent
                         GaudiKernel
                         GaudiObjDesc
                         Kernel/LHCbKernel
                         Kernel/LHCbMath)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(PhysEvent
                  src/*.cpp
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES RecEvent GaudiKernel LHCbKernel LHCbMathLib)

god_build_dictionary(xml/*.xml
                     EXTEND dict/lcgDict.h
                     LINK_LIBRARIES RecEvent GaudiKernel LHCbKernel LHCbMathLib PhysEvent)

