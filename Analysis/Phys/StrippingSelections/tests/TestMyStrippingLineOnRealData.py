# Test single line of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from StrippingSelections.StrippingBd2DstarMuNuLoose import StrippingBd2DstarMuNuLooseConf
from StrippingSelections import StrippingBd2DstarMuNu
from StrippingSelections.StrippingB2twobody import B2twobodyLine
from StrippingSelections.StrippingDstarVeryLooseWithD02HH import StrippingDstarVeryLooseWithD02HHConf


from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")
stream.appendLines(  StrippingDstarVeryLooseWithD02HHConf() )
stream.appendLines( B2twobodyLine() )
stream.appendLines( [ StrippingBd2DstarMuNu.line, StrippingBd2DstarMuNuLooseConf().line() ] )
sc = StrippingConf( Streams = [ stream ] )

from StrippingSelections.StartupOptions import veloNZSKiller, redoPV
dvinit = GaudiSequencer("DaVinciInitSeq")
dvinit.Members.insert(0, redoPV() )
dvinit.Members.insert(0, veloNZSKiller() )

# Configure StrippingReport - an algorithm that shows selection statistics
# ChronoAuditor is use by StrippingReport to show the timing. 

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections());

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

DaVinci().PrintFreq = 500
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 100000
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco04-Stripping07-SDSTs.py")
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco05-Stripping08_SDSTs.py")
