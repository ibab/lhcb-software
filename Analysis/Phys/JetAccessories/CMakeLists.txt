################################################################################
# Package: JetAccessories
################################################################################
gaudi_subdir(JetAccessories v2r0)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Kernel/LHCbMath
                         Phys/LoKiAlgo
                         Phys/LoKiJets)

find_package(FastJet)

gaudi_add_module(JetAccessories
                 src/*.cpp
                 INCLUDE_DIRS FastJet
                 LINK_LIBRARIES FastJet CaloUtils LHCbMathLib LoKiAlgo)

gaudi_install_python_modules()

