### @file 
 #  Test file for HLT statistics
 #
 #  @author P. Koppenburg
 #  @date 2008-06-24
 #
from Gaudi.Configuration import *
from Configurables import Hlt2Statistics, CheckSelResult, EventTuple, TupleToolGeneration, TupleToolTrigger, L0Filter

importOptions("$HLTSELECTIONSROOT/options/DVTestHlt2.py")

# sequencer
statseq = GaudiSequencer("StatisticsSeq")
ApplicationMgr().TopAlg += [ statseq ]

# run only for Hlt2 eaccepted events
filter = CheckSelResult("OnlyHlt2")
filter.Algorithms = [ "Hlt2Decision" ]
# statseq.Members += [ filter ]


# make Hlt2 statistics
statseq.Members += [L0Filter(), Hlt2Statistics() ]

# add event-info tuple
tuple = EventTuple("TriggerTuple")
statseq.Members += [tuple ]
tuple.ToolList = [ "TupleToolTrigger", "TupleToolEventInfo" , "TupleToolGeneration" ]
tuple.addTool( TupleToolTrigger() )
tuple.TupleToolTrigger.VerboseHlt2 = TRUE
#tuple.TupleToolTrigger.OutputLevel = 1
#tuple.addTool( TupleToolGeneration() )
#tuple.TupleToolGeneration.OutputLevel = 1
#tuple.OutputLevel = 1


# save tuple
NTupleSvc().Output = ["FILE1 DATAFILE='Hlt12StatsTuple.root' TYP='ROOT' OPT='NEW'"]


importOptions("$HLTSELECTIONSROOT/options/MinimumBiasDst.py")

ApplicationMgr().EvtMax = 1000

EventSelector().PrintFreq = 100


