### @file DVTestHlt2Bd2MuMuKstar.opts
 #
 #  Test file for HLT Bd->MuMuK* selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, TupleToolDecay, ReadHltSummary, PrintTree, PrintMCTree, TupleToolTrigger, CheckSelResult, PrintHeader
# from Configurables ChargedProtoPAlg, PreLoadParticles, CombineParticles, FilterDesktop
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$B2DILEPTONROOT/options/DVPreselBd2KstarMuMu.opts")
#
# Run correlations only on offline selected events
#
GaudiSequencer("Hlt2CorrsSeq").Members += [ CheckSelResult("CheckOffline") ]
CheckSelResult("CheckOffline").Algorithms += [ "PreselBd2KstarMuMu" ]
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
EventSelector().PrintFreq = 1 

#
#ApplicationMgr().TopAlg += [ PrintTree("PrintHlt2"), PrintTree("PrintPresel"), PrintMCTree() ]
#
PrintTree("PrintHlt2").addTool(PhysDesktop())
PrintTree("PrintHlt2").PhysDesktop.InputLocations = [ "Phys/Hlt2SelBd2MuMuKstar" ]
PrintTree("PrintPresel").addTool(PhysDesktop())
PrintTree("PrintPresel").PhysDesktop.InputLocations = [ "Phys/PreselBd2KstarMuMu"  ]
PrintMCTree().ParticleNames = [ "B0", "B~0" ]
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
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(TupleToolTrigger())
DecayTreeTuple("Hlt2DecayTreeTuple").TupleToolTrigger.VerboseL0 = True
DecayTreeTuple("Hlt2DecayTreeTuple").ToolList += [ "TupleToolTISTOS" ]
# importOptions( "$FLAVOURTAGGINGOPTS/BTaggingTool.opts")

# HltBackgroundCategory.FillAll = false 

#ReadHltSummary().OutputLevel = 1

#CombineParticles("Hlt2SelBd2MuMuKstar").OutputLevel = 1

#ApplicationMgr().TopAlg += [ FilterDesktop() ]
#FilterDesktop().addTool(PhysDesktop())
#FilterDesktop().PhysDesktop.InputLocations = ["Phys/Hlt2SelBd2MuMuKstar"]
#FilterDesktop().OutputLevel = 1
#FilterDesktop().Code = "ALL"

#from Configurables import CondDBAccessSvc, MagneticFieldSvc, MagFieldToolDC06
#MagneticFieldSvc().Polarity = 2

