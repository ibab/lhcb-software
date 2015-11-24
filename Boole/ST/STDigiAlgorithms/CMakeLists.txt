################################################################################
# Package: STDigiAlgorithms
################################################################################
gaudi_subdir(STDigiAlgorithms v2r7)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DigiEvent
                         Event/MCEvent
                         GaudiAlg
                         GaudiGSL
                         GaudiKernel
                         Kernel/LHCbMath
                         Kernel/MCInterfaces
                         ST/STKernel)

find_package(GSL)

gaudi_add_module(STDigiAlgorithms
                 src/*.cpp
                 INCLUDE_DIRS GSL Event/DigiEvent Kernel/MCInterfaces
                 LINK_LIBRARIES GSL STDetLib MCEvent GaudiAlgLib GaudiGSLLib GaudiKernel LHCbMathLib STKernelLib)

