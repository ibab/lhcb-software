################################################################################
# Package: RichRecTrackTools
################################################################################
gaudi_subdir(RichRecTrackTools v1r41)

gaudi_depends_on_subdirs(Det/RichDet
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiKernel
                         Rich/RichRecBase
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(Boost)
find_package(GSL)

gaudi_add_module(RichRecTrackTools
                 src/*.cpp
                 INCLUDE_DIRS Boost GSL Tr/TrackInterfaces Tr/TrackKernel
                 LINK_LIBRARIES Boost GSL RichDetLib RecEvent TrackEvent TrackKernel GaudiKernel RichRecBase)

