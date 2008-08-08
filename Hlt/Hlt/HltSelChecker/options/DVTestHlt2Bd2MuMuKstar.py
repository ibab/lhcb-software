### @file DVTestHlt2Bd2MuMuKstar.opts
 #
 #  Test file for HLT Bd->MuMuK* selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, TupleToolDecay
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$B2DILEPTONROOT/options/DVPreselBd2KstarMuMu.opts")
HltCorrelations("Hlt2SelectionsCorrs").Algorithms += [ "PreselBd2KstarMuMu" ]
#
# Hlt test
#
importOptions( "$HLTSELECTIONSROOT/options/DVTestHlt2.py")
#
# Plots
#
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
#
# True filter criterion
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "{[[B0]nos => ^K+ ^pi- ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}]cc, [[B0]os => ^K- ^pi+ ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}]cc}" 
#
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = TRUE  
#
# Overwrite input - uncomment to run HLT on TRUE signal only
#
# importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
#
# Options
#
EventSelector().Input   = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/Bd2MuMuKst-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-Bd2MuMuKstar.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-Bd2MuMuKstar.root"

ApplicationMgr().EvtMax = -1 
EventSelector().FirstEvent = 1 
EventSelector().PrintFreq = 10 

# ApplicationMgr().TopAlg += [ "PrintTree", "PrintMCTree" ]
# PrintTree.PhysDesktop.InputLocations = [ "Phys/Hlt2SharedDiMuon" ]
# PrintMCTree.ParticleNames = [ "B0", "B~0" ]
###
 # Tuple
###
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelBd2MuMuKstar"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B0 -> (^J/psi(1S) => ^mu+ ^mu-) (^K*(892)0 -> ^K+ ^pi-)]cc"
# special for head
DecayTreeTuple("Hlt2DecayTreeTuple").Branches = {
  "Head" : "[B0]cc : [B0 -> (J/psi(1S) => mu+ mu-) (K*(892)0 -> K+ pi-)]cc" 
}
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(TupleToolDecay("Head"))
DecayTreeTuple("Hlt2DecayTreeTuple").Head.ToolList += [ "TupleToolP2VV" ]
# importOptions( "$FLAVOURTAGGINGOPTS/BTaggingTool.opts")

# HltBackgroundCategory.FillAll = false 


