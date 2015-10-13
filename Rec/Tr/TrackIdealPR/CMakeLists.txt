################################################################################
# Package: TrackIdealPR
################################################################################
gaudi_subdir(TrackIdealPR v2r20p1)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Det/VeloDet
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         Kernel/LHCbKernel
                         Kernel/MCInterfaces)

gaudi_add_module(TrackIdealPR
                 src/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces
                 LINK_LIBRARIES OTDetLib STDetLib VeloDetLib LinkerEvent MCEvent RecEvent TrackEvent GaudiAlgLib GaudiKernel LHCbKernel)

