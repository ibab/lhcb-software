#!/usr/bin/env python

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to < 3. We already used that
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }


# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

from StrippingSelections.StrippingBeauty2XGamma import Beauty2XGamma
from StrippingSelections.StrippingBeauty2XGamma import config

lb = Beauty2XGamma("Beauty2XGamma_TestStr18", config)
stream.appendLines(lb.lines())

from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

#from Configurables import AlgorithmCorrelationsAlg
#ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

DaVinci().PrintFreq = 2000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 20000
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
#DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType  = "2012"
DaVinci().InputType = "DST"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60

# database
DaVinci().DDDBtag  = "head-20120413"
DaVinci().CondDBtag = "cond-20120710"

#from GaudiConf import IOHelper
#IOHelper().inputFiles([
#    'PFN:castor:/castor/cern.ch/user/j/jhe/Stripping18/111473_0000000110-FastVeloFix.dst'])

importOptions('$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py')

#OLD
#from Configurables import CondDB
#DaVinci().DDDBtag  = "head-20110914"
#DaVinci().CondDBtag = "head-20110914"

#DaVinci().PrintFreq = 2000
#DaVinci().HistogramFile = 'DV_stripping_histos.root'
#DaVinci().EvtMax = 300000
#DaVinci().appendToMainSequence( [ sc.sequence() ] )
#DaVinci().appendToMainSequence( [ sr ] )
###DaVinci().appendToMainSequence( [ ac ] )
#DaVinci().DataType  = "2011"
#DaVinci().InputType = "SDST"

## TEST DATA
#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco12_Run97120_SDST.py")
## MC10 K*GAMMA SIGNAL
#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/KStarGamma_MC10.py")
## MC10 PHI GAMMA SIGNAL
#importOptions("$STRIPPINGSELECTIONSROOT/tests/data/PhiGamma_MC10.py")
