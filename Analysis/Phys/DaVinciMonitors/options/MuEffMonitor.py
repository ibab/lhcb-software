
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
moneff.MomentumCut = 8000.

moneff.UseCalo  = False
moneff.EecalMax = 1000.
moneff.EhcalMax = 3500.
moneff.EhcalMin = 500.

        
moneff.Chi2ProbTrMin = 0.01
moneff.Chi2MuMin = 10.
moneff.Chi2OtherMuMin =2.
moneff.CosThetaCut = 0.99
moneff.xyDistCut = 20.

moneff.HistoLevel = "OfflineFull"        
moneff.OutputLevel = 3
moneff.MeasureTime = True

##############################################################################
#
# Add Sequences to Monitors
#
MuEffiSeq.Members += [ moneff ]

DaVinci().MoniSequence += [ MuEffiSeq ] # Muon Chamber efficiency monitor sequence
