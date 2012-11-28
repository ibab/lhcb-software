################################################################################
# Package: STAlgorithms
################################################################################
gaudi_subdir(STAlgorithms v4r5)

gaudi_depends_on_subdirs(Det/STDet
                         Event/DigiEvent
                         Event/RecEvent
                         GaudiAlg
                         GaudiGSL
                         GaudiKernel
                         Kernel/LHCbKernel
                         ST/STKernel)

gaudi_add_module(STAlgorithms
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent
                 LINK_LIBRARIES STDetLib RecEvent GaudiAlgLib GaudiGSLLib GaudiKernel LHCbKernel STKernelLib)

