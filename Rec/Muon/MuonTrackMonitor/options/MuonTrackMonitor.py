from Configurables import GaudiSequencer,MuonTrackMonitor,MuonEfficiencyMonitor,MuonAlignmentMonitor

montracks = MuonTrackMonitor( "MuonTrackMonitor",
                              HistoTopDir = "Muon/"
                              )
GaudiSequencer("MoniMUONSeq").Members += [montracks]

moneffi = MuonEfficiencyMonitor( "MuonEfficiencyMonitor",
                                 HistoTopDir = "Muon/"
                                 )
GaudiSequencer("MoniMUONSeq").Members += [moneffi]

monalig = MuonAlignmentMonitor( "MuonAlignmentMonitor",
                                 HistoTopDir = "Muon/"
                                 )
GaudiSequencer("MoniMUONSeq").Members += [monalig]
