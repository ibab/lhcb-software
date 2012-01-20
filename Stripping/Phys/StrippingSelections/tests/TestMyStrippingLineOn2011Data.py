# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")
#
#from StrippingSelections.StrippingBuToKX3872 import StrippingBu2KX3872Conf as dummy_builder
#from StrippingSelections.StrippingBuToKX3872 import Stripping_BuToKX3872_TestDictonary
#
#from StrippingSelections.StrippingZ02TauTauProng import Z02TauTauProngConf as builder
#from StrippingSelections.StrippingZ02TauTauProng import config_params
#
#lb  = builder('Z02TauTauProng',config_params)
#dlb = dummy_builder('Bu2KX3872',Stripping_BuToKX3872_TestDictonary)
#stream.appendLines( dlb.lines() )
#stream.appendLines( lb.lines() )


from StrippingSelections.StrippingDiMuonNew import DiMuonConf as dummy_builder
from StrippingSelections.StrippingDiMuonNew import config_default

from StrippingSelections.StrippingLowMassHiggs import A1MuMuConf as builder
from StrippingSelections.StrippingLowMassHiggs import config_params

lb  = builder('A1MuMu',config_params)
dlb = dummy_builder('DiMuon',config_default)
stream.appendLines( dlb.lines() )
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

importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco12_Run97120_SDST.py")
