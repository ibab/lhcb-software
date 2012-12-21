################################################################################
# Package: TrackFitEvent
################################################################################
gaudi_subdir(TrackFitEvent v5r19)

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
                         Tr/TrackInterfaces)

include(GaudiObjDesc)

god_build_headers(xml/*.xml)

gaudi_add_library(TrackFitEvent
                  src/*.cpp
                  PUBLIC_HEADERS Event
                  INCLUDE_DIRS Event/DigiEvent OT/OTDAQ Tr/TrackInterfaces
                  LINK_LIBRARIES FTDetLib MuonDetLib OTDetLib STDetLib VPDetLib VeloDetLib MCEvent TrackEvent LHCbKernel LHCbMathLib)

god_build_dictionary(xml/*.xml
                     EXTEND Event/lcgDict.h
                     INCLUDE_DIRS Event/DigiEvent OT/OTDAQ Tr/TrackInterfaces
                     LINK_LIBRARIES FTDetLib MuonDetLib OTDetLib STDetLib VPDetLib VeloDetLib MCEvent TrackEvent LHCbKernel LHCbMathLib TrackFitEvent)

