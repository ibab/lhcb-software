################################################################################
# Package: DaVinciTools
################################################################################
gaudi_subdir(DaVinciTools v23r15)

gaudi_depends_on_subdirs(Phys/DaVinciKernel)

gaudi_add_module(DaVinciTools
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib)

gaudi_install_python_modules()
