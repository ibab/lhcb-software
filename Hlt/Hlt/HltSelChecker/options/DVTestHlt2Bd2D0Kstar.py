### @file DVTestHlt2Bd2D0Kstar.opts
 #
 #  Test file for HLT Bd->D0(Kpi)K*(Kpi) selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
 #/
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, PrintHeader, CheckSelResult
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$B2D0XROOT/options/DVPreselBd2D0Kst_D02HH.opts")
PrintHeader("PrintPreselBd2D0Kst_D02HH").OutputLevel = 4 
#
# Run correlations only on offline selected events
#
GaudiSequencer("Hlt2CorrsSeq").Members += [ CheckSelResult("CheckOffline") ]
CheckSelResult("CheckOffline").Algorithms += [ "PreselBd2D0Kst_D02HH" ]
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
FilterTrueTracks().MCDecayFinder.Decay =  "{[[B0]nos -> (D~0 -> ^K+ ^pi- {,gamma}{,gamma}) (K*(892)0 -> ^K+ ^pi- {,gamma}{,gamma})]cc, [[B0]os -> (D0 -> ^K- ^pi+ {,gamma}{,gamma}) (K*(892)~0 -> ^K- ^pi+ {,gamma}{,gamma})]cc, [[B0]nos -> (D~0 -> ^K+ ^K- {,gamma}{,gamma}) (K*(892)0 -> ^K+ ^pi- {,gamma}{,gamma})]cc, [[B0]os -> (D0 -> ^K- ^K+ {,gamma}{,gamma}) (K*(892)~0 -> ^K- ^pi+ {,gamma}{,gamma})]cc, [[B0]nos -> (D~0 -> ^pi+ ^pi- {,gamma}{,gamma}) (K*(892)0 -> ^K+ ^pi- {,gamma}{,gamma})]cc, [[B0]os -> (D0 -> ^pi- ^pi+ {,gamma}{,gamma}) (K*(892)~0 -> ^K- ^pi+ {,gamma}{,gamma})]cc}"
#
# Overwrite input - uncomment to run HLT on TRUE signal only
#
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = TRUE  
#
# importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
###
 # Tuple
 #/
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Hlt2SelBd2D0Kstar"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B0 -> (^D0 -> ^K- ^pi+) (K*(892)0 -> ^K+ ^pi-)]cc"
#
# Options
#
EventSelector().Input   = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bd2D0Kst-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-Bd2D0Kstar.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-Bd2D0Kstar.root"

ApplicationMgr().EvtMax = -1 
#EventSelector().FirstEvent = 815 
EventSelector().PrintFreq = 10 
