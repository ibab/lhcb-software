################################################################################
# Package: DigiAlg
################################################################################
gaudi_subdir(DigiAlg v4r22)

gaudi_depends_on_subdirs(Det/OTDet
                         Det/STDet
                         Det/VeloDet
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
                 LINK_LIBRARIES OTDetLib STDetLib VeloDetLib DAQEventLib GenEvent LinkerEvent MCEvent GaudiAlgLib)

