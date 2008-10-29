### @file 
 #  Test file for HLT statistics
 #
 #  @author P. Koppenburg
 #  @date 2008-06-24
 #
from Gaudi.Configuration import *
from Configurables import Hlt2Statistics, CheckSelResult
from Configurables import HistogramPersistencySvc, ReadHltSummary
importOptions ("$DAVINCIROOT/options/DaVinciCommon.opts")
########################################################################
#
# Trigger. Uncomment what you need. Hlt1 needs L0, Hlt2 doesn't.
#
from HltConf.Configuration import *
#
## enable if you want to rerun L0
HltConf().replaceL0BanksWithEmulated = True
## both levels
HltConf().Hlt2IgnoreHlt1Decision = True
#
## pick one of 'Hlt1', 'Hlt2', or 'Hlt1+Hlt2'
#HltConf().hltType = 'Hlt1'
#HltConf().hltType = 'Hlt2'
#HltConf().hltType = 'Hlt1+Hlt2'
## don't forget to actually apply the configuration!!!
HltConf().applyConf()
##################################################################
#
# Now require L0, not Hlt1 in Hlt2
#
GaudiSequencer("Hlt2CheckHlt1Passed").Members =[ L0Filter() ] 
##################################################################
#
# to get Hlt2 correlations
#
importOptions( "$HLTSELECTIONSROOT/options/Hlt2Correlations.py")
##################################################################
#
# to get Hlt2 moniror plots
#
#importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
# sequencer
statseq = GaudiSequencer("StatisticsSeq")
ApplicationMgr().TopAlg += [ statseq ]

# run only for Hlt2 eaccepted events
filter = CheckSelResult("OnlyHlt2")
filter.Algorithms = [ "Hlt2Decision" ]
# statseq.Members += [ filter ]

# make Hlt2 statistics
statseq.Members += ["L0Filter", "Hlt2Statistics" ]

# event tuple
importOptions( "$HLTSELCHECKERROOT/options/Hlt2EventTuple.py" )

# save tuple
NTupleSvc().Output = ["FILE1 DATAFILE='Hlt12-StatsTuple.root' TYP='ROOT' OPT='NEW'"]

ApplicationMgr().EvtMax = 100000

EventSelector().PrintFreq = 1000 

