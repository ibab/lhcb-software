### @file DVTestHlt2B2HH.opts
 #
 #  Test file for HLT B->hh selection
 #
 #  @author P. Koppenburg
 #  @date 2007-05-05
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, PrintTree, CheckSelResult
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$B2UDSROOT/options/DVPreselB2HH.opts")
#
# Run correlations only on offline selected events
#
GaudiSequencer("Hlt2CorrsSeq").Members += [ CheckSelResult("CheckOffline") ]
CheckSelResult("CheckOffline").Algorithms += [ "PreselB2HH" ]
#
# Hlt test
#
importOptions( "$HLTSELECTIONSROOT/options/DVTestHlt2.py")
from HltConf.Configuration import *
HltConf().Hlt2IgnoreHlt1Decision = True  # do both Hlt1 and 2
HltConf().applyConf()
#
# Plots
#
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
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
###
 # Tuple
###
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Hlt2SelB2HH"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "B0 -> ^pi+ ^pi-"
#
# Options
#
EventSelector().Input   = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bd2Kpi-decprodcut-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                              #GP: (45,1)
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-B2HH.root' TYP='ROOT' OPT='NEW'" ] #GP: (46,1)
HistogramPersistencySvc().OutputFile = "DVHlt2-B2HH.root"

ApplicationMgr().EvtMax = -1 
EventSelector().FirstEvent = 1 
EventSelector().PrintFreq = 10 

# ApplicationMgr().TopAlg += [ "PrintTree" ]
# PrintTree().PhysDesktop.InputLocations = [ "Hlt2SelB2HH" ]

