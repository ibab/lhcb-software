################################################################################
# Package: ParticleMakerChecker
################################################################################
gaudi_subdir(ParticleMakerChecker v1r6)

gaudi_depends_on_subdirs(Kernel/MCInterfaces
                         Phys/DaVinciKernel
                         Phys/DaVinciMCKernel)

gaudi_add_module(ParticleMakerChecker
                 src/*.cpp
                 INCLUDE_DIRS Kernel/MCInterfaces
                 LINK_LIBRARIES DaVinciKernelLib DaVinciMCKernelLib)

