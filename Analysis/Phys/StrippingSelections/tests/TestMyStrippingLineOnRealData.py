# Test single line of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from StrippingSelections.StrippingBd2DstarMuNuLoose import StrippingBd2DstarMuNuLooseConf
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")
stream.appendLines( [ StrippingBd2DstarMuNuLooseConf().line() ] )
sc = StrippingConf( Streams = [ stream ] )

from StrippingSelections.StartupOptions import veloNZSKiller, redoPV
dvinit = GaudiSequencer("DaVinciInitSeq")
dvinit.Members.insert(0, redoPV() )
dvinit.Members.insert(0, veloNZSKiller() )

DaVinci().PrintFreq = 100
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 10 
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco04-Stripping07-SDSTs.py")
