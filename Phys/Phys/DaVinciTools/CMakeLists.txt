################################################################################
# Package: DaVinciTools
################################################################################
gaudi_subdir(DaVinciTools v24r4)

gaudi_depends_on_subdirs(Phys/DaVinciKernel
                         Phys/LoKiCore)

gaudi_add_module(DaVinciTools
                 src/*.cpp
                 LINK_LIBRARIES DaVinciKernelLib LoKiCoreLib)

gaudi_install_python_modules()
