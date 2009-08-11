### $Id: DVTestHlt2Bu2D0K_KsHH.py,v 1.19 2009-08-11 08:07:22 pkoppenb Exp $
 #
 #  Test file for HLT Bd->D0(Kpi)K*(Kpi) selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-25
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, DecayTreeTuple, PrintHeader, CheckSelResult
#--------------------------------------------------------------
signal = "Bu2D0K_KsHH"
#
# True filter criterion
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder)
FilterTrueTracks().MCDecayFinder.Decay =  "[B+ -> (D~0 -> (KS0 -> ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^K+ {,gamma}{,gamma}]cc" 
#
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqTrueSignalTracks").IgnoreFilterPassed = True  
#
# Overwrite input - uncomment to run HLT on True signal only
#
# importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
#
# Monitoring
#
moni = GaudiSequencer("Hlt2MonitorSeq")
moni.IgnoreFilterPassed = True
moni.Context = "HLT"
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Correlations.py")
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
importOptions( "$HLTSELCHECKERROOT/options/Hlt2EventTuple.py" )
###
 # Tuple
###
#importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
#DecayTreeTuple("Hlt2DecayTreeTuple").InputLocations = ["Hlt2Bu2D0K_KsHHCombine"]
#DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "{[B+ -> (^D0 -> (^KS0 -> ^pi+ ^pi-) ^pi+ ^pi-) ^K+]cc, [B+ -> (^D~0 -> (^KS0 -> ^pi+ ^pi-) ^pi+ ^pi-) ^K+]cc}"
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
DaVinci().MoniSequence += [ moni ]#, DecayTreeTuple("Hlt2DecayTreeTuple") ]
DaVinci().Input = [
  "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/Bu2KD-KSPiPi-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
