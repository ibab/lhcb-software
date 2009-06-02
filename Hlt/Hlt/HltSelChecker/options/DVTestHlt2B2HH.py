### $Id: DVTestHlt2B2HH.py,v 1.16 2009-06-02 08:07:32 pkoppenb Exp $
 #
 #  Test file for HLT B->hh selection
 #
 #  @author P. Koppenburg
 #  @date 2007-05-05
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, DecayTreeTuple, PrintDecayTree, CheckSelResult
#--------------------------------------------------------------
signal = "B2HH"
#
# True filter criterion
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "{[B0 -> ^pi- ^pi+ {,gamma}{,gamma}{,gamma}]cc, [[B0]nos -> ^K+ ^pi- {,gamma}{,gamma}{,gamma}]cc, [[B0]os -> ^K- ^pi+ {,gamma}{,gamma}{,gamma}]cc, [[B_s0]nos -> ^K- ^pi+ {,gamma}{,gamma}{,gamma}]cc, [[B_s0]os -> ^K+ ^pi- {,gamma}{,gamma}{,gamma}]cc, [B_s0 -> ^K- ^K+ {,gamma}{,gamma}{,gamma}]cc}"
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = TRUE  
#
# Overwrite input - uncomment to run HLT on TRUE signal only
#
# importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
# Monitoring
#
moni = GaudiSequencer("Hlt2MonitorSeq")
moni.IgnoreFilterPassed = True
moni.Context = "HLT"
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Correlations.py")
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
###
 # Tuple
###
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").InputLocations = ["Hlt2B2HHCombine"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "B0 -> ^pi+ ^pi-"
#
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
DaVinci().UserAlgorithms = [ ] 
DaVinci().MoniSequence += [ moni, DecayTreeTuple("Hlt2DecayTreeTuple") ] 
DaVinci().Input = [
  "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/B2HH-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
