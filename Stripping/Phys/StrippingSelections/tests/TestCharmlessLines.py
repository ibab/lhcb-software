from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

#
# Import your stripping lines
#

##
## B->hh (lifetime biased)
##
from StrippingSelections.StrippingB2TwoBaryons import B2TwoBaryonLines
from StrippingSelections.StrippingB2TwoBaryons import default_config as B2PPbar
confB2PPbar          = B2TwoBaryonLines("B2TwoBaryons", B2PPbar)
stream.appendLines(confB2PPbar.lines())

from StrippingSelections.StrippingBu2KSh import Bu2KShConf
from StrippingSelections.StrippingBu2KSh import default_config as B2KSh
confB2KSh = Bu2KShConf("B2KSh",B2KSh)
stream.appendLines(confB2KSh.lines())

from StrippingSelections.StrippingHb2Charged2Body import Hb2Charged2BodyLines
from StrippingSelections.StrippingHb2Charged2Body import default_config as Hb2Charged2Body
confBhh          = Hb2Charged2BodyLines("Hb2Charged2Body", Hb2Charged2Body)
stream.appendLines(confBhh.lines())

from StrippingSelections.StrippingB2HHBkg import B2HHBkgLines
from StrippingSelections.StrippingB2HHBkg import default_config as B2HHBkg
confBhhBkg          = B2HHBkgLines("B2HHBkg", B2HHBkg)
stream.appendLines(confBhhBkg.lines())

from StrippingSelections.StrippingB2hhLTUnbiased import StrippingB2hhLTUnbiasedConf
from StrippingSettings.Stripping18.LineConfigDictionaries_Charmless import B2hhLTUnbiased, B2hhLTUnbiasedDetached
confBhhLTUnbiased = StrippingB2hhLTUnbiasedConf("BhhLTUnbiased",B2hhLTUnbiased['CONFIG'])
confBhhLTUnbiasedDetached = StrippingB2hhLTUnbiasedConf("BhhLTUnbiasedDetached",B2hhLTUnbiasedDetached['CONFIG'])
stream.appendLines(confBhhLTUnbiased.lines())
stream.appendLines(confBhhLTUnbiasedDetached.lines())

from StrippingSelections.StrippingBu2hhh import Bu2hhhBuilder
from StrippingSelections.StrippingBu2hhh import config_params as B2hhh
confB2hhh = Bu2hhhBuilder("Bu2hhh",B2hhh)
stream.appendLines(confB2hhh.lines())

from StrippingSelections.StrippingD02KPiPi0 import StrippingD02KPiPi0Conf
from StrippingSettings.Stripping19a.LineConfigDictionaries_Calib import D02KPiPi0
confD02KPiPi0 = StrippingD02KPiPi0Conf(D02KPiPi0['BUILDERTYPE'],D02KPiPi0['CONFIG'])
stream.appendLines(confD02KPiPi0.lines())

from StrippingSelections.StrippingB2HHPi0 import StrippingB2HHPi0Conf
from StrippingSettings.Stripping18.LineConfigDictionaries_Charmless import B2HHPi0
confBHHPI0 = StrippingB2HHPi0Conf("B2HHPI0",B2HHPi0['CONFIG'])
stream.appendLines(confBHHPI0.lines())

from StrippingSelections.StrippingB2KShh import B2KShhConf
from StrippingSelections.StrippingB2KShh import default_config as B2KShh
confB2KShh = B2KShhConf("B2KShh",B2KShh)
stream.appendLines(confB2KShh.lines())

# Added section for new Lb2V0hhConf line - commented out for now
from StrippingSelections.StrippingLb2V0hh import Lb2V0hhConf
from StrippingSelections.StrippingLb2V0hh import default_config as Lb2V0hh
confLb2V0hh = Lb2V0hhConf("Lb2V0hh",Lb2V0hh)
stream.appendLines(confLb2V0hh.lines())

from StrippingSelections.StrippingB2Kphiphi import B2KPhiPhiConf
from StrippingSelections.StrippingB2Kphiphi import config_params as B2KPhiPhi
confB2Kphiphi = B2KPhiPhiConf("B2KPhiPhi",B2KPhiPhi)
stream.appendLines(confB2Kphiphi.lines())

from StrippingSelections.StrippingB2CharmlessQuasi2Body import B2Quasi2BodyConf
from StrippingSelections.StrippingB2CharmlessQuasi2Body import config_params as BetaSQ2B
confB2CharmlessQuasi2Body = B2Quasi2BodyConf("B2Quasi2Body",BetaSQ2B)
stream.appendLines(confB2CharmlessQuasi2Body.lines())

from StrippingSelections.StrippingBs2PhiKst0 import StrippingBs2PhiKstConf
from StrippingSettings.Stripping19a.LineConfigDictionaries_Charmless import BetaSBs2PhiKst
confBs2PhiKst = StrippingBs2PhiKstConf("Bs2PhiKst",BetaSBs2PhiKst['CONFIG'])
stream.appendLines(confBs2PhiKst.lines())

from StrippingSelections.StrippingBs2Kst0Kst0 import StrippingBs2KstKstConf
from StrippingSelections.StrippingBs2Kst0Kst0 import __config_default__ as BetaSBs2KstKst
confBs2KstKst = StrippingBs2KstKstConf("Bs2KstKst",BetaSBs2KstKst)
stream.appendLines(confBs2KstKst.lines())

from StrippingSelections.StrippingBs2Kst_0Kst_0 import StrippingBs2Kst_0Kst_0Conf
from StrippingSelections.StrippingBs2Kst_0Kst_0 import __config_default__ as BetaSBs2K0stK0st
confBs2Kst_0Kst_0 = StrippingBs2Kst_0Kst_0Conf("Bs2Kst_0Kst_0",BetaSBs2K0stK0st)
stream.appendLines(confBs2Kst_0Kst_0.lines())

from StrippingSelections.StrippingBs2Q2B import Bs2Q2Body4piConf
from StrippingSelections.StrippingBs2Q2B import config_params as BetaSBs2Q2Body4pi
confBs2Q2Body = Bs2Q2Body4piConf("Bs2Q2Body4pi",BetaSBs2Q2Body4pi)
stream.appendLines(confBs2Q2Body.lines())

from StrippingSelections.StrippingBs2KKhh import BsPhiRhoConf
from StrippingSelections.StrippingBs2KKhh import config_params as BsPhiRho
confBsPhiRho = BsPhiRhoConf("BsPhiRho",BsPhiRho)
stream.appendLines(confBsPhiRho.lines())

from StrippingSelections.StrippingB2FourKHigh import Bs2Q2Body4KConf
from StrippingSelections.StrippingB2FourKHigh import config_params as Bs2Q2Body4K
confBs2Q2Body4K = Bs2Q2Body4KConf("Bs2Q2Body4K",Bs2Q2Body4K)
stream.appendLines(confBs2Q2Body4K.lines())

from StrippingSelections.StrippingBs2PhiPhi import StrippingBs2PhiPhiConf
from StrippingSelections.StrippingBs2PhiPhi import config_params as BetaSBs2PhiPhi
confBs2PhiPhi = StrippingBs2PhiPhiConf("Bs2PhiPhi",BetaSBs2PhiPhi)
stream.appendLines(confBs2PhiPhi.lines())

from StrippingSelections.StrippingBs2KSKS import Bs2KSKSConf
from StrippingSelections.StrippingBs2KSKS import default_config as Bs2KSKS
confBs2KSKS = Bs2KSKSConf("Bs2KSKS",Bs2KSKS)
stream.appendLines(confBs2KSKS.lines())

from StrippingSelections.StrippingB2XEta import B2XEtaConf
from StrippingSelections.StrippingB2XEta import default_config as B2XEta
confB2XEta = B2XEtaConf("B2XEta",B2XEta)
stream.appendLines(confB2XEta.lines())

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents,
                    TESPrefix = 'Strip'
                    )

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 100000
DaVinci().PrintFreq = 2000
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType  = "2012"
DaVinci().InputType = "DST"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# database
DaVinci().DDDBtag  = "head-20120413"
DaVinci().CondDBtag = "cond-20120710"

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco13c_Run124134.py")

