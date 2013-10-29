################################################################################
# Package: DaVinciTools
################################################################################
gaudi_subdir(DaVinciTools v23r11)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Event/HltEvent
                         Event/MicroDst
                         GaudiConfUtils
                         Kernel/LHCbMath
                         Phys/DaVinciKernel
                         Phys/DecayTreeFitter
                         Phys/LoKiArrayFunctors
                         Phys/LoKiPhys)

find_package(Boost COMPONENTS regex)

gaudi_add_module(DaVinciTools
                 src/*.cpp
                 INCLUDE_DIRS Boost AIDA
                 LINK_LIBRARIES Boost CaloUtils HltEvent MicroDstLib LHCbMathLib DaVinciKernelLib DecayTreeFitter LoKiArrayFunctorsLib LoKiPhysLib)

gaudi_install_python_modules()


gaudi_add_test(QMTest QMTEST)
