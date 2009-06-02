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
###
 # Tuple
###
from Configurables import DecayTreeTuple
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").InputLocations = ["Hlt2Bu2MuMuKSignalCombine"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B+ -> (^J/psi(1S) => ^mu+ ^mu-) ^K+]cc"
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
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Bu2Kmumu-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
