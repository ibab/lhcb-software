# Test all B2CC lines.
# Carlos Vazquez Sierra (carlos.vazquez@cern.ch)

from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

#from StrippingSelections.Utils import buildStreams, lineBuilder
#from StrippingSettings.Utils import *
from StrippingConf.Configuration import StrippingConf, StrippingStream
from StrippingSelections import buildersConf
from StrippingSelections.Utils import buildStreams, lineBuilder
from StrippingSettings.Utils import *

current_stripping="stripping21"
db = strippingConfiguration( current_stripping )
all_lines = groupFromDBase( db,'WGs')
lines_in_myWG=all_lines["B2CC"]
print lines_in_myWG
print "here are the lines I'm testing"
for ln in lines_in_myWG:
  print ln, db[ln]['STREAMS']

from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test_all_B2CC")
for config in lines_in_myWG:
  stream.appendLines( lineBuilder(current_stripping, config).lines() )

# Now build the stream
#from StrippingConf.StrippingStream import StrippingStream
#streamBetaS = StrippingStream("Test_B2JpsiXforBeta_s")
#streamPsi2S = StrippingStream("Test_B2Psi2SX")
#streamPsi2SMuMu = StrippingStream("Test_B2Psi2SMuMu")
#streamJpsieePhi = StrippingStream("Test_Bs2JpsieePhi")
#streamCharmonium = StrippingStream("Test_B2CharmoniumX_6H")
#streamEtacPhiBDT = StrippingStream("Test_Bs2EtacPhiBDT")

# Append the lines
#from StrippingSelections import buildersConf
#confs = buildersConf()
#from StrippingSelections.Utils import lineBuilder

#builderBetaS = lineBuilder(confs,'BetaS') #StrippingB2JpsiXforBeta_s.py
#builderPsi2S = lineBuilder(confs, 'BetaSPsi2S') #StrippingB2Psi2SX.py
#builderPsi2SMuMu = lineBuilder(confs, 'BetaSPsi2SMuMu') #StrippingB2Psi2SXMuMu.py
#builderBs2JpsieePhi = lineBuilder(confs, 'BetaSBs2JpsieePhi') #StrippingBs2JpsieePhi.py
#builderB2CharmoniumX = lineBuilder(confs, 'B2CharmoniumX_6H') #StrippingB2CharmoniumX_6H.py
#builderEtacPhiBDT = lineBuilder(confs, 'Bs2EtacPhiBDT') #StrippingBs2EtacPhiBDT.py

#streamBetaS.appendLines( builderBetaS.lines() )
#streamPsi2S.appendLines( builderPsi2S.lines() )
#streamPsi2SMuMu.appendLines( builderPsi2SMuMu.lines() )
#streamJpsieePhi.appendLines( builderBs2JpsieePhi.lines() )
#streamCharmonium.appendLines( builderB2CharmoniumX.lines() )
#streamEtacPhiBDT.appendLines( builderEtacPhiBDT.lines() )

# Standard configuration of Stripping, do NOT change them
from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

#sc = StrippingConf( Streams = [ streamBetaS, streamPsi2S, streamPsi2SMuMu, streamJpsieePhi, streamCharmonium, streamEtacPhiBDT ],
sc = StrippingConf ( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents,
                    TESPrefix = 'Strip',
                    Verbose = True
                    )

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 100000
DaVinci().PrintFreq = 10000
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType  = "2012"
#DaVinci().DataType = "2011"
DaVinci().InputType = "DST"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# database
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008" # 2012
#DaVinci().CondDBtag = "cond-20121025" # 2011

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py") # 2012
#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_2011Data_MagDn_New.py") # MagDown sample, 2011
#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_2011Data_MagUp_New.py") # MagUp sample, 2011
