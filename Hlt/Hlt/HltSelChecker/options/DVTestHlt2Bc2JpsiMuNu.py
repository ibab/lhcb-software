### @Id: $
 #
 #  Test file for HLT Bc->J/psiMuNu selection
 #
 #  @author P. Koppenburg
 #  @date 2009-07-21
###
from Gaudi.Configuration import *
from Configurables import CheckSelResult, L0Filter, AlgorithmCorrelationsAlg, PrintHeader
#--------------------------------------------------------------
signal = "Bc2JpsiMuNu"
#
# True filter criterion
#
from Configurables import FilterTrueTracks, MCDecayFinder
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "[B_c+ -> (J/psi(1S) -> ^mu+ ^mu- {,gamma} {,gamma}) ^mu+ nu_mu {,gamma} {,gamma}]cc"
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
importOptions( "$HLTSELCHECKERROOT/options/Hlt2EventTuple.py" )
###
 # Tuple
###
from Configurables import DecayTreeTuple
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").InputLocations = ["Hlt2UnbiasedJPsiFilter"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "J/psi(1S) => ^mu+ ^mu-"
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
  "DATAFILE='PFN:castor:/castor/cern.ch/user/j/jhe/Bc2JpsiMuNu/offline_Bc2JpsiMuNu.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
