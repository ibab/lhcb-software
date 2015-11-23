################################################################################
# Package: STAlgorithms
################################################################################
gaudi_subdir(STAlgorithms v4r7p2)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DigiEvent
                         Event/RecEvent
                         GaudiAlg
                         GaudiGSL
                         GaudiKernel
                         Kernel/LHCbKernel
                         ST/STKernel)

find_package(AIDA)

gaudi_add_module(STAlgorithms
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent AIDA
                 LINK_LIBRARIES STDetLib RecEvent GaudiAlgLib GaudiGSLLib GaudiKernel LHCbKernel STKernelLib)

