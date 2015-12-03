################################################################################
# Package: JetAccessoriesMC
################################################################################
gaudi_subdir(JetAccessoriesMC v2r10)

gaudi_depends_on_subdirs(Kernel/LHCbMath
                         Phys/JetAccessories
                         Phys/JetTagging
                         Phys/LoKiAlgoMC
                         Phys/LoKiGenMC
                         Phys/LoKiPhysMC)

find_package(FastJet)

gaudi_add_module(JetAccessoriesMC
                 src/*.cpp
                 INCLUDE_DIRS FastJet
                 LINK_LIBRARIES FastJet LHCbMathLib LoKiAlgoMCLib LoKiGenMCLib LoKiPhysMCLib)

gaudi_install_headers(Kernel)
gaudi_install_python_modules()

