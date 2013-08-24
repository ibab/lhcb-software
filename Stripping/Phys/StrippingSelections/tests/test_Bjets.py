# $Id: $
# Test your line(s) of the stripping
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf

from GaudiKernel import SystemOfUnits as units

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

# Append your line
from StrippingSelections.StrippingBjets import BJetsLinesConf
from StrippingSelections.StrippingBjets import default_config as _config

_quickconfig = { "2B4jet" : { "PTmin" : 8*units.GeV ,  "prescale" : 1 } , 'my2TOPO' : {'ANGLE_MIN': (2/57.),'M_MIN':100000,'DPHI_MIN':0}   }

DiMuonBuilder = BJetsLinesConf( name="StrippingBjets", config=_config )
stream.appendLines( DiMuonBuilder.lines() )


# Standard configuration of Stripping, do NOT change them
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
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008"

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")
