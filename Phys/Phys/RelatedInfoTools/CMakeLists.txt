################################################################################
# Package: RelatedInfoTools
################################################################################
gaudi_subdir(RelatedInfoTools v1r0)

gaudi_depends_on_subdirs(Phys/DaVinciKernel)

gaudi_add_module(RelatedInfoTools
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib)
