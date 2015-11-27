################################################################################
# Package: LoKiExample
################################################################################
gaudi_subdir(LoKiExample v8r10)

gaudi_depends_on_subdirs(Kernel/LHCbMath
                         Phys/LoKi
                         Phys/LoKiAlgoMC
                         Phys/LoKiArrayFunctors
                         Phys/LoKiGenMC)

gaudi_add_module(LoKiExample
                 src/*.cpp
                 INCLUDE_DIRS Phys/LoKi
                 LINK_LIBRARIES LHCbMathLib LoKiAlgoMCLib LoKiArrayFunctorsLib LoKiGenMCLib)

gaudi_install_python_modules()

