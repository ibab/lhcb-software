from Gaudi.Configuration import *
from DetDescSvc.DetDescSvcConf import *

ec = EventClockSvc()
ec.EventTimeDecoder = FakeEventTime().name()
ec.FakeEventTime = FakeEventTime()

importOptions("$GAUSSOPTS/v200601.opts")
