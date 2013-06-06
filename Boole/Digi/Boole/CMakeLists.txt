################################################################################
# Package: Boole
################################################################################
gaudi_subdir(Boole v26r6)

gaudi_depends_on_subdirs(Bcm/BcmDigi
			 Calo/CaloDigit
                         Calo/CaloMoniDigi
			 DAQ/MDF
                         Det/CaloDetXmlCnv
                         Det/DDDB
                         Det/DetDescSvc
                         Det/Magnet
			 Digi/BooleUMC
			 Digi/DigiAlg
                         Event/EventPacker
			 FT/FTDigitisation
                         Gaudi
                         GaudiConf
                         GaudiKernel
                         Kernel/LHCbAlgs
                         LbcomSys
                         L0/L0DU
			 Muon/MuonAlgs
			 Muon/MuonBackground
			 Muon/MuonMoniDigi
			 OT/OTSimulation
			 Rich/RichDigiQC
			 Rich/RichMCAssociators
			 Rich/RichDigiSys
			 Rich/RichReadout
                         Sim/SimComponents
			 ST/STDigiAlgorithms
                         Tools/XmlTools
			 VL/VLDigitisation
			 VP/VPAlgorithms
			 VP/VPAssociators)

gaudi_install_python_modules()

gaudi_env(SET BOOLEOPTS \${BOOLEROOT}/options)


gaudi_add_test(QMTest QMTEST)
