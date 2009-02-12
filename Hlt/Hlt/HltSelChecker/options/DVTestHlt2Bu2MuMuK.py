### @Id: $
 #
 #  Test file for HLT Bu->LLK* selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-23
###
from Gaudi.Configuration import *
from Configurables import CheckSelResult, L0Filter, AlgorithmCorrelationsAlg, PrintHeader
#--------------------------------------------------------------
signal = "Bu2MuMuK"
#
# Preselection
#
importOptions( "$STDOPTS/PreloadUnits.opts")
importOptions( "$B2DILEPTONROOT/options/DoDC06SelBu2MuMuK.py")
GaudiSequencer("Hlt2CorrsSeq").Members += [ CheckSelResult("CheckOffline") ]
CheckSelResult("CheckOffline").Algorithms += [ "PreselBu2LLK" ]
PrintHeader("PrintDiLeptonForPreselBu2LLK").OutputLevel = 4
PrintHeader("PrintPreselBu2LLK").OutputLevel = 4
presel = GaudiSequencer("SeqPreselBu2LLK")
#
# True filter criterion
#
from Configurables import FilterTrueTracks, MCDecayFinder
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "{[B+ -> ^K+ ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}{,gamma}]cc, [B+ -> ^K+ ^mu+ ^mu- {,gamma}{,gamma}{,gamma}{,gamma}{,gamma}]cc}"
#
from Configurables import GaudiSequencer
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
#
# Some more correlations
#
from Configurables import GaudiSequencer, CheckSelResult
corrs = GaudiSequencer("Corrs") 
corrs.Members += [ CheckSelResult("CheckOffline"), L0Filter(), AlgorithmCorrelationsAlg("MuMuK") ]
CheckSelResult("CheckOffline").Algorithms = [ "DC06SelBu2MuMuK" ]
AlgorithmCorrelationsAlg("MuMuK").Algorithms = [ "DC06SelBu2MuMuK", "Hlt2SelBu2LLK", "Hlt2SelBu2LLKSignal", "Hlt2SelBu2LLKHighMass", "Hlt2SelBu2LLKJpsi", "Hlt2SelBiasedDiElectron", "Hlt2SelBiasedDiMuon", "Hlt2SelUnbiasedDiMuon", "Hlt2Decision" ]
###
 # Tuple
###
from Configurables import PhysDesktop, DecayTreeTuple
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Hlt2SelBu2LLKSignal"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B+ -> (^J/psi(1S) => ^mu+ ^mu-) ^K+]cc"
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
DaVinci().UserAlgorithms = [ presel ] 
DaVinci().MoniSequence += [ moni, corrs, DecayTreeTuple("Hlt2DecayTreeTuple") ]
DaVinci().Input = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bu2Kmumu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
