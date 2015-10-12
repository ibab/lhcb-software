################################################################################
# Package: L0MuonMonitor
################################################################################
gaudi_subdir(L0MuonMonitor v2r4p1)

gaudi_depends_on_subdirs(Det/MuonDet
                         Event/DAQEvent
                         Event/L0Event
                         GaudiAlg
                         GaudiUtils
                         L0/L0MuonKernel
                         Muon/MuonDAQ)

gaudi_add_module(L0MuonMonitor
                 src/component/*.cpp
                 INCLUDE_DIRS Muon/MuonDAQ
                 LINK_LIBRARIES MuonDetLib DAQEventLib L0Event GaudiAlgLib GaudiUtilsLib L0MuonKernel)

