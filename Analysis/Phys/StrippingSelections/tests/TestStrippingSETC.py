#
#   Stripping selections test job
#
#   @author A. Poluektov
#   @date 2009-09-29
#

from Gaudi.Configuration import *
from StrippingConf.Configuration import StrippingConf

from StrippingSelections import StreamBmuon

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# Stripping job configuration
#

sc = StrippingConf(TES = True)
sc.appendStream( StreamBmuon.stream )

from Configurables import EventTuple, TupleToolSelResults

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
tag.addTool(TupleToolSelResults)

tag.TupleToolSelResults.Selections = sc.selections()  # Add the list of stripping selections to TagCreator

from Configurables import DaVinci

DaVinci().appendToMainSequence( [ sc.sequence() ] )   # Append the stripping selection sequence to DaVinci
DaVinci().appendToMainSequence( [ tag ] )             # Append the TagCreator to DaVinci
DaVinci().EvtMax = 1000                        # Number of events
DaVinci().DataType = "2008"
DaVinci().ETCFile = "setc.root"                 # ETC file name

EventSelector().Input   = [
"   DATAFILE='000000.Bmuon.dst' TYP='POOL_ROOTTREE' OPT='READ'"
]
