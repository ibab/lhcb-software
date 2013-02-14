################################################################################
# Package: LbcomSys
################################################################################
gaudi_subdir(LbcomSys v13r4)

gaudi_depends_on_subdirs(Associators/Associators
                         Calo/CaloAssociators
                         Calo/CaloTools
                         DAQ/DAQMonitors
                         FT/FTDAQ
                         Hlt/HltDAQ
                         L0/L0Calo
                         L0/L0DU
                         L0/L0Muon
                         L0/L0MuonMonitor
                         L0/PuVeto
                         Muon/MuonAssociators
                         OT/OTAssociators
                         OT/OTMonitor
                         Rich/RichAlgorithms
                         Rich/RichDAQ
                         Rich/RichMCMonitors
                         Rich/RichMCTools
                         Rich/RichTools
                         ST/STAlgorithms
                         ST/STAssociators
                         ST/STCheckers
                         ST/STDAQ
                         ST/STMonitors
                         ST/STTools
                         Si/SiTools
                         VL/VLDAQ
                         VP/VPDAQ
                         VP/VPTools
                         Velo/VeloAlgorithms
                         Velo/VeloAssociators
                         Velo/VeloDAQ
                         Velo/VeloSimulation
                         Velo/VeloTools)


gaudi_add_test(QMTest QMTEST)
