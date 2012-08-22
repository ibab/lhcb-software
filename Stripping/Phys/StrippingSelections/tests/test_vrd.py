# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

#ok
from StrippingSelections.StrippingTau23MuLines import Tau23MuLinesConf as b0
lb0 = b0('t23m',b0.config_default)
stream.appendLines( lb0.lines() )

#ok
from StrippingSelections.StrippingBs2MuMuLines import Bs2MuMuLinesConf as b1
lb1 = b1('bmm',b1.config_default)
stream.appendLines( lb1.lines() )

#ok
from StrippingSelections.StrippingVeryDetachedJPsiLines import VDetJPsiLinesConf as b2
lb2 = b2('bmm',b2.config_default)
stream.appendLines( lb2.lines() )

#ok
from StrippingSelections.StrippingB2MuMuMuMuLines import B2MuMuMuMuLinesConf as b3
lb3 = b3('b24m',b3.config_default)
stream.appendLines( lb3.lines() )

#ok
from StrippingSelections.StrippingTriMuons import  StrippingTriMuonsConf  as b4
from StrippingSelections.StrippingTriMuons import  confdict as c4
lb4 = b4('trim',c4)
stream.appendLines( lb4.lines() )

#ok
from StrippingSelections.StrippingHyperCPX import  StrippingHyperCPXConf as b5
from StrippingSelections.StrippingHyperCPX import  default_config as c5
lb5 = b5('trim',c5)
stream.appendLines( lb5.lines() )

#ok
from StrippingSelections.StrippingLFVLines import LFVLinesConf as b6
lb6 = b6('lfv',b6.config_default)
stream.appendLines( lb6.lines() )

#ok
from StrippingSelections.StrippingInflaton2MuMuLine import StrippingInflaton2MuMuConf as b7
lb7 = b7('inflaton',b7.config_default)
stream.appendLines( lb7.lines() )

#ok - go to mDST
from StrippingSelections.StrippingK0s2MuMuLines import K0s2MuMuLinesConf as b8
lb8 = b8('ksmm',b8.config_default)
stream.appendLines( lb8.lines() )


# Tau2pmumu goes out




#for line in lb1.lines():
#     print "AS DEBUG", line
#     print "AS DEBUG", line.name()











from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents,
                    TESPrefix = 'Strip'
                    )


MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

DaVinci().PrintFreq = 200
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 100000
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType  = "2012"
DaVinci().InputType = "DST"

# change the column size of timing table
#from Configurables import TimingAuditor, SequencerTimerTool
#TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
#TimingAuditor().TIMER.NameSize = 60

# database
#DaVinci().DDDBtag  = "head-20120413"
#DaVinci().CondDBtag = "head-20120413"
#
#from GaudiConf import IOHelper
#IOHelper().inputFiles([
#    'PFN:castor:/castor/cern.ch/user/j/jhe/Stripping18/111473_0000000110-FastVeloFix.dst'])


# database
DaVinci().DDDBtag  = "head-20120413"
DaVinci().CondDBtag = "head-20120420"

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco13a_Run114753_SDST.py")





