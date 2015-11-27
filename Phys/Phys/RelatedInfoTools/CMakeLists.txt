################################################################################
# Package: RelatedInfoTools
################################################################################
gaudi_subdir(RelatedInfoTools v1r4)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Phys/LoKiPhys)

gaudi_add_module(RelatedInfoTools
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib LoKiPhysLib)
