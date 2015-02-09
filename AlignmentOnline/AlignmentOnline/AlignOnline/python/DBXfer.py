from Gaudi.Configuration import *
from Configurables import LHCbAlgsTests__TestTimeDecoderOdin as TimeDecoder
from Configurables import (RunChangeHandlerSvc, DBXferAlg ,XmlCnvSvc, DDDBConf, CondDB, EventClockSvc, FakeEventTime)
import RunOption
import All
ecs = EventClockSvc()
ecs.InitialTime = RunOption.RunStartTime*1000000000
ecs.addTool(FakeEventTime,"EventTimeDecoder")
ecs.EventTimeDecoder.StartTime = ecs.InitialTime
ecs.EventTimeDecoder.TimeStep = 10

#xmlCnvSvc = XmlCnvSvc(AllowGenericConversion = True)
DDDBConf()
#detDataSvc = DetectorDataSvc()
#DetectorPersistencySvc( CnvServices = [ xmlCnvSvc ] )
cdb = CondDB()
cdb.RunChangeHandlerConditions=All.ConditionMap
cdb.EnableRunChangeHandler = True
cdb.UseDBSnapshot = True
cdb.Tags = { "DDDB" : RunOption.DDDBTag,
                "LHCBCOND" : RunOption.CondDbTag,
                "ONLINE" : 'fake'}
cdb.IgnoreHeartBeat = True
dbxalg = DBXferAlg()
dbxalg.RunNumber = RunOption.RunNumber
dbxalg.RunStartTime = RunOption.RunStartTime*1000000000
dbxalg.OnlineXmlDir = RunOption.OutputDirectory
app = ApplicationMgr()
app.TopAlg = [ dbxalg ]
#app.ExtSvc += [ detDataSvc]#, rch ]
app.EvtSel = "NONE"
app.EvtMax = 1
app.OutputLevel = INFO
# from Configurables import XmlParserSvc
# RunChangeHandlerSvc(OutputLevel=VERBOSE)
# MessageSvc(OutputLevel=1)
EventDataSvc(ForceLeaves = True)
from GaudiPython.Bindings import AppMgr
Gaudi=AppMgr()
Gaudi.run(1)