################################################################################
# Package: PrUtils
################################################################################
gaudi_subdir(PrUtils v2r0p1)

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
                         Pr/PrKernel
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

gaudi_add_module(PrUtils
                 src/*.cpp
                 INCLUDE_DIRS Tr/TrackInterfaces
                 LINK_LIBRARIES CaloDetLib OTDetLib STDetLib HltEvent
LinkerEvent TrackEvent GaudiAlgLib HltInterfaces LHCbMathLib PartPropLib
PrKernel TrackFitEvent TrackKernel)


