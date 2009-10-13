from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

from StrippingSelections import StreamBmuon, StreamHadron, StreamJpsi, StreamDstar, StreamLambda, StreamBelectron

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

#
# Stripping job configuration
#

sc = StrippingConf()
sc.appendStream( StreamBmuon.stream )
sc.appendStream( StreamHadron.stream )
sc.appendStream( StreamJpsi.stream )
sc.appendStream( StreamDstar.stream )
sc.appendStream( StreamLambda.stream )
#sc.appendStream( StreamBelectron.stream )

dstWriter = SelDSTWriter("MyDSTWriter",
	SelectionSequences = sc.activeStreams(),
        OutputPrefix = 'Strip',
	OutputFileSuffix = '000000'
        )

DaVinci().EvtMax = 200                         # Number of events
DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().UserAlgorithms = [ dstWriter.sequence() ]

importOptions("$STRIPPINGSELECTIONSROOT/tests/2008-InclBJpsiMuMu.py")
