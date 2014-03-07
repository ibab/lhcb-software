from Gaudi.Configuration import *
from Configurables import DetCondTest__TestConditionAlg as TestAlg
from Configurables import LHCbAlgsTests__TestTimeDecoderOdin as TimeDecoder
from Configurables import ( EventClockSvc, RunChangeHandlerSvc, XmlCnvSvc )
from Configurables import LoadDDDB, DDDBConf, CondDB

import sys
sys.path.append('.')

from rchoption_withhlt2 import calibconds

DDDBConf() # Configure the detector description
CondDB(IgnoreHeartBeat = True)

EventClockSvc().addTool(TimeDecoder,"EventTimeDecoder")


testAlg = TestAlg("TestCond")
testAlg.Conditions = []
testAlg.Conditions += calibconds # Load the conditions in rch for testing

app = ApplicationMgr()
app.TopAlg = [ TestAlg("TestCond") ]

app.EvtSel = "NONE"
app.EvtMax = 2

EventDataSvc(ForceLeaves = True)
