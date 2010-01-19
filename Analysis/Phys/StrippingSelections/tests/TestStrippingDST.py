from Gaudi.Configuration import *
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
	OutputFileSuffix = '000000'
        )

from StrippingSelections.StartupOptions import veloNZSKiller, redoPV
dvinit = GaudiSequencer("DaVinciInitSeq")
dvinit.Members.insert(0, redoPV() )
dvinit.Members.insert(0, veloNZSKiller() )

DaVinci().EvtMax = 10                         # Number of events
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ dstWriter.sequence() ] )

#importOptions("$STRIPPINGSELECTIONSROOT/tests/2008-InclBJpsiMuMu.py")
importOptions("$STRIPPINGSELECTIONSROOT/tests/MC09_Bincl.py")     # Data file

