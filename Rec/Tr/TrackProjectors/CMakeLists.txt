################################################################################
# Package: TrackProjectors
################################################################################
gaudi_subdir(TrackProjectors v3r4)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Det/VPDet
                         Det/VeloDet
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(GSL)

gaudi_add_module(TrackProjectors
                 src/*.cpp
                 INCLUDE_DIRS GSL Tr/TrackInterfaces
                 LINK_LIBRARIES GSL OTDetLib STDetLib VPDetLib VeloDetLib RecEvent TrackEvent GaudiAlgLib TrackFitEvent TrackKernel)

