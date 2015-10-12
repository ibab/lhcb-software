################################################################################
# Package: EventPacker
################################################################################
gaudi_subdir(EventPacker v5r7)

gaudi_depends_on_subdirs(DAQ/MDF
                         Event/DAQEvent
                         Event/HltEvent
                         Event/MCEvent
                         Event/MicroDst
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel
                         Kernel/Relations)

find_package(Boost COMPONENTS regex)

gaudi_add_library(EventPackerLib
                  src/lib/*.cpp
                  PUBLIC_HEADERS Event
                  LINK_LIBRARIES MCEvent PhysEvent RecEvent GaudiAlgLib GaudiKernel LHCbKernel)

gaudi_add_module(EventPacker
                 src/component/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost MDFLib DAQEventLib HltEvent MCEvent MicroDstLib RecEvent TrackEvent GaudiAlgLib GaudiKernel LHCbKernel RelationsLib EventPackerLib)

gaudi_add_dictionary(EventPacker
                     dict/PackedEventDict.h
                     dict/PackedEventDict.xml
                     LINK_LIBRARIES MCEvent PhysEvent RecEvent GaudiAlgLib GaudiKernel LHCbKernel EventPackerLib)
