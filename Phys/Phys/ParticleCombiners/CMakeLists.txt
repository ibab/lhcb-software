################################################################################
# Package: ParticleCombiners
################################################################################
gaudi_subdir(ParticleCombiners v1r3)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Phys/LoKiArrayFunctors
                         Phys/LoKiPhys)

gaudi_add_module(ParticleCombiners
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib LoKiArrayFunctorsLib LoKiPhysLib)
