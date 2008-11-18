### @file Hlt2EventTuple.opts
 #
 #  Generic Event Tuple configuration
 #
 #  For some unknown reason one can't have it at the same time as the DecayTreeTuple...
 #
 #  See DecayTreeTuple documentation
 #
 #  @author P. Koppenburg
 #  @date 2008-03-07
#/
from Gaudi.Configuration import *
from Configurables import EventTuple, TupleToolGeneration, TupleToolTrigger, L0Filter
#

# add event-info tuple
tuple = EventTuple("TriggerTuple")
ApplicationMgr().TopAlg += [tuple ]
tuple.ToolList = [ "TupleToolEventInfo", "TupleToolGeneration", "TupleToolTrigger"  ]
tuple.addTool( TupleToolTrigger() )
tuple.TupleToolTrigger.VerboseHlt1 = True
tuple.TupleToolTrigger.VerboseHlt2 = True
#tuple.addTool( TupleToolGeneration() )
#tuple.TupleToolGeneration.OutputLevel = 1
#tuple.OutputLevel = 1


