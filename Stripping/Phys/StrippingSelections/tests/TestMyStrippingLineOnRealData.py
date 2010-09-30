# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Import your stripping lines
from StrippingSelections.StrippingBd2DstarMuNuLoose import StrippingBd2DstarMuNuLooseConf
from StrippingSelections import StrippingBd2DstarMuNu
from StrippingSelections.StrippingDstarVeryLooseWithD02HH import StrippingDstarVeryLooseWithD02HHConf

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")
stream.appendLines( [ StrippingDstarVeryLooseWithD02HHConf().line()] )
stream.appendLines( [ StrippingBd2DstarMuNu.line, StrippingBd2DstarMuNuLooseConf().line() ] )

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )
sc.OutputType = "ETC"                    # Can be either "ETC" or "DST"

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Configure the ETC writing step
from Configurables import EventTuple, TupleToolSelResults
from Configurables import TupleToolStripping

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolStripping"  ]
tag.addTool( TupleToolStripping )
tag.TupleToolStripping.StrippigReportsLocations = "/Event/Strip/Phys/DecReports"

# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )

seq = GaudiSequencer("TagSeq")
seq.Members = [tag]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

DaVinci().PrintFreq = 500
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().ETCFile = "etc.root"
DaVinci().EvtMax = 100
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().MoniSequence += [ seq ]                     # Append the TagCreator to DaVinci
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_79646_RealData+Reco06-Stripping10_90000000_SDST.py")

