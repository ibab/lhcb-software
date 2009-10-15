### $ID: $
 #
 #  Test file for Neutralino
 #
 #  @author P. Koppenburg
 #  @date 2009-08-21
###
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, DecayTreeTuple, PrintDecayTree, CheckSelResult
#--------------------------------------------------------------
signal = "Neutralino"
#
# True filter criterion
#
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqTrueSignalTracks").IgnoreFilterPassed = True  
#
# Overwrite input - uncomment to run HLT on True signal only
#
# importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
# Monitoring
#
moni = GaudiSequencer("Hlt2MonitorSeq")
moni.IgnoreFilterPassed = True
moni.Context = "HLT"
importOptions( "$HLTSELCHECKERROOT/options/Hlt2Correlations.py")
importOptions( "$HLTSELCHECKERROOT/options/Hlt2MonitorPlots.py")
importOptions( "$HLTSELCHECKERROOT/options/Hlt2EventTuple.py" )
###
 # Tuple
###
#
#
# Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 1000                        # There are too many
DaVinci().Hlt = True                # Both Hlt levels
DaVinci().Hlt2Requires = 'L0'                  # Ignore Hlt1 in 2
DaVinci().ReplaceL0BanksWithEmulated = False   # Redo L0
DaVinci().DataType = "MC09"                    # MC09 
DaVinci().Simulation = True 
DaVinci().TupleFile =  "HLT-"+signal+".root"
DaVinci().HistogramFile = "DVHlt2-"+signal+".root"
DaVinci().UserAlgorithms = [ ] 
DaVinci().MoniSequence += [ moni ] # , DecayTreeTuple("Hlt2DecayTreeTuple") ] 
DaVinci().Input = [
  "DATAFILE='PFN:castor:/castor/cern.ch/user/n/ngueissa/Susy/TriggerSamples/KaplanNeutralino_mN=48GeV_tN=10ps_MC09.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
