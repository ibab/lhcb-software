################################################################################
# Package: PackedEvent
################################################################################
gaudi_subdir(PackedEvent v4r12)

gaudi_depends_on_subdirs(Event/MCEvent
                         Event/PhysEvent
                         Event/RecEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel)

gaudi_add_library(PackedEventLib
                  src/*.cpp
                  PUBLIC_HEADERS Event
                  LINK_LIBRARIES MCEvent PhysEvent RecEvent GaudiAlgLib GaudiKernel LHCbKernel)

gaudi_add_dictionary(PackedEvent
                     dict/PackedEventDict.h
                     dict/PackedEventDict.xml
                     LINK_LIBRARIES MCEvent PhysEvent RecEvent GaudiAlgLib GaudiKernel LHCbKernel PackedEventLib)

