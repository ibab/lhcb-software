#-------------------------------------------------------------------------------
# Run ZS monitoring for TT and IT
#
# Mark Tobin, June 2011
#-------------------------------------------------------------------------------
from Gaudi.Configuration import *
from Configurables import LHCbApp
#from Configurables import *

#importOptions( 'DecodeRawEvent.py' )

## Import default LHCb application options
#importOptions( '$STDOPTS/LHCbApplication.opts' )

mainSeq = GaudiSequencer( 'MainSeq' )
mainSeq.MeasureTime = True

appMgr = ApplicationMgr()
#appMgr.EvtMax = 100000
appMgr.EvtMax = 1
#appMgr.EvtMax = -1
#appMgr.EvtSel= "NONE"
appMgr.TopAlg.append( mainSeq )

#appMgr.HistogramPersistency = 'ROOT'
#HistogramPersistencySvc().OutputFile = 'ROOT-250V-50481.root'
#HistogramPersistencySvc().OutputFile = 'ROOT.root'

from Configurables import LHCbApp
from Configurables import EventClockSvc

EventDataSvc().RootCLID = 1
EventDataSvc().EnableFaultHandler = True
EventDataSvc().ForceLeaves = True
EventPersistencySvc().CnvServices = [ 'LHCb::RawDataCnvSvc' ]
EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'

AuditorSvc().Auditors = [ 'ChronoAuditor' ]

# DataBase - leave blank for latest tags
LHCbApp().DDDBtag = ''
LHCbApp().CondDBtag = ''
#LHCbApp().DataType = '2010'
LHCbApp().OutputLevel = 3
#LHCbApp().SkipEvents = 2500
MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"

from Configurables import STPerformanceMonitor

appMgr.TopAlg.append( STPerformanceMonitor("TTPerformanceMonitor", DetType="TT") )
appMgr.TopAlg.append( STPerformanceMonitor("ITPerformanceMonitor", DetType="IT") )

ApplicationMgr().HistogramPersistency = 'ROOT'
HistogramPersistencySvc().OutputFile = 'statusNOW.root'
