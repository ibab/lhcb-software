################################################################################
# Package: STTELL1Event
################################################################################
gaudi_subdir(STTELL1Event v1r10)

gaudi_depends_on_subdirs(GaudiObjDesc
                         Kernel/LHCbKernel
                         ST/STKernel)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(STTELL1Event
                  src/*.cpp
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES LHCbKernel STKernelLib)

god_build_dictionary(xml/*.xml
                     LINK_LIBRARIES LHCbKernel STKernelLib STTELL1Event)

