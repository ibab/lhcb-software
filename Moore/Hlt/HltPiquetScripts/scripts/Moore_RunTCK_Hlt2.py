import os
import Gaudi.Configuration
from Moore.Configuration import Moore

Moore().UseTCK = True
Moore().InitialTCK = '0x00f7014e'
Moore().ForceSingleL0Configuration = True
Moore().OutputLevel = 4
Moore().EvtMax = 1000
Moore().EnableTimer = True
Moore().CondDBtag = 'cond-20150625'
Moore().DDDBtag = 'dddb-20150526'
Moore().DataType = '2015'
Moore().CheckOdin = False

from Configurables import EventSelector
EventSelector().PrintFreq = 100

Moore().Split = 'Hlt2'

from Configurables import CondDB
CondDB().Simulation = False
CondDB().UseDBSnapshot = True
CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
CondDB().Tags["ONLINE"] = 'fake'
CondDB().Online = True
CondDB().EnableRunChangeHandler = True

import sys
try:
    import AllHlt1
except ImportError:
    rd = '/group/online/hlt/conditions/RunChangeHandler'
    sys.path.append(rd)
    import AllHlt1

import AllHlt1
CondDB().RunChangeHandlerConditions = AllHlt1.ConditionMap

from Configurables import MagneticFieldSvc
MagneticFieldSvc().UseSetCurrent = True

# base_dir = '/localdisk/hlt1'
# runnrs = [154838]
# Moore().inputFiles = [os.path.join(base_dir, f) for f in os.listdir(base_dir) if any((str(r) in f for r in runnrs))]
# Moore().inputFiles = ['/localdisk/hlt1/Run_0154503_20150610-233639.hlta0901.mdf']
Moore().inputFiles = ['/net/hlta0503/localdisk/hlt1/Run_0156687_20150703-151254.hlta0503.mdf']
# Moore().inputFiles = ['Moore_Hlt1.mdf']
Moore().outputFile = 'Moore_Hlt2.mdf'

# GaudiPython
# from GaudiPython.Bindings import AppMgr
# gaudi = AppMgr()
# gaudi.initialize()
# gaudi.run(1)
