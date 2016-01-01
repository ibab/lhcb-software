################################################################################
# Package: PatPV
################################################################################
gaudi_subdir(PatPV v3r29)

gaudi_depends_on_subdirs(Det/VeloDet
                         Event/RecEvent
                         Event/TrackEvent
                         Tr/TrackInterfaces)

find_package(Boost)

gaudi_add_module(PatPV
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES VeloDetLib RecEvent TrackEvent)

gaudi_install_python_modules()

