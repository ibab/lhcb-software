################################################################################
# Package: TrackTools
################################################################################
gaudi_subdir(TrackTools v4r20)

gaudi_depends_on_subdirs(Calo/CaloUtils
                         Det/MuonDet
                         Det/OTDet
                         Det/RichDet
                         Det/STDet
                         Det/VPDet
                         Det/VeloDet
                         Event/DigiEvent
                         Event/TrackEvent
                         GaudiAlg
                         GaudiGSL
                         Kernel/LHCbKernel
                         Kernel/PartProp
                         Kernel/Relations
                         Muon/MuonInterfaces
                         OT/OTDAQ
                         ST/STKernel
                         Tf/TsaKernel
                         Tr/TrackFitEvent
                         Tr/TrackInterfaces
                         Tr/TrackKernel)

find_package(GSL)

gaudi_add_module(TrackTools
                 src/*.cpp
                 INCLUDE_DIRS GSL Event/DigiEvent OT/OTDAQ Tr/TrackInterfaces
                 LINK_LIBRARIES GSL CaloUtils MuonDetLib OTDetLib RichDetLib STDetLib VPDetLib VeloDetLib TrackEvent GaudiAlgLib GaudiGSLLib LHCbKernel PartPropLib RelationsLib MuonInterfacesLib STKernelLib TsaKernel TrackFitEvent TrackKernel)

