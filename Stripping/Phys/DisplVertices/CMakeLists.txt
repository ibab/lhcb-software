################################################################################
# Package: DisplVertices
################################################################################
gaudi_subdir(DisplVertices v7r22)

gaudi_depends_on_subdirs(Det/VeloDet
                         Event/HltEvent
                         Phys/DaVinciKernel
                         Phys/LoKiPhys
                         Phys/LoKiTracks
                         Phys/LoKiArrayFunctors
                         Tr/TrackKernel
                         Phys/JetAccessories)

gaudi_add_module(DisplVertices
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES VeloDetLib DaVinciKernelLib LoKiPhysLib
                                LoKiTracksLib LoKiArrayFunctorsLib TrackKernel
                                HltEvent)

gaudi_install_headers(Kernel)
gaudi_install_python_modules()

