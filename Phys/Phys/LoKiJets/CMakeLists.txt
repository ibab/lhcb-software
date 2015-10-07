################################################################################
# Package: LoKiJets
################################################################################
gaudi_subdir(LoKiJets v6r13)

gaudi_depends_on_subdirs(Kernel/LHCbMath
                         Phys/LoKiAlgo)

find_package(FastJet)

gaudi_add_module(LoKiJets
                 src/*.cpp
                 INCLUDE_DIRS FastJet
                 LINK_LIBRARIES FastJet LHCbMathLib LoKiAlgo)

gaudi_install_python_modules()

