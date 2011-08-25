
#-----------------------------------------
#  Define the MuEffMonitor sequence
#-----------------------------------------

from Gaudi.Configuration import *
from Configurables import MuEffMonitor, TrackMasterExtrapolator, DaVinci

MuEffiSeq = GaudiSequencer("MuEffiSeq")

moneff = MuEffMonitor( "MuEffMonitor")
moneff.addTool(TrackMasterExtrapolator, name = "MuEffExtrap")
moneff.Extrapolator = moneff.MuEffExtrap
        
moneff.MuEffExtrap.ApplyMultScattCorr = True
moneff.MuEffExtrap.ApplyEnergyLossCorr = True
moneff.MuEffExtrap.MaterialLocator = "SimplifiedMaterialLocator" 
moneff.MuEffExtrap.OutputLevel = 5

moneff.MomentumCut = 3000.

moneff.EecalMax = 1000.
moneff.EecalMin = -100.                                   
moneff.EhcalMax = 3500.
moneff.EhcalMin = 1000.
        
moneff.nSigmaX = [2.,3.5,3.5,3.5,3.5]
moneff.nSigmaY = [3.5,2.,2.,2.,2.]
moneff.Chi2Min = 10
moneff.CosThetaCut = 0.99
moneff.xyDistCut = 60.
moneff.PCutEff = 15.     
        
moneff.OutputLevel = 6


##############################################################################
#
# Add Sequences to Monitors
#
MuEffiSeq.Members += [ moneff ]

DaVinci().MoniSequence += [ MuEffiSeq ] # Muon Chamber efficiency monitor sequence
