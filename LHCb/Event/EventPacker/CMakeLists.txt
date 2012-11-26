################################################################################
# Package: EventPacker
################################################################################
gaudi_subdir(EventPacker v2r20)

gaudi_depends_on_subdirs(DAQ/MDF
                         Event/DAQEvent
                         Event/HltEvent
                         Event/MCEvent
                         Event/MicroDst
                         Event/PackedEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel
                         Kernel/Relations)

find_package(Boost COMPONENTS regex)

gaudi_add_module(EventPacker
                 src/*.cpp
                 INCLUDE_DIRS Boost
                 LINK_LIBRARIES Boost MDFLib DAQEventLib HltEvent MCEvent MicroDstLib PackedEventLib RecEvent TrackEvent GaudiAlgLib GaudiKernel LHCbKernel RelationsLib)

