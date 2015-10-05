################################################################################
# Package: DaVinciDecayFinder
################################################################################
gaudi_subdir(DaVinciDecayFinder v1r1)

gaudi_depends_on_subdirs(Phys/DaVinciKernel)

gaudi_add_module(DaVinciDecayFinder
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib)
