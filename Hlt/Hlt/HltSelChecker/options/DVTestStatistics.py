### @file 
 #  Test file for HLT statistics
 #
 #  @author P. Koppenburg
 #  @date 2008-06-24
 #
from Gaudi.Configuration import *
##################################################################
#
# to get Hlt2 monitoring
#
from Configurables import GaudiSequencer
moni = GaudiSequencer("Hlt2MonitorSeq")
moni.IgnoreFilterPassed = True
moni.Context = "HLT"
#importOptions( "$HLTSELECTIONSROOT/options/Hlt2Correlations.py")
#importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
##################################################################
#
# to get Hlt2 moniror plots
#
# sequencer
statseq = GaudiSequencer("StatisticsSeq")

#from Configurables import L0Filter, Hlt2Statistics
# make Hlt2 statistics
#statseq.Members += [ L0Filter(), Hlt2Statistics() ]

# event tuple
#importOptions( "$HLTSELCHECKERROOT/options/Hlt2EventTuple.py" )
###
 # Tuple
###
#from Configurables import PhysDesktop, DecayTreeTuple
#importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
#DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop)
#DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Hlt2SelBu2LLKSignal"]
#DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[B+ -> (^J/psi(1S) => ^e+ ^e-) ^K+]cc"
# save tuple
#
# Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 10000
DaVinci().HltType = "Hlt1+Hlt2"                # Both Hlt levels
DaVinci().Hlt2Requires = 'L0'                  # Ignore Hlt1 in 2
DaVinci().ReplaceL0BanksWithEmulated = False   # Redo L0
# DaVinci().DataType = "DC06" 
DaVinci().Simulation = True 
DaVinci().TupleFile =  "Hlt12-StatsTuple.root"
# DaVinci().HistogramFile = "DVHlt2stats.root"
DaVinci().MoniSequence += [ moni, statseq ]

def noInit():
    GaudiSequencer("PhysInitSeq").Members = []
    GaudiSequencer("AnalysisInitSeq").Members = []

appendPostConfigAction(noInit)

