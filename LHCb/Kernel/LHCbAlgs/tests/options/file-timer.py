from Configurables import LHCbApp
from Gaudi.Configuration import *

LHCbApp().EvtMax=100


from PRConfig import TestFileDB
TestFileDB.test_file_db["2012_raw_default"].run()

from Configurables import createODIN, EventCountAlg, PostScaler
ApplicationMgr().TopAlg+=[createODIN(),EventCountAlg(),PostScaler(),GaudiSequencer("WAAAATUP")]

from Configurables import LHCbTimingAuditor, LHCbSequencerTimerTool, TimingAuditor, AuditorSvc

LHCbTimingAuditor().addTool(LHCbSequencerTimerTool,name="TIMER")
LHCbTimingAuditor().TIMER.SummaryFile="timing.dat"
LHCbTimingAuditor().TIMER.OutputLevel=DEBUG
LHCbTimingAuditor().TIMER.NameSize=2
ApplicationMgr().ExtSvc.append( 'AuditorSvc' )
AuditorSvc().Auditors.append(LHCbTimingAuditor())
LHCbTimingAuditor().Enable=True
LHCbTimingAuditor().OutputLevel=VERBOSE
AuditorSvc().Auditors.append(TimingAuditor())

TimingAuditor().Enable=True

from Configurables import IODataManager
IODataManager().DisablePFNWarning=True
