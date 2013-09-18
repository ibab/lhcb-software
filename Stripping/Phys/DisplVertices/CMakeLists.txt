################################################################################
# Package: DisplVertices
################################################################################
gaudi_subdir(DisplVertices v7r16)

gaudi_depends_on_subdirs(Det/VeloDet
                         Phys/DaVinciKernel
                         Phys/LoKiPhys
                         Phys/LoKiTracks)

gaudi_add_module(DisplVertices
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES VeloDetLib DaVinciKernelLib LoKiPhysLib LoKiTracksLib)

gaudi_add_dictionary(DisplVertices
                     dict/DisplVerticesDict.h
                     dict/DisplVerticesDict.xml
                     LINK_LIBRARIES VeloDetLib DaVinciKernelLib LoKiPhysLib LoKiTracksLib
                     OPTIONS "-U__MINGW32__")

