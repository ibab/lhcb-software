### @Id: $
 #
 #  Test file for HLT B -> mu mu X selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, DecayTreeTuple, PrintHeader, CheckSelResult
#--------------------------------------------------------------
signal = "Bs2JpsiPhiBiased"
#
# True filter criterion - will only run HLT on TRUE signal
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "[B_s0 -> (phi(1020) -> ^K+ ^K- {,gamma}{,gamma}) (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma})]cc"
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
DecayTreeTuple("Hlt2DecayTreeTuple").InputLocations = ["Hlt2Bs2JpsiPhiSignalCombine"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B_s0 -> (^J/psi(1S) => ^mu+ ^mu-) (^phi(1020) -> ^K+ ^K-)]cc"
#
# Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 1000
DaVinci().HltType = "Hlt1+Hlt2"                # Both Hlt levels
DaVinci().Hlt2Requires = 'L0'                  # Ignore Hlt1 in 2
DaVinci().ReplaceL0BanksWithEmulated = False   # Redo L0
DaVinci().DataType = "DC06" 
DaVinci().Simulation = True 
DaVinci().TupleFile =  "HLT-"+signal+".root"
DaVinci().HistogramFile = "DVHlt2-"+signal+".root"
DaVinci().MoniSequence += [ moni, DecayTreeTuple("Hlt2DecayTreeTuple") ]
DaVinci().Input = [
  "DATAFILE='PFN:castor:/castor/cern.ch/user/g/gcowan/for_HLT_physics_meet/offline_Bs2JpsiPhi_biased_latest.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
