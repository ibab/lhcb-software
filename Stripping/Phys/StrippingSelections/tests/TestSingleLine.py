#
#   Stripping selections test job
#
#   @author A. Poluektov
#   @date 2009-09-29
#

from Gaudi.Configuration import *

MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"

#
# Stripping job configuration
#

from StrippingConf.StrippingStream import StrippingStream
from StrippingSelections import StrippingBu2hhh

stream = StrippingStream("Test")
stream.appendLines( [ StrippingBu2hhh.line ] ) 

from StrippingConf.Configuration import StrippingConf

sc = StrippingConf( Streams = [ stream ] )

print sc

# Configure ETC output

from Configurables import EventTuple, TupleToolSelResults

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
tag.addTool(TupleToolSelResults)

tag.TupleToolSelResults.Selections = sc.selections()  # Add the list of stripping selections to TagCreator

# Configure StrippingReport - an algorithm that shows selection statistics

from Configurables import StrippingReport

sr = StrippingReport(Selections = sc.selections());

# Configure DaVinci

from Configurables import DaVinci

DaVinci().appendToMainSequence( [ sc.sequence() ] )   # Append the stripping selection sequence to DaVinci
DaVinci().appendToMainSequence( [ tag ] )             # Append the TagCreator to DaVinci
DaVinci().appendToMainSequence( [ sr ] )              # Append the StrippingReport algoritm to DaVinci

DaVinci().EvtMax = 100                         # Number of events

DaVinci().ETCFile = "etc.root"                 # ETC file name
DaVinci().HistogramFile = "hist.root"
DaVinci().DataType = 'MC09'
