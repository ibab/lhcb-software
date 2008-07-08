### @file 
 #  Test file for HLT statistics
 #
 #  @author P. Koppenburg
 #  @date 2008-06-24
 #
from Gaudi.Configuration import *
from Configurables import Hlt2Statistics, CheckSelResult, EventTuple, TupleToolGeneration, TupleToolTrigger

importOptions("$HLTSELECTIONSROOT/options/DVTestHlt.opts")

# sequencer
statseq = GaudiSequencer("StatisticsSeq")
ApplicationMgr().TopAlg += [ statseq ]

# run only for Hlt2 eaccepted events
filter = CheckSelResult("OnlyHlt2")
filter.Algorithms = [ "Hlt2Decision" ]
# statseq.Members += [ filter ]


# make Hlt2 statistics
statseq.Members += ["Hlt2Statistics" ]

# add event-info tuple
tuple = EventTuple("TriggerTuple")
statseq.Members += [tuple ]
tuple.ToolList = [ "TupleToolTrigger", "TupleToolEventInfo" , "TupleToolGeneration" ]
tuple.addTool( TupleToolTrigger() )
tuple.TupleToolTrigger.VerboseHlt2 = TRUE
#tuple.addTool( TupleToolGeneration() )
#tuple.TupleToolGeneration.OutputLevel = 1
#tuple.OutputLevel = 1


# save tuple
NTupleSvc().Output = ["FILE1 DATAFILE='HltStatsTuple.root' TYP='ROOT' OPT='NEW'"]


# Hlt2Statistics().OutputLevel = 1 ;

# importOptions("$HLTSELECTIONSROOT/options/Bs2JpsiPhiDigi.opts")
# importOptions("$HLTSELECTIONSROOT/options/Bs2PhiPhiDigi.opts")
# importOptions("$HLTSELECTIONSROOT/options/B2DstarX.opts")
importOptions("$HLTSELECTIONSROOT/options/MinimumBiasPFN.opts")
# importOptions("$HLTSELECTIONSROOT/options/MinimumBiasRaw.opts")
# importOptions("$HLTSELECTIONSROOT/options/Bd2MuMuKstarPFN.opts")

ApplicationMgr().EvtMax = 100000

EventSelector().PrintFreq = 100 


