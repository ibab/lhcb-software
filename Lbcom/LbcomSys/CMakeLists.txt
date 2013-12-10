################################################################################
# Package: LbcomSys
################################################################################
gaudi_subdir(LbcomSys v14r4)

gaudi_depends_on_subdirs(Associators/Associators
                         Calo/CaloAssociators
                         DAQ/DAQMonitors
                         L0/L0MuonMonitor
                         L0/PuVeto
                         Muon/MuonAssociators
                         OT/OTAssociators
                         OT/OTMonitor
                         Rich/RichAlgorithms
                         Rich/RichMCMonitors
                         Rich/RichMCTools
                         Rich/RichTools
                         ST/STAlgorithms
                         ST/STAssociators
                         ST/STCheckers
                         ST/STMonitors
                         ST/STTools
                         Si/SiTools
                         VP/VPTools
                         Velo/VeloAlgorithms
                         Velo/VeloAssociators
                         Velo/VeloSimulation
                         Velo/VeloTools)


gaudi_add_test(QMTest QMTEST)
