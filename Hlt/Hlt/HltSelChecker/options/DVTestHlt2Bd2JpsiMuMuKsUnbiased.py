### $Id :$
 #
 #  Test file for HLT Bd->Jpsi(MuMu)Ks selection
 #
 #  @author S. Amato, C. Gobel, F. Rodrigues
 #  @date 2008-10-09
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, DecayTreeTuple, TupleToolDecay, CheckSelResult, PrintHeader
#--------------------------------------------------------------
signal = "Bd2JpsiMuMuKsBiased"
#
# True filter criterion - will only run HLT on TRUE signal
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "{[[B0]os -> (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) (KS0 -> ^pi+ ^pi-)]cc,[[B0]nos -> (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) (KS0 -> ^pi+ ^pi-)]cc}"

#
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = True
#
# Overwrite input - uncomment to run HLT on TRUE signal only
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
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").InputLocations = ["Hlt2Bd2JpsiMuMuKsLLUnbiasedCombine", "Hlt2Bd2JpsiMuMuKsDDUnbiasedCombine"]
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
#
# Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 2000
DaVinci().HltType = "Hlt1+Hlt2"                # Both Hlt levels
DaVinci().Hlt2Requires = 'L0'                  # Ignore Hlt1 in 2
DaVinci().ReplaceL0BanksWithEmulated = False   # Redo L0
DaVinci().DataType = "DC06" 
DaVinci().Simulation = True 
DaVinci().TupleFile =  "HLT-"+signal+".root"
DaVinci().HistogramFile = "DVHlt2-"+signal+".root"
DaVinci().MoniSequence += [ moni, DecayTreeTuple("Hlt2DecayTreeTuple") ]
DaVinci().Input = [
  "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/Bd2Jpsi2MuMuKS-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
