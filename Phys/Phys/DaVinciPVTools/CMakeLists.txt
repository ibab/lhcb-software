################################################################################
# Package: DaVinciPVTools
################################################################################
gaudi_subdir(DaVinciPVTools v1r2)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Event/MicroDst)

gaudi_add_module(DaVinciPVTools
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib MicroDstLib)

gaudi_install_python_modules()
