### @file 
 #  Test file for HLT
 #
 #  @author P. Koppenburg
 #  @date 2006-02-01
 #
 ####################################################
from Gaudi.Configuration import *
from Configurables import Z0Checker
signal = "Z0"
#
# Monitoring
#
moni = GaudiSequencer("Hlt2MonitorSeq")
moni.IgnoreFilterPassed = True
moni.Context = "HLT"
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Correlations.py")
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
#
# various instances of Z0 checker
#
Z0 = GaudiSequencer("Z0")
Z0.Members += [ Z0Checker("OffNoIDZ0Checker") ]  
Z0Checker("OffNoIDZ0Checker").Locations = [ "/Event/Phys/StdNoPIDsMuons/Particles" ]
Z0.Members += [ Z0Checker("OffMuZ0Checker") ]  
Z0Checker("OffMuZ0Checker").Locations = [ "/Event/Phys/StdLooseMuons/Particles" ]
Z0.Members += [ Z0Checker("HltPiZ0Checker") ]  
Z0Checker("HltPiZ0Checker").Locations = [ "/Event/HLT/Hlt2Pions/Particles" ]
Z0.Members += [ Z0Checker("HltMuZ0Checker") ]  
Z0Checker("HltMuZ0Checker").Locations = [ "/Event/HLT/Hlt2Muons/Particles" ]

#
# Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = -1
DaVinci().HltType = "Hlt1+Hlt2"                # Both Hlt levels
DaVinci().Hlt2Requires = 'L0'                  # Ignore Hlt1 in 2
DaVinci().ReplaceL0BanksWithEmulated = False   # Redo L0
DaVinci().DataType = "DC06" 
DaVinci().Simulation = True 
DaVinci().TupleFile =  "HLT-"+signal+".root"
DaVinci().HistogramFile = "DVHlt2-"+signal+".root"
DaVinci().MoniSequence += [ moni, Z0 ]
DaVinci().Input = [
  "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/Z02MuMu.dst' TYP='POOL_ROOTTREE' OPT='READ'" 
]
