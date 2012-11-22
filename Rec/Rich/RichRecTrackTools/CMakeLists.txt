################################################################################
# Package: RichRecTrackTools
################################################################################
gaudi_subdir(RichRecTrackTools v1r33)

gaudi_depends_on_subdirs(Det/RichDet
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiKernel
                         Rich/RichRecBase
                         Tr/TrackInterfaces)

find_package(Boost)
find_package(GSL)

gaudi_add_module(RichRecTrackTools
                 src/*.cpp
                 INCLUDE_DIRS Boost GSL Tr/TrackInterfaces
                 LINK_LIBRARIES Boost GSL RichDetLib RecEvent TrackEvent GaudiKernel RichRecBase)

