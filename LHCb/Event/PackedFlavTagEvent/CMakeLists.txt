################################################################################
# Package: PackedFlavTagEvent
################################################################################
gaudi_subdir(PackedFlavTagEvent v1r0)

gaudi_depends_on_subdirs(DAQ/MDF
                         Event/MicroDst
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel
                         Kernel/Relations)

find_package(Boost COMPONENTS regex)

include_directories(src)

gaudi_add_library(PackedFlavTagEventLib
                  src/lib/*.cpp
                  NO_PUBLIC_HEADERS
                  LINK_LIBRARIES GaudiAlgLib GaudiKernel LHCbKernel)

gaudi_add_dictionary(PackedFlavTagEvent
                     dict/PackedFlavTagEventDict.h
                     dict/PackedFlavTagEventDict.xml
                     LINK_LIBRARIES GaudiAlgLib GaudiKernel LHCbKernel)
