#
#   Stripping selections test job
#
#   @author A. Poluektov
#   @date 2009-09-29
#

from Gaudi.Configuration import *

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# Stripping job configuration
#

from StrippingSelections.Streams import allStreams
from StrippingConf.Configuration import StrippingConf

streams = allStreams

from StrippingSelections import StreamMiniBias, StreamV0 

#streams.remove( StreamV0.stream )
#streams.remove( StreamMiniBias.stream )

#streams = [ StreamMiniBias.stream, StreamV0.stream ] 

from StrippingSettings.Default import Default

sc = StrippingConf( Streams = streams, Settings = Default() )

print sc

from Configurables import EventTuple, TupleToolSelResults

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
tag.addTool(TupleToolSelResults)

tag.TupleToolSelResults.Selections = sc.selections()  # Add the list of stripping selections to TagCreator

from Configurables import DaVinci

#from StrippingSelections.StartupOptions import veloNZSKiller, redoPV
#dvinit = GaudiSequencer("DaVinciInitSeq")
#dvinit.Members.insert(0, redoPV() )
#dvinit.Members.insert(0, veloNZSKiller() )

DaVinci().appendToMainSequence( [ sc.sequence() ] )   # Append the stripping selection sequence to DaVinci
DaVinci().appendToMainSequence( [ tag ] )             # Append the TagCreator to DaVinci
DaVinci().EvtMax = 10                        # Number of events
DaVinci().ETCFile = "etc.root"                 # ETC file name
DaVinci().HistogramFile = "hist.root"

importOptions("$STRIPPINGSELECTIONSROOT/tests/MC09_Bincl.py")     # Data file
