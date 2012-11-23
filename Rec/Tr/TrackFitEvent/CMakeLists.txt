################################################################################
# Package: TrackFitEvent
################################################################################
gaudi_subdir(TrackFitEvent v5r18)

gaudi_depends_on_subdirs(Det/MuonDet
                         Det/OTDet
                         Det/STDet
                         Det/VPDet
                         Det/VeloDet
                         Event/DigiEvent
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
                  LINK_LIBRARIES MuonDetLib OTDetLib STDetLib VPDetLib VeloDetLib TrackEvent LHCbKernel LHCbMathLib)

god_build_dictionary(xml/*.xml
                     EXTEND Event/lcgDict.h
                     INCLUDE_DIRS Event/DigiEvent OT/OTDAQ Tr/TrackInterfaces
                     LINK_LIBRARIES MuonDetLib OTDetLib STDetLib VPDetLib VeloDetLib TrackEvent LHCbKernel LHCbMathLib TrackFitEvent)

