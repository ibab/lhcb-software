################################################################################
# Package: TrackMCTools
################################################################################
gaudi_subdir(TrackMCTools v3r12p1)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Det/VeloDet
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         Kernel/MCInterfaces
                         Tf/PatKernel
                         Tr/TrackInterfaces)

gaudi_add_module(TrackMCTools
                 src/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces Tf/PatKernel Tr/TrackInterfaces
                 LINK_LIBRARIES OTDetLib STDetLib VeloDetLib LinkerEvent MCEvent RecEvent TrackEvent GaudiAlgLib)

