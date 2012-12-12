################################################################################
# Package: BooleSys
################################################################################
gaudi_subdir(BooleSys v26r0)

gaudi_depends_on_subdirs(Bcm/BcmDigi
                         Calo/CaloDigit
                         Calo/CaloMoniDigi
                         Digi/Boole
                         Digi/BooleUMC
                         Digi/DigiAlg
                         FT/FTDigitisation
                         GaudiConf
                         Muon/MuonAlgs
                         Muon/MuonBackground
                         Muon/MuonMoniDigi
                         OT/OTSimulation
                         Rich/RichDigiQC
                         Rich/RichDigiSys
                         Rich/RichMCAssociators
                         Rich/RichReadout
                         ST/STDigiAlgorithms
                         VL/VLDigitisation
                         VP/VPAlgorithms
                         VP/VPAssociators)


gaudi_add_test(QMTest QMTEST)
