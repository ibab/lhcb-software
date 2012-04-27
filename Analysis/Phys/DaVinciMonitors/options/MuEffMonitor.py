
#-----------------------------------------
#  Define the MuEffMonitor sequence
#-----------------------------------------

from Gaudi.Configuration import *
from Configurables import MuEffMonitor, TrackMasterExtrapolator, DaVinci, MuonFastPosTool

MuEffiSeq = GaudiSequencer("MuEffiSeq")



moneff = MuEffMonitor( "MuEffMonitor")
moneff.addTool(TrackMasterExtrapolator, name = "MuEffExtrap")
moneff.Extrapolator = moneff.MuEffExtrap

moneff.MuEffExtrap.ApplyMultScattCorr = True
moneff.MuEffExtrap.ApplyEnergyLossCorr = True
moneff.MuEffExtrap.MaterialLocator = "SimplifiedMaterialLocator" 
moneff.MuEffExtrap.OutputLevel = 5

moneff.RequiredStations = 4
moneff.MomentumCut = 5000.

##moneff.UseCalo  = False
moneff.EecalMax = 1000.
moneff.EhcalMax = 3500.
moneff.EhcalMin = 1000.

        
moneff.Chi2ProbTrMin = 0.01
moneff.Chi2MuMin = 10.
moneff.nSigmaXother =2.
moneff.nSigmaYother =2.

moneff.HistoLevel = "OfflineFull"        
moneff.OutputLevel = 4

##############################################################################
#
# Add Sequences to Monitors
#
MuEffiSeq.Members += [ moneff ]

DaVinci().MoniSequence += [ MuEffiSeq ] # Muon Chamber efficiency monitor sequence
