################################################################################
# Package: GaussSys
################################################################################
gaudi_subdir(GaussSys v46r4)

gaudi_depends_on_subdirs(Bcm/BcmMoniSim
                         Bls/BlsMoniSim
                         Calo/CaloMoniSim
                         Gen/BcVegPy
                         Gen/EvtGen
                         Gen/EvtGenExtras
                         Gen/GENSER
                         Gen/GenCuts
                         Gen/GenTests
                         Gen/GenTune
                         Gen/GenXicc
                         Gen/Generators
                         Gen/LbAlpGen
                         Gen/LbBcVegPy
                         Gen/LbGenXicc
                         Gen/LbHerwigpp
                         Gen/LbHidValley
                         Gen/LbHijing
                         Gen/LbMIB
                         Gen/LbOniaPairs
                         Gen/LbPGuns
                         Gen/LbPowheg
                         Gen/LbPythia
                         Gen/LbPythia8
                         Gen/Mint
                         Muon/MuonMoniSim
                         Sim/Gauss
                         Sim/GaussAlgs
                         Sim/GaussCalo
                         Sim/GaussCherenkov
                         Sim/GaussKine
                         Sim/GaussMonitor
                         Sim/GaussPhysics
                         Sim/GaussRICH
                         Sim/GaussTools
                         Sim/GaussTracker
                         Sim/GaussUMC
                         Sim/GiGa
                         Sim/GiGaCnv
                         Sim/GiGaVisUI
                         Sim/LbGDML
                         Sim/SimChecks
                         Sim/SimSvc
                         Tools/ClhepTools
                         VL/VLMoniSim
                         Velo/VeloGauss
                         Vis/VisSvc
                         Vis/XmlVis)


gaudi_add_test(QMTest QMTEST)
