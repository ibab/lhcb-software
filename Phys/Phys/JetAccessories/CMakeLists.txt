################################################################################
# Package: JetAccessories
################################################################################
gaudi_subdir(JetAccessories v3r14)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Kernel/LHCbMath
                         Phys/LoKiAlgo
                         Phys/LoKiJets)

find_package(FastJet)

gaudi_install_headers(Kernel)

gaudi_add_module(JetAccessories
                 src/*.cpp
                 INCLUDE_DIRS FastJet
                 LINK_LIBRARIES FastJet CaloUtils LHCbMathLib LoKiAlgo)

gaudi_install_python_modules()

gaudi_add_dictionary(JetAccessories
                     dict/JetAccessoriesDict.h
                     dict/JetAccessories.xml
                     LINK_LIBRARIES FastJet CaloUtils LHCbMathLib LoKiAlgo
                     OPTIONS "-U__MINGW32__")
