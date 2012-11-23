################################################################################
# Package: TrackSeeding
################################################################################
gaudi_subdir(TrackSeeding v1r6)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Event/DigiEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces)

gaudi_add_module(TrackSeeding
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Tr/TrackInterfaces
                 LINK_LIBRARIES OTDetLib STDetLib TrackEvent GaudiAlgLib GaudiKernel LHCbKernel TrackFitEvent)

