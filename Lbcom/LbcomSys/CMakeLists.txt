################################################################################
# Package: LbcomSys
################################################################################
gaudi_subdir(LbcomSys v17r1)

gaudi_depends_on_subdirs(Associators/Associators
                         Calo/CaloAssociators
                         DAQ/DAQMonitors
                         HC/HCMonitors
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
                         VP/VPAlgorithms
                         VP/VPAssociators
                         VP/VPCheckers
                         VP/VPTools
                         Velo/VeloAlgorithms
                         Velo/VeloAssociators
                         Velo/VeloSimulation
                         Velo/VeloTools)


gaudi_add_test(QMTest QMTEST)
