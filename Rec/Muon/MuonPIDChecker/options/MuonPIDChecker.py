from Configurables import GaudiSequencer,MuonPIDChecker
GaudiSequencer("CheckMUONSeq").Members += [ "MuonPIDChecker" ]
MuonPIDChecker().HistoTopDir = "Muon/"
MuonPIDChecker().DLLCut = -1.0
MuonPIDChecker().NSHCut = 1.
MuonPIDChecker().RunningMC = True
MuonPIDChecker().HistoDir = "MuonPID/Checker"
