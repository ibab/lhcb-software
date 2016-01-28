################################################################################
# Package: TrackFitEvent
################################################################################
gaudi_subdir(TrackFitEvent v6r6)

gaudi_depends_on_subdirs(Det/FTDet
                         Det/MuonDet
                         Det/OTDet
                         Det/STDet
                         Det/VPDet
                         Det/VeloDet
                         Event/DigiEvent
                         Event/MCEvent
                         Event/TrackEvent
                         GaudiObjDesc
                         Kernel/LHCbKernel
                         Kernel/LHCbMath
                         OT/OTDAQ
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(TrackFitEvent
                  src/*.cpp
                  PUBLIC_HEADERS Event
                  INCLUDE_DIRS Event/DigiEvent OT/OTDAQ Tr/TrackInterfaces Tr/TrackKernel
                  LINK_LIBRARIES FTDetLib MuonDetLib OTDetLib STDetLib VPDetLib VeloDetLib MCEvent TrackEvent TrackKernel LHCbKernel LHCbMathLib)

god_build_dictionary(xml/*.xml
                     EXTEND Event/lcgDict.h
                     INCLUDE_DIRS Event/DigiEvent OT/OTDAQ Tr/TrackInterfaces Tr/TrackKernel
                     LINK_LIBRARIES FTDetLib MuonDetLib OTDetLib STDetLib VPDetLib VeloDetLib MCEvent TrackEvent TrackKernel LHCbKernel LHCbMathLib TrackFitEvent)

