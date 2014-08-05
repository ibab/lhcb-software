################################################################################
# Package: DaVinciAngleCalculators
################################################################################
gaudi_subdir(DaVinciAngleCalculators v1r0)

gaudi_depends_on_subdirs(Phys/DaVinciKernel)

gaudi_add_module(DaVinciAngleCalculators
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib)
