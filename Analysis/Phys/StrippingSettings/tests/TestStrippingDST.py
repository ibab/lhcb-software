from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

from StrippingSelections.Streams import allStreams

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# Stripping job configuration
#

streams = allStreams

from StrippingSelections import StreamMiniBias, StreamV0 

#streams.remove( StreamV0.stream )
#streams.remove( StreamMiniBias.stream )

#streams = [ StreamV0.stream ] 

sc = StrippingConf( Streams = streams )

dstWriter = SelDSTWriter("MyDSTWriter",
	SelectionSequences = sc.activeStreams(),
        OutputPrefix = 'Strip',
	OutputFileSuffix = '000000'
        )

#from StrippingSelections.StartupOptions import veloNZSKiller, redoPV
#dvinit = GaudiSequencer("DaVinciInitSeq")
#dvinit.Members.insert(0, redoPV() )
#dvinit.Members.insert(0, veloNZSKiller() )

DaVinci().EvtMax = 10                         # Number of events
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )

importOptions("$STRIPPINGSELECTIONSROOT/tests/MC09_Bincl.py")     # Data file

