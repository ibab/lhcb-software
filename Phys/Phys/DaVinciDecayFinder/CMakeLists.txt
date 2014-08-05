################################################################################
# Package: DaVinciDecayFinder
################################################################################
gaudi_subdir(DaVinciDecayFinder v1r0)

gaudi_depends_on_subdirs(Phys/DaVinciKernel)

gaudi_add_module(DaVinciDecayFinder
                 src/*.cpp
                 INCLUDE_DIRS Boost AIDA
                 LINK_LIBRARIES DaVinciKernelLib)
