from Gaudi.Configuration import *

from CommonParticles.Utils import DefaultTrackingCuts

from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

from StrippingSelections.Streams import allStreams

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# Stripping job configuration
#

sc = StrippingConf( Streams = allStreams )

dstWriter = SelDSTWriter("MyDSTWriter",
	SelectionSequences = sc.activeStreams(),
        OutputPrefix = 'Strip',
	OutputFileSuffix = 'MagUp'
        )

DaVinci().EvtMax = 10                        # Number of events
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )


