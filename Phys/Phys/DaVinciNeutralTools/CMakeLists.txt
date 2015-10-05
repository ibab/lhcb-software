################################################################################
# Package: DaVinciNeutralTools
################################################################################
gaudi_subdir(DaVinciNeutralTools v1r1)

gaudi_depends_on_subdirs(Phys/DaVinciKernel)

gaudi_add_module(DaVinciNeutralTools
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib)
