################################################################################
# Package: PatVeloTT
################################################################################
gaudi_subdir(PatVeloTT v3r20)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DigiEvent
                         Event/TrackEvent
                         Tf/PatKernel
                         Tf/TfKernel
                         Tr/TrackInterfaces)

gaudi_add_module(PatVeloTT
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Tf/PatKernel Tf/TfKernel Tr/TrackInterfaces
                 LINK_LIBRARIES STDetLib TrackEvent)

gaudi_install_python_modules()

