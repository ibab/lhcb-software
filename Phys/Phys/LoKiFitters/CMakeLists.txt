################################################################################
# Package: LoKiFitters
################################################################################
gaudi_subdir(LoKiFitters v4r19)

gaudi_depends_on_subdirs(Kernel/LHCbMath
                         Phys/DaVinciKernel
                         Phys/DecayTreeFitter
                         Phys/KalmanFilter
                         Phys/LoKiPhys)

gaudi_add_module(LoKiFitters
                 src/*.cpp
                 LINK_LIBRARIES LHCbMathLib DaVinciKernelLib DecayTreeFitter KalmanFilter LoKiPhysLib)

