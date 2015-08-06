################################################################################
# Package: DaVinciFilters
################################################################################
gaudi_subdir(DaVinciFilters v1r2)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Phys/LoKiArrayFunctors
                         Phys/DecayTreeFitter)

gaudi_add_module(DaVinciFilters
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib LoKiArrayFunctorsLib DecayTreeFitter)
