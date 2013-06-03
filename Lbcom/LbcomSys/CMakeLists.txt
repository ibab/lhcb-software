################################################################################
# Package: LbcomSys
################################################################################
gaudi_subdir(LbcomSys v14r0)

gaudi_depends_on_subdirs(Associators/Associators
                         Calo/CaloAssociators
                         Calo/CaloTools
                         DAQ/DAQMonitors
                         FT/FTDAQ
                         Hlt/HltDAQ
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
                         VL/VLDAQ
                         VP/VPDAQ
                         VP/VPTools
                         Velo/VeloAlgorithms
                         Velo/VeloAssociators
                         Velo/VeloSimulation
                         Velo/VeloTools)


gaudi_add_test(QMTest QMTEST)
