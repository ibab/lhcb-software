################################################################################
# Package: PatVeloTT
################################################################################
gaudi_subdir(PatVeloTT v4r8)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DigiEvent
                         Event/TrackEvent
                         Tf/PatKernel
                         Tf/TfKernel
                         Tr/TrackInterfaces
                         Event/LinkerEvent
                         Event/MCEvent)

find_package(VDT)
find_package(Boost COMPONENTS system)

gaudi_add_module(PatVeloTT
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Tf/PatKernel Tf/TfKernel Tr/TrackInterfaces VDT Boost
                 LINK_LIBRARIES STDetLib TrackEvent VDT LinkerEvent MCEvent)

gaudi_install_python_modules()

