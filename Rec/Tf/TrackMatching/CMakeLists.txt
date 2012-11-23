################################################################################
# Package: TrackMatching
################################################################################
gaudi_subdir(TrackMatching v2r33p1)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DigiEvent
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiKernel
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(Boost)
find_package(GSL)

gaudi_add_module(TrackMatching
                 src/*.cpp
                 INCLUDE_DIRS Boost GSL Event/DigiEvent Tr/TrackInterfaces
                 LINK_LIBRARIES Boost GSL STDetLib RecEvent TrackEvent GaudiAlgLib GaudiKernel TrackFitEvent TrackKernel)

gaudi_install_python_modules()

