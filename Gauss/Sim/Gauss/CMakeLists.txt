################################################################################
# Package: Gauss
################################################################################
gaudi_subdir(Gauss v46r4)

gaudi_depends_on_subdirs(Bcm/BcmMoniSim
                         Bls/BlsMoniSim
                         Calo/CaloMoniSim
                         Det/DDDB
                         Det/DetSys
                         Event/EventPacker
                         Gaudi
                         GaudiConf
                         GaudiKernel
                         Gen/LbAlpGen
                         Gen/LbBcVegPy
                         Gen/LbHerwigpp
                         Gen/LbHidValley
                         Gen/LbHijing
                         Gen/LbMIB
                         Gen/LbPGuns
                         Gen/LbPythia
                         Gen/LbPythia8
                         Kernel/KernelSys
                         Kernel/LHCbAlgs
                         Muon/MuonMoniSim
                         Sim/GaussAlgs
                         Sim/GaussCalo
                         Sim/GaussKine
                         Sim/GaussMonitor
                         Sim/GaussPhysics
                         Sim/GaussRICH
                         Sim/GaussTools
                         Sim/GaussTracker
                         Sim/GaussUMC
                         Sim/GiGaCnv
                         Sim/SimComponents
                         Velo/VeloGauss
                         Vis/XmlVis)

find_package(HepMC)

gaudi_install_python_modules()
gaudi_install_scripts()

gaudi_env(SET AlwaysKillLeadingHadron 1
          SET GAUSSOPTS \${GAUSSROOT}/options)


gaudi_add_test(QMTest QMTEST)
