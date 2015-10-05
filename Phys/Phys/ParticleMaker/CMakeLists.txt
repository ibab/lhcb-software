################################################################################
# Package: ParticleMaker
################################################################################
gaudi_subdir(ParticleMaker v6r16)

gaudi_depends_on_subdirs(Calo/CaloInterfaces
                         Calo/CaloUtils
                         Event/RecEvent
                         Phys/DaVinciKernel
                         Phys/DaVinciTools
                         Tr/TrackInterfaces)

gaudi_add_module(ParticleMaker
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES CaloInterfaces CaloUtils RecEvent DaVinciKernelLib)

