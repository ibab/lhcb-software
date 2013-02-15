################################################################################
# Package: FstTools
################################################################################
gaudi_subdir(FstTools v1r0)

gaudi_depends_on_subdirs(Event/RecEvent
                         GaudiAlg
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces)

gaudi_add_module(FstTools
                 src/*.cpp
                 LINK_LIBRARIES TrackFitEvent GaudiAlgLib RecEvent)

gaudi_install_python_modules()
