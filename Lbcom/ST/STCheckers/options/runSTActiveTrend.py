#-------------------------------------------------------------------------------
# Run algorithm to get active fraction of ST as function of time
#
# Mark Tobin, August 2011
#-------------------------------------------------------------------------------
from Gaudi.Configuration import *
from Configurables import LHCbApp

MainSeq = GaudiSequencer( 'MainSeq' )
MainSeq.MeasureTime = True

appMgr = ApplicationMgr()
appMgr.EvtSel= "NONE"
appMgr.TopAlg.append( MainSeq )

from Configurables import LHCbApp
from Configurables import EventClockSvc

EventDataSvc().RootCLID = 1
EventDataSvc().EnableFaultHandler = True
EventDataSvc().ForceLeaves = True
EventPersistencySvc().CnvServices = [ 'LHCb::RawDataCnvSvc' ]

AuditorSvc().Auditors = [ 'ChronoAuditor' ]

# DataBase - leave blank for latest tags
LHCbApp().DDDBtag = ''
LHCbApp().CondDBtag = ''
LHCbApp().OutputLevel = 3
MessageSvc().Format = "% F%30W%S%7W%R%T %0W%M"

import time
StartDate="2008-06-01 00:00:00"
StartTime = time.mktime(time.strptime(StartDate, "%Y-%m-%d %H:%M:%S"))
Date2="2008-06-02 00:00:00"
Time2 = time.mktime(time.strptime(Date2, "%Y-%m-%d %H:%M:%S"))
TimeStep = Time2-StartTime

EndTime = time.mktime(time.localtime())

print "Start Time", int(StartTime)*1000000000
print "End Time", int(EndTime)*1000000000
print "Time steps", int(TimeStep)*1000000000


nSteps = (EndTime-StartTime)/TimeStep
print "This time range requires",int(nSteps),"steps"

# have to convert time from s to ns since the epoch
ApplicationMgr().EvtMax = int(nSteps)
ApplicationMgr().OutputLevel = DEBUG
EventClockSvc().EventTimeDecoder = 'FakeEventTime'
EventClockSvc().InitialTime = int(StartTime)*1000000000
from Configurables import FakeEventTime
EventClockSvc().addTool( FakeEventTime )
# required to add step otherwise 2nd event has the same time in the EventClockSvc as the initial time
EventClockSvc().FakeEventTime.StartTime = int(StartTime)*1000000000 + int(TimeStep)*1000000000 
EventClockSvc().FakeEventTime.TimeStep = int(TimeStep)*1000000000
EventClockSvc().OutputLevel = 2

from Configurables import STPerformanceMonitor, ST__STActiveFraction
ttFraction = ST__STActiveFraction("TTActiveFraction")
ttFraction.DetType="TT"
ttFraction.StartTime = int(StartTime)*1000000000
ttFraction.TimeStep = int(TimeStep)*1000000000
ttFraction.Steps = int(nSteps)
MainSeq.Members.append( ttFraction )
itFraction = ST__STActiveFraction("ITActiveFraction")
itFraction.DetType="IT"
itFraction.StartTime = int(StartTime)*1000000000
itFraction.TimeStep = int(TimeStep)*1000000000
itFraction.Steps = int(nSteps)
MainSeq.Members.append( itFraction )

ApplicationMgr().HistogramPersistency = 'ROOT'
HistogramPersistencySvc().OutputFile = 'status.root'
