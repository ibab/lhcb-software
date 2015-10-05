################################################################################
# Package: DaVinciOverlapsAndClones
################################################################################
gaudi_subdir(DaVinciOverlapsAndClones v1r1)

gaudi_depends_on_subdirs(Phys/DaVinciKernel)

gaudi_add_module(DaVinciOverlapsAndClones
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib)
