# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from StrippingSelections.StrippingBd2DstarMuNuLoose import StrippingBd2DstarMuNuLooseConf
from StrippingSelections import StrippingBd2DstarMuNu
from StrippingSelections.StrippingDstarVeryLooseWithD02HH import StrippingDstarVeryLooseWithD02HHConf

from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")
stream.appendLines( [ StrippingDstarVeryLooseWithD02HHConf().line()] )
stream.appendLines( [ StrippingBd2DstarMuNu.line, StrippingBd2DstarMuNuLooseConf().line() ] )
sc = StrippingConf( Streams = [ stream ] )

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
DaVinci().EvtMax = 100
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_79646_RealData+Reco06-Stripping10_90000000_SDST.py")
#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_79647_RealData+Reco06-Stripping10_90000000_SDST.py")
