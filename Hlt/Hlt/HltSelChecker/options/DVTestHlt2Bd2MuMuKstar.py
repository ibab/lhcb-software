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
signal = "Bd2MuMukstar"
#
# Preselection
#
importOptions( "$STDOPTS/PreloadUnits.opts")
importOptions( "$B2DILEPTONROOT/options/DoDC06SelBd2KstarMuMu.opts")
dc06sel = GaudiSequencer("SeqDC06SelBd2KstarMuMu")
# DimuonForDC06SelBd2KstarMuMu.OutputLevel = 3
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
###
 # Tuple
###
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Hlt2SelBd2MuMuKstar"]
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
DaVinci().Hlt2IgnoreHlt1Decision = True        # Ignore Hlt1 in 2
DaVinci().ReplaceL0BanksWithEmulated = False   # Redo L0
DaVinci().DataType = "DC06" 
DaVinci().Simulation = True 
DaVinci().TupleFile =  "HLT-"+signal+".root"
DaVinci().HistogramFile = "DVHlt2-"+signal+".root"
DaVinci().UserAlgorithms = [ dc06sel ] 
DaVinci().MoniSequence += [ moni, DecayTreeTuple("Hlt2DecayTreeTuple") ]
DaVinci().Input = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/Bd2MuMuKst-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
