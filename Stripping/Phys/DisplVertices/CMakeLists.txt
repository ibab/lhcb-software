################################################################################
# Package: DisplVertices
################################################################################
gaudi_subdir(DisplVertices v7r16)

gaudi_depends_on_subdirs(Det/VeloDet
                         Phys/DaVinciKernel
                         Phys/LoKiPhys
                         Phys/LoKiTracks
                         Tr/TrackKernel)

gaudi_add_module(DisplVertices
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES VeloDetLib DaVinciKernelLib LoKiPhysLib
                                LoKiTracksLib TrackKernel)

