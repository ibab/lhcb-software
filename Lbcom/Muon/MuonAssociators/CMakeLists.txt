################################################################################
# Package: MuonAssociators
################################################################################
gaudi_subdir(MuonAssociators v3r5)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/DigiEvent
                         Event/EventBase
                         Event/LinkerEvent
                         Event/MCEvent
                         Event/RecEvent
                         GaudiAlg
                         Kernel/MCInterfaces
                         Muon/MuonDAQ)

gaudi_add_module(MuonAssociators
                 src/*.cpp
                 INCLUDE_DIRS Event/DigiEvent Event/EventBase Kernel/MCInterfaces Muon/MuonDAQ
                 LINK_LIBRARIES MuonDetLib LinkerEvent MCEvent RecEvent GaudiAlgLib)

