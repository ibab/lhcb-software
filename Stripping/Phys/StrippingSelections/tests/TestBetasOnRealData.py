# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf
from StrippingSelections.Utils import lineBuilder

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Betas")

BetaSconfigs = [
        'BetaS',
        'BetaSPsi2S',
        'BetaSPsi2SMuMu',
        'BetaSBs2JpsieePhi',
        'BetaSQ2B',
        'BetaSBs2PhiPhi',
        'BetaSBs2KstKst',
        'BetaSBs2PhiKst',
        'BetaSBs2EtacPhi',
        'BetaSBs2ChicPhi_Chic2KKPiPi',
        'BetaSBs2ChicPhi_Chic2PiPiPiPi',
	'BetaSBs2Q2Body4pi'
        ]
for config in BetaSconfigs:
        stream.appendLines( lineBuilder('strippingdev', config).lines() )

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

# Remove the microbias, nobias and beam gas etc events before doing the tagging step. Remove these lines if inapropriate for your analysis
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NoBias)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )


MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

DaVinci().PrintFreq = 2000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 25000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
#DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType  = "2011"
DaVinci().InputType = "SDST"

importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco09-Stripping13_SDSTs.py")

