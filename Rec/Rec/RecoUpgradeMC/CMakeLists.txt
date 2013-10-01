################################################################################
# Package: RecoUpgradeMC
################################################################################
gaudi_subdir(RecoUpgradeMC v2r2)

gaudi_depends_on_subdirs(Det/VPDet
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         Event/TrackEvent
                         Kernel/LHCbMath
                         Kernel/PartProp
                         Tf/PatKernel
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

gaudi_add_module(RecoUpgradeMC
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces Tf/PatKernel
                 LINK_LIBRARIES VPDetLib LinkerEvent MCEvent RecEvent TrackEvent LHCbMathLib PartPropLib TrackFitEvent TrackKernel)

