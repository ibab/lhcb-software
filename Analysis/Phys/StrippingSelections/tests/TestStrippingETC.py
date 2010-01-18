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

from StrippingSelections.StrippingPass import StrippingPassConf

StrippingPassConf().PassPrescale = 0.1
StrippingPassConf().PhysPrescale = 0.1

from StrippingSelections.Streams import allStreams
from StrippingConf.Configuration import StrippingConf

sc = StrippingConf( Streams = allStreams )

print sc

from Configurables import EventTuple, TupleToolSelResults

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolSelResults"  ]
tag.addTool(TupleToolSelResults)

tag.TupleToolSelResults.Selections = sc.selections()  # Add the list of stripping selections to TagCreator

from Configurables import DaVinci

from StrippingSelections.StartupOptions import veloNZSKiller, redoPV
dvinit = GaudiSequencer("DaVinciInitSeq")
dvinit.Members.insert(0, redoPV() )
dvinit.Members.insert(0, veloNZSKiller() )

#DaVinci().appendToMainSequence( [ removePV(), redoPV(), veloNZSKiller() ] )
DaVinci().appendToMainSequence( [ sc.sequence() ] )   # Append the stripping selection sequence to DaVinci
DaVinci().appendToMainSequence( [ tag ] )             # Append the TagCreator to DaVinci
DaVinci().EvtMax = 100                        # Number of events
DaVinci().ETCFile = "etc.root"                 # ETC file name

#importOptions("$STRIPPINGSELECTIONSROOT/tests/2008-InclBJpsiMuMu.py")     # Data file
importOptions("$STRIPPINGSELECTIONSROOT/tests/MC09_Bincl.py")     # Data file

