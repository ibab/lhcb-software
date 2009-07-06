### @file DVTestHlt2Bd2MuMuKstar.opts
 #
 #  Test file for HLT Bd->MuMuK* selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, DecayTreeTuple, TupleToolDecay, ReadHltSummary, PrintDecayTree, PrintMCTree, TupleToolTrigger, CheckSelResult, PrintHeader
#--------------------------------------------------------------
signal = "Bd2MuMukstar"
#
# @todo Stripping
#
#
# True filter criterion - will only run HLT on TRUE signal
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "{[[B0]nos => ^K+ ^pi- ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}]cc, [[B0]os => ^K- ^pi+ ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}]cc}"
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
DecayTreeTuple("Hlt2DecayTreeTuple").InputLocations = ["Hlt2Bd2MuMuKstarCombine"]
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
DaVinci().MoniSequence += [ moni, DecayTreeTuple("Hlt2DecayTreeTuple") ]
DaVinci().Input = [
  "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/Bd2KstarMuMu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
