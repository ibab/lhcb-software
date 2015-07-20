################################################################################
# Package: TrackExtrapolators
################################################################################
gaudi_subdir(TrackExtrapolators v2r38)

gaudi_depends_on_subdirs(Det/DetDesc
                         Event/TrackEvent
                         GaudiAlg
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(GSL)

gaudi_add_module(TrackExtrapolators
                 src/*.cpp
                 INCLUDE_DIRS GSL Tr/TrackInterfaces
                 LINK_LIBRARIES GSL DetDescLib TrackEvent GaudiAlgLib TrackKernel)

