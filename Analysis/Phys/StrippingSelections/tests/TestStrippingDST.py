from Gaudi.Configuration import *
from Configurables import SelDSTWriter, StrippingConf, DaVinci

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

sc = StrippingConf()
sc.ActiveLines = [ ]
sc.ActiveStreams = [ ]
sc.OutputType = "DST"

dstWriter = SelDSTWriter("MyDSTWriter",
	SelectionSequences = sc.activeStreams(),
        OutputPrefix = 'Strip',
	OutputFileSuffix = '000000'
        )

DaVinci().EvtMax = 1000                         # Number of events
DaVinci().DataType = "2008"                    # Default is "DC06"
DaVinci().UserAlgorithms = [ dstWriter.sequence() ]

importOptions("$STRIPPINGSELECTIONSROOT/tests/2008-InclBJpsiMuMu.py")
