################################################################################
# Package: PatVeloTT
################################################################################
gaudi_subdir(PatVeloTT v4r7p1)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DigiEvent
                         Event/TrackEvent
                         Tf/PatKernel
                         Tf/TfKernel
                         Tr/TrackInterfaces)

find_package(VDT)

gaudi_add_module(PatVeloTT
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Tf/PatKernel Tf/TfKernel Tr/TrackInterfaces VDT
                 LINK_LIBRARIES STDetLib TrackEvent VDT)

gaudi_install_python_modules()

