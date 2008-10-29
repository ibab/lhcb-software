### @file DVTestHlt2Bd2JpsiMuMuKs.opts
 #
 #  Test file for HLT Bd->Jpsi(MuMu)Ks selection
 #
 #  @author S. Amato, C. Gobel, F. Rodrigues
 #  @date 2008-10-09
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple, TupleToolDecay, CheckSelResult, PrintHeader
#--------------------------------------------------------------
#
# Preselection
#
importOptions( "$CCBARROOT/options/DVDC06SelBd2JpsiKS_Jpsi2MuMu_lifetime_unbiased.opts")
PrintHeader("PrintDC06selBd2Jpsi2MuMuKs").OutputLevel = 4
#
# Run correlations only on offline accepted events
#
GaudiSequencer("Hlt2CorrsSeq").Members += [ CheckSelResult("CheckDC06selBd2Jpsi2MuMuKs") ]
CheckSelResult("CheckDC06selBd2Jpsi2MuMuKs").Algorithms += [ "DC06selBd2Jpsi2MuMuKs" ]
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
FilterTrueTracks().MCDecayFinder.Decay =  "{[[B0]os -> (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) (KS0 -> ^pi+ ^pi-)]cc,[[B0]nos -> (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) (KS0 -> ^pi+ ^pi-)]cc}"

# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = TRUE  
#
# Overwrite input - uncomment to run HLT on TRUE signal only
#
#importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
#
# Option
#
#importOptions("$HLTSELECTIONSROOT/options/Bd2JpsiMuMuKsDst.py")

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-Bd2JpsiMuMuKsDDUnbiased.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-Bd2JpsiMuMuKsDDUnbiased.root"

ApplicationMgr().EvtMax = 5000 
EventSelector().FirstEvent = 1 
EventSelector().PrintFreq = 500 

# ApplicationMgr().TopAlg += [ "PrintTree", "PrintMCTree" ]
# PrintTree.PhysDesktop.InputLocations = [ "Phys/Hlt2SharedDiMuon" ]
# PrintMCTree.ParticleNames = [ "B0", "B~0" ]
###
 # Tuple
###
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelBd2JpsiMuMuKsDDUnbiased"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B0 -> (^KS0 => ^pi+ ^pi-) (^J/psi(1S) => ^mu+ ^mu-)]cc"
# special for head
DecayTreeTuple("Hlt2DecayTreeTuple").Branches = {
"kshort"  : "[B0 -> (^KS0 =>  pi+  pi-) ( J/psi(1S) =>  mu+  mu-)]cc"
,"jpsi"   : "[B0 -> ( KS0 =>  pi+  pi-) (^J/psi(1S) =>  mu+  mu-)]cc"
,"muon"   : "[B0 -> ( KS0 =>  pi+  pi-) ( J/psi(1S) => ^mu+ ^mu-)]cc"
,"pion"   : "[B0 -> ( KS0 => ^pi+ ^pi-) ( J/psi(1S) =>  mu+  mu-)]cc"
,"B0"     : "[B0]cc : [B0 -> ( KS0 =>  pi+  pi-) ( J/psi(1S) =>  mu+  mu-)]cc"
 
}
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(TupleToolDecay("Head"))
DecayTreeTuple("Hlt2DecayTreeTuple").Head.ToolList += [ "TupleToolP2VV"
, "TupleToolTrigger"
, "TupleToolMCTruth"
, "TupleToolMCBackgroundInfo"
, "TupleToolGeometry"
, "TupleToolKinematic"
, "TupleToolPropertime"
, "TupleToolPrimaries"
, "TupleToolEventInfo"
, "TupleToolMCHierarchy"
, "TupleToolPid"
, "TupleToolTrackInfo"
, "TupleToolVtxIsoln"
]
# importOptions( "$FLAVOURTAGGINGOPTS/BTaggingTool.opts")

#
# Data Option
#
importOptions("$HLTSELCHECKERROOT/options/Bd2JpsiKs.py")
