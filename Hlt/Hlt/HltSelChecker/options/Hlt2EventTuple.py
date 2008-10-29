### @file Hlt2EventTuple.opts
 #
 #  Generic Event Tuple configuration
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
tuple.ToolList = [ "TupleToolTrigger", "TupleToolEventInfo" , "TupleToolGeneration" ]
tuple.addTool( TupleToolTrigger() )
tuple.TupleToolTrigger.VerboseHlt1 = TRUE
tuple.TupleToolTrigger.VerboseHlt2 = TRUE
#tuple.addTool( TupleToolGeneration() )
#tuple.TupleToolGeneration.OutputLevel = 1
#tuple.OutputLevel = 1


