################################################################################
# Package: TrackMCTools
################################################################################
gaudi_subdir(TrackMCTools v3r11)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Det/VeloDet
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         Event/TrackEvent
			 Event/PhysEvent
                         GaudiAlg
                         Kernel/MCInterfaces
                         Tf/PatKernel
                         Tr/TrackInterfaces)

gaudi_add_module(TrackMCTools
                 src/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces Tf/PatKernel Tr/TrackInterfaces
                 LINK_LIBRARIES OTDetLib STDetLib VeloDetLib LinkerEvent MCEvent RecEvent TrackEvent PhysEvent GaudiAlgLib)

