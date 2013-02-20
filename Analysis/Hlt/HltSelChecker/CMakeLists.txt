################################################################################
# Package: HltSelChecker
################################################################################
gaudi_subdir(HltSelChecker v12r1p3)

gaudi_depends_on_subdirs(Event/HltEvent
                         Kernel/HltInterfaces
                         Phys/DaVinciKernel
                         Phys/DaVinciMCKernel)

gaudi_add_module(HltSelChecker
                 src/*.cpp
                 INCLUDE_DIRS AIDA
                 LINK_LIBRARIES HltEvent HltInterfaces DaVinciKernelLib DaVinciMCKernelLib)

gaudi_install_python_modules()

