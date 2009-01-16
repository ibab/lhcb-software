### @file 
 #  Test file for HLT statistics
 #
 #  @author P. Koppenburg
 #  @date 2008-06-24
 #
from Gaudi.Configuration import *
from Configurables import Hlt2Statistics, CheckSelResult
from Configurables import HistogramPersistencySvc, ReadHltSummary
##################################################################
#
# to get Hlt2 monitoring
#
moni = GaudiSequencer("Hlt2MonitorSeq")
moni.IgnoreFilterPassed = True
moni.Context = "HLT"
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Correlations.py")
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
##################################################################
#
# to get Hlt2 moniror plots
#
# sequencer
statseq = GaudiSequencer("StatisticsSeq")
# run only for Hlt2 accepted events
filter = CheckSelResult("OnlyHlt2")
filter.Algorithms = [ "Hlt2Decision" ]
# statseq.Members += [ filter ]

# make Hlt2 statistics
statseq.Members += ["L0Filter", "Hlt2Statistics" ]

# event tuple
importOptions( "$HLTSELCHECKERROOT/options/Hlt2EventTuple.py" )

# save tuple
#
# Configuration
#
from Configurables import DaVinci
DaVinci().EvtMax = 100000
DaVinci().HltType = "Hlt1+Hlt2"                # Both Hlt levels
DaVinci().Hlt2IgnoreHlt1Decision = True        # Ignore Hlt1 in 2
DaVinci().ReplaceL0BanksWithEmulated = False   # Redo L0
DaVinci().DataType = "DC06" 
DaVinci().Simulation = True 
DaVinci().TupleFile =  "Hlt12-StatsTuple.root"
DaVinci().HistogramFile = "DVHlt2stats.root"
DaVinci().MoniSequence += [ moni, statseq ]
