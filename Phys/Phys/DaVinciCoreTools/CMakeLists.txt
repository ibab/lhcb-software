################################################################################
# Package: DaVinciCoreTools
################################################################################
gaudi_subdir(DaVinciCoreTools v1r0)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Phys/LoKiArrayFunctors
                         Phys/LoKiPhys)

gaudi_add_module(DaVinciCoreTools
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib LoKiArrayFunctorsLib LoKiPhysLib)
