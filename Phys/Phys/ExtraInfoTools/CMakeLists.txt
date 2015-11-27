################################################################################
# Package: ExtraInfoTools
################################################################################
gaudi_subdir(ExtraInfoTools v1r2)

gaudi_depends_on_subdirs(Phys/DaVinciKernel)

gaudi_add_module(ExtraInfoTools
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib)
