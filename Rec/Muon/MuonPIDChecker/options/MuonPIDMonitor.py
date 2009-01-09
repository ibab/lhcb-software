from Configurables import GaudiSequencer,MuonPIDChecker
monitor = MuonPIDChecker( "MuonPIDMonitor",
                           HistoTopDir = "Muon/",
                           DLLCut = -1.0,
                           NSHCut = 1.,
                           RunningMC = False,
                           HistoDir = "MuonPID/Monitor" )
GaudiSequencer("MoniMUONSeq").Members += [monitor]
