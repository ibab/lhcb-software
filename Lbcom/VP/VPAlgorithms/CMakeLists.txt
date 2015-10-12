################################################################################
# Package: VPAlgorithms
################################################################################
gaudi_subdir(VPAlgorithms v4r4p1)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/DigiEvent
                         GaudiAlg
                         Kernel/LHCbKernel)

gaudi_add_module(VPAlgorithms
                 src/*.cpp
                 INCLUDE_DIRS AIDA Event/DigiEvent 
                 LINK_LIBRARIES VPDetLib GaudiAlgLib LHCbKernel)

