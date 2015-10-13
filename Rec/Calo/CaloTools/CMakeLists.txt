################################################################################
# Package: CaloTools
################################################################################
gaudi_subdir(CaloTools v6r10p1)

gaudi_depends_on_subdirs(Calo/CaloInterfaces
                         Calo/CaloUtils
                         Det/CaloDet
                         Event/RecEvent
                         Event/TrackEvent
                         GaudiAlg
                         Kernel/LHCbKernel
                         Kernel/LHCbMath
                         Kernel/Relations
                         Tr/TrackInterfaces)

gaudi_add_module(CaloTools
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES CaloInterfaces CaloUtils CaloDetLib RecEvent TrackEvent GaudiAlgLib LHCbKernel LHCbMathLib RelationsLib)

