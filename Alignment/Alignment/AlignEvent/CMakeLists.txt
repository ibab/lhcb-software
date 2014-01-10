################################################################################
# Package: AlignEvent
################################################################################
gaudi_subdir(AlignEvent v1r1)

gaudi_depends_on_subdirs(Alignment/AlignKernel
                         GaudiObjDesc
                         Kernel/LHCbKernel)

find_package(GSL)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(AlignEvent
                  src/*.cpp
                  NO_PUBLIC_HEADERS
                  INCLUDE_DIRS GSL
                  LINK_LIBRARIES GSL AlignKernel LHCbKernel)

god_build_dictionary(xml/*.xml
                     INCLUDE_DIRS GSL
                     LINK_LIBRARIES GSL AlignKernel LHCbKernel AlignEvent)

