################################################################################
# Package: DigiAlg
################################################################################
gaudi_subdir(DigiAlg v5r4)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Det/VeloDet
                         Det/VPDet
                         Det/FTDet
                         Event/DAQEvent
                         Event/DigiEvent
                         Event/EventBase
                         Event/GenEvent
                         Event/LinkerEvent
                         Event/MCEvent
                         GaudiAlg)

gaudi_add_module(DigiAlg
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Event/EventBase
                 LINK_LIBRARIES OTDetLib STDetLib VeloDetLib VPDetLib FTDetLib DAQEventLib GenEvent LinkerEvent MCEvent GaudiAlgLib)

