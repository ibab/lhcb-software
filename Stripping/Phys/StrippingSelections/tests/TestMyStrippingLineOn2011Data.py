# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

from StrippingSelections.StrippingB2SameChargeMuon import StrippingB2SameChargeMuonConf as builder
config_params =  {
    'LinePrescale'        : 1.0
    , 'LinePostscale'            : 1.0
}
lb = builder('B2SameChargeMuon',config_params)

stream.appendLines( lb.lines() )

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 50000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )

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
DaVinci().DDDBtag  = "head-20110914"
DaVinci().CondDBtag = "head-20110914"

DaVinci().PrintFreq = 2000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 50000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType  = "2011"
DaVinci().InputType = "SDST"

importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco11aNewRoot_Run97120_SDSTs.py")
