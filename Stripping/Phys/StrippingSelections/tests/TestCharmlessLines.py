# $Id: $
# Test your line(s) of the stripping

#
# for Physics WG CP Gamma/Loop
#
# contact: V. Vagnoni, U. Kerzel
#

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf
from StrippingSettings.Utils import lineBuilderConfiguration

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

#
# Import your stripping lines
#

##
## B->hh (lifetime biased)
##
from StrippingSelections.StrippingHb2Charged2Body import Hb2Charged2BodyLines
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import Hb2Charged2Body
confBhh          = Hb2Charged2BodyLines("Hb2Charged2Body", Hb2Charged2Body['CONFIG'])
stream.appendLines(confBhh.lines())

from StrippingSelections.StrippingB2hhLTUnbiased import StrippingB2hhLTUnbiasedConf
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import B2hhLTUnbiased, B2hhLTUnbiasedDetached
confBhhLTUnbiased = StrippingB2hhLTUnbiasedConf("BhhLTUnbiased",B2hhLTUnbiasedDetached['CONFIG'])
stream.appendLines(confBhhLTUnbiased.lines())

from StrippingSelections.StrippingBu2hhh import Bu2hhhBuilder
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import B2hhh
confB2hhh = Bu2hhhBuilder("Bu2hhh",B2hhh['CONFIG'])
stream.appendLines(confB2hhh.lines())

from StrippingSelections.StrippingB2HHPi0 import StrippingB2HHPi0Conf
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import B2HHPi0
confBHHPI0 = StrippingB2HHPi0Conf("B2HHPI0",B2HHPi0['CONFIG'])
stream.appendLines(confBHHPI0.lines())

from StrippingSelections.StrippingB2KShh import B2KShhConf
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import B2KShh
confB2KShh = B2KShhConf("B2KShh",B2KShh['CONFIG'])
stream.appendLines(confB2KShh.lines())

# Added section for new Lb2V0hhConf line - commented out for now
#from StrippingSelections.StrippingLb2V0hh import Lb2V0hhConf
#from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import Lb2V0hh
#confLb2V0hh = Lb2V0hhConf("Lb2V0hh",Lb2V0hh['CONFIG'])
#stream.appendLines(confLb2V0hh.lines())

from StrippingSelections.StrippingB2Kphiphi import B2KPhiPhiConf
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import B2KPhiPhi
confB2Kphiphi = B2KPhiPhiConf("B2KPhiPhi",B2KPhiPhi['CONFIG'])
stream.appendLines(confB2Kphiphi.lines())

from StrippingSelections.StrippingB2CharmlessQuasi2Body import B2Quasi2BodyConf
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import BetaSQ2B
confB2CharmlessQuasi2Body = B2Quasi2BodyConf("B2Quasi2Body",BetaSQ2B['CONFIG'])
stream.appendLines(confB2CharmlessQuasi2Body.lines())

from StrippingSelections.StrippingBs2PhiKst0 import StrippingBs2PhiKstConf
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import BetaSBs2PhiKst
confBs2PhiKst = StrippingBs2PhiKstConf("Bs2PhiKst",BetaSBs2PhiKst['CONFIG'])
stream.appendLines(confBs2PhiKst.lines())

from StrippingSelections.StrippingBs2Kst0Kst0 import StrippingBs2KstKstConf
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import BetaSBs2KstKst
confBs2KstKst = StrippingBs2KstKstConf("Bs2KstKst",BetaSBs2KstKst['CONFIG'])
stream.appendLines(confBs2KstKst.lines())

from StrippingSelections.StrippingBs2Kst_0Kst_0 import StrippingBs2Kst_0Kst_0Conf
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import BetaSBs2K0stK0st
confBs2Kst_0Kst_0 = StrippingBs2Kst_0Kst_0Conf("Bs2Kst_0Kst_0",BetaSBs2K0stK0st["CONFIG"])
stream.appendLines(confBs2Kst_0Kst_0.lines())

from StrippingSelections.StrippingBs2Q2B import Bs2Q2Body4piConf
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import BetaSBs2Q2Body4pi
confBs2Q2Body = Bs2Q2Body4piConf("Bs2Q2Body4pi",BetaSBs2Q2Body4pi['CONFIG'])
stream.appendLines(confBs2Q2Body.lines())

from StrippingSelections.StrippingBs2PhiPhi import StrippingBs2PhiPhiConf
from StrippingSettings.PreStripping18.LineConfigDictionaries_Charmless import BetaSBs2PhiPhi
confBs2PhiPhi = StrippingBs2PhiPhiConf("Bs2PhiPhi",BetaSBs2PhiPhi['CONFIG'])
stream.appendLines(confBs2PhiPhi.lines())

#
# remaining config
#
from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 50000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )
#sc.OutputType = "ETC"                    # Can be either "ETC" or "DST"

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
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
#CondDB(UseOracle = True)
CondDB().IgnoreHeartBeat = True
DaVinci().DDDBtag  = "head-20110722"
DaVinci().CondDBtag = "head-20110722"

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
