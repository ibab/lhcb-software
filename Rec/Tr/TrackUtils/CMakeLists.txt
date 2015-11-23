################################################################################
# Package: TrackUtils
################################################################################
gaudi_subdir(TrackUtils v1r62)

gaudi_depends_on_subdirs(Det/CaloDet
                         Det/OTDet
                         Det/STDet
                         Event/HltEvent
                         Event/LinkerEvent
                         Event/TrackEvent
                         GaudiAlg
                         Kernel/HltInterfaces
                         Kernel/LHCbMath
                         Kernel/PartProp
                         Tf/TfKernel
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

gaudi_add_module(TrackUtils
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces Tf/TfKernel
                 LINK_LIBRARIES CaloDetLib OTDetLib STDetLib HltEvent LinkerEvent TrackEvent GaudiAlgLib HltInterfaces LHCbMathLib PartPropLib TrackFitEvent TrackKernel)

