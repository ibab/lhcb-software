from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ]} 
#                                "GhostProbCut" : [0, 0.35 ] }

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
streamBhadron = StrippingStream("StreamBhadron")
streamBhadronCompleteEvent = StrippingStream("StreamBhadronCompleteEvent")

bhadronLines = []
bhadroncompleteeventLines = []

#
# Import your stripping lines
#
from StrippingSelections.StrippingBu2KSh import Bu2KShConf
#from StrippingSelections.StrippingBu2KSh import default_config as B2KSh
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import Bu2KSh as B2KSh
confB2KSh = Bu2KShConf("Bu2KSh",B2KSh['CONFIG'])
for l in confB2KSh.lines():
  if l.name() in B2KSh['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in B2KSh['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingHb2Charged2Body import Hb2Charged2BodyLines
#from StrippingSelections.StrippingHb2Charged2Body import default_config as Hb2Charged2Body
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import Hb2Charged2Body as Hb2Charged2Body
confBhh          = Hb2Charged2BodyLines("Hb2Charged2Body", Hb2Charged2Body['CONFIG'])
for l in confBhh.lines():
  if l.name() in Hb2Charged2Body['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in Hb2Charged2Body['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingB2HHBDT import B2HHBDTLines
#from StrippingSelections.StrippingB2HHBDT import default_config as B2HHBDT
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import B2HHBDT as B2HHBDT
confBhhBDT          = B2HHBDTLines("B2HHBDT", B2HHBDT['CONFIG'])
for l in confBhhBDT.lines():
  if l.name() in B2HHBDT['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in B2HHBDT['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingD2HHBDT import D2HHBDTLines
#from StrippingSelections.StrippingD2HHBDT import default_config as D2HHBDT
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import D2HHBDT as D2HHBDT
confDhhBDT          = D2HHBDTLines("D2HHBDT", D2HHBDT['CONFIG'])
for l in confDhhBDT.lines():
  if l.name() in D2HHBDT['STREAMS']['Charm']:
    bhadronLines.append(l)


from StrippingSelections.StrippingB2hhLTUnbiased import StrippingB2hhLTUnbiasedConf
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import B2hhLTUnbiased, B2hhLTUnbiasedDetached
confBhhLTUnbiased = StrippingB2hhLTUnbiasedConf("B2hhLTUnbiased",B2hhLTUnbiased['CONFIG'])
confBhhLTUnbiasedDetached = StrippingB2hhLTUnbiasedConf("B2hhLTUnbiasedDetached",B2hhLTUnbiasedDetached['CONFIG'])
for l in confBhhLTUnbiased.lines():
  if l.name() in B2hhLTUnbiased['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in B2hhLTUnbiased['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)
for l in confBhhLTUnbiasedDetached.lines():
  if l.name() in B2hhLTUnbiasedDetached['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in B2hhLTUnbiasedDetached['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingBu2hhh import Bu2hhhBuilder
#from StrippingSelections.StrippingBu2hhh import config_params as B2hhh
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import B2hhh as B2hhh
confB2hhh = Bu2hhhBuilder("B2hhh",B2hhh['CONFIG'])
for l in confB2hhh.lines():
  if l.name() in B2hhh['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in B2hhh['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

#from StrippingSelections.StrippingD02KPiPi0 import StrippingD02KPiPi0Conf
#from StrippingSettings.Stripping20r1.LineConfigDictionaries_Calib import D02KPiPi0
#confD02KPiPi0 = StrippingD02KPiPi0Conf(D02KPiPi0['BUILDERTYPE'],D02KPiPi0['CONFIG'])
#stream.appendLines(confD02KPiPi0.lines())

from StrippingSelections.StrippingB2HHPi0 import StrippingB2HHPi0Conf
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import B2HHPi0
confB2HHPI0 = StrippingB2HHPi0Conf("B2HHPi0",B2HHPi0['CONFIG'])
for l in confB2HHPI0.lines():
  if l.name() in B2HHPi0['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in B2HHPi0['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingB2TwoBaryons import B2TwoBaryonLines
#from StrippingSelections.StrippingB2TwoBaryons import default_config as B2PPbar
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import B2PPbar as B2PPbar
confB2PPbar          = B2TwoBaryonLines("B2PPbar", B2PPbar['CONFIG'])
for l in confB2PPbar.lines():
  if l.name() in B2PPbar['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in B2PPbar['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)


from StrippingSelections.StrippingB2KShh import B2KShhConf
#from StrippingSelections.StrippingB2KShh import default_config as B2KShh
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import B2KShh as B2KShh
confB2KShh = B2KShhConf("B2KShh",B2KShh['CONFIG'])
for l in confB2KShh.lines():
  if l.name() in B2KShh['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in B2KShh['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingLb2V0hh import Lb2V0hhConf
#from StrippingSelections.StrippingLb2V0hh import default_config as Lb2V0hh
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import Lb2V0hh as Lb2V0hh
confLb2V0hh = Lb2V0hhConf("Lb2V0hh",Lb2V0hh['CONFIG'])
for l in confLb2V0hh.lines():
  if l.name() in Lb2V0hh['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in Lb2V0hh['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingB2Kphiphi import B2KPhiPhiConf
#from StrippingSelections.StrippingB2Kphiphi import config_params as B2KPhiPhi
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import B2KPhiPhi as B2KPhiPhi
confB2Kphiphi = B2KPhiPhiConf("B2KPhiPhi",B2KPhiPhi['CONFIG'])
for l in confB2Kphiphi.lines():
  if l.name() in B2KPhiPhi['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in B2KPhiPhi['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingB2CharmlessQuasi2Body import B2Quasi2BodyConf
#from StrippingSelections.StrippingB2CharmlessQuasi2Body import config_params as BetaSQ2B
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import BetaSQ2B as BetaSQ2B
confB2CharmlessQuasi2Body = B2Quasi2BodyConf("BetaSQ2B",BetaSQ2B['CONFIG'])
for l in confB2CharmlessQuasi2Body.lines():
  if l.name() in BetaSQ2B['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in BetaSQ2B['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingBs2PhiKst0 import StrippingBs2PhiKstConf
#from StrippingSelections.StrippingBs2PhiKst0 import confdict as Bs2PhiKst
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import BetaSBs2PhiKst as Bs2PhiKst
confBs2PhiKst = StrippingBs2PhiKstConf("BetaSBs2PhiKst",Bs2PhiKst['CONFIG'])
for l in confBs2PhiKst.lines():
  if l.name() in Bs2PhiKst['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in Bs2PhiKst['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingBs2Kst0Kst0 import StrippingBs2KstKstConf
#from StrippingSelections.StrippingBs2Kst0Kst0 import __config_default__ as BetaSBs2KstKst
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import BetaSBs2KstKst as BetaSBs2KstKst
confBs2KstKst = StrippingBs2KstKstConf("BetaSBs2KstKst",BetaSBs2KstKst['CONFIG'])
for l in confBs2KstKst.lines():
  if l.name() in BetaSBs2KstKst['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in BetaSBs2KstKst['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)


from StrippingSelections.StrippingBs2Kst_0Kst_0 import StrippingBs2Kst_0Kst_0Conf
#from StrippingSelections.StrippingBs2Kst_0Kst_0 import __config_default__ as BetaSBs2K0stK0st
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import BetaSBs2K0stK0st as BetaSBs2K0stK0st
confBs2Kst_0Kst_0 = StrippingBs2Kst_0Kst_0Conf("BetaSBs2K0stK0st",BetaSBs2K0stK0st['CONFIG'])
for l in confBs2Kst_0Kst_0.lines():
  if l.name() in BetaSBs2K0stK0st['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in BetaSBs2K0stK0st['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingBs2Q2B import Bs2Q2Body4piConf
#from StrippingSelections.StrippingBs2Q2B import config_params as BetaSBs2Q2Body4pi
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import BetaSBs2Q2Body4pi as BetaSBs2Q2Body4pi
confBs2Q2Body = Bs2Q2Body4piConf("BetaSBs2Q2Body4pi",BetaSBs2Q2Body4pi['CONFIG'])
for l in confBs2Q2Body.lines():
  if l.name() in BetaSBs2Q2Body4pi['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in BetaSBs2Q2Body4pi['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingBs2KKhh import BsPhiRhoConf
#from StrippingSelections.StrippingBs2KKhh import config_params as BsPhiRho
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import BsPhiRho as BsPhiRho
confBsPhiRho = BsPhiRhoConf("BsPhiRho",BsPhiRho['CONFIG'])
for l in confBsPhiRho.lines():
  if l.name() in BsPhiRho['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in BsPhiRho['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingB2FourKHigh import Bs2Q2Body4KConf
#from StrippingSelections.StrippingB2FourKHigh import config_params as Bs2Q2Body4K
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import Bs2Q2Body4K as Bs2Q2Body4K
confBs2Q2Body4K = Bs2Q2Body4KConf("Bs2Q2Body4K",Bs2Q2Body4K['CONFIG'])
for l in confBs2Q2Body4K.lines():
  if l.name() in Bs2Q2Body4K['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in Bs2Q2Body4K['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingBs2PhiPhi import StrippingBs2PhiPhiConf
#from StrippingSelections.StrippingBs2PhiPhi import config_params as BetaSBs2PhiPhi
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import BetaSBs2PhiPhi as BetaSBs2PhiPhi
confBs2PhiPhi = StrippingBs2PhiPhiConf("BetaSBs2PhiPhi",BetaSBs2PhiPhi['CONFIG'])
for l in confBs2PhiPhi.lines():
  if l.name() in BetaSBs2PhiPhi['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in BetaSBs2PhiPhi['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingBs2KSKS import Bs2KSKSConf
#from StrippingSelections.StrippingBs2KSKS import default_config as Bs2KSKS
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import Bs2KSKS as Bs2KSKS
confBs2KSKS = Bs2KSKSConf("Bs2KSKS",Bs2KSKS['CONFIG'])
for l in confBs2KSKS.lines():
  if l.name() in Bs2KSKS['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in Bs2KSKS['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

from StrippingSelections.StrippingB2XEta import B2XEtaConf
#from StrippingSelections.StrippingB2XEta import default_config as B2XEta
from StrippingSettings.Stripping20r1.LineConfigDictionaries_Charmless import B2XEta as B2XEta
confB2XEta = B2XEtaConf("B2XEta",B2XEta['CONFIG'])
for l in confB2XEta.lines():
  if l.name() in B2XEta['STREAMS']['Bhadron']:
    bhadronLines.append(l)
  if l.name() in B2XEta['STREAMS']['BhadronCompleteEvent']:
    bhadroncompleteeventLines.append(l)

streamBhadron.appendLines(bhadronLines)
streamBhadronCompleteEvent.appendLines(bhadroncompleteeventLines)

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

sc = StrippingConf( Streams = [ streamBhadron, streamBhadronCompleteEvent ],
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
DaVinci().DataType  = "2011"
DaVinci().InputType = "DST"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# database
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121025"

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_2011Data_MagDn.py") # MagDown sample
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_2011Data_MagUp.py") # MagUp sample

