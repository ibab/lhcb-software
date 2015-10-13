################################################################################
# Package: TrackAssociators
################################################################################
gaudi_subdir(TrackAssociators v2r12p1)

gaudi_depends_on_subdirs(Event/DigiEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg
                         Kernel/LHCbKernel
                         Tr/TrackFitEvent)

gaudi_add_module(TrackAssociators
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent
                 LINK_LIBRARIES LinkerEvent MCEvent GaudiAlgLib LHCbKernel TrackFitEvent)

