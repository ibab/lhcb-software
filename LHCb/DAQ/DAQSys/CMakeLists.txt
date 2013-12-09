################################################################################
# Package: DAQSys
################################################################################
gaudi_subdir(DAQSys v7r2)

gaudi_depends_on_subdirs(Calo/CaloDAQ
                         L0/L0Calo
                         L0/L0DU
                         L0/L0Muon
                         Muon/MuonDAQ
                         OT/OTDAQ
                         Rich/RichDAQ
                         Si/SiDAQ
			 ST/STDAQ
			 Velo/VeloDAQ)

