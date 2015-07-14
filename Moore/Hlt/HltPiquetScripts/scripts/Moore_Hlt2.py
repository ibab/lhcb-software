import os
import Gaudi.Configuration
from Moore.Configuration import Moore

Moore().ThresholdSettings = 'Physics_draftEM2015'
Moore().ForceSingleL0Configuration = True
Moore().OutputLevel = 3
Moore().EvtMax = 10000
Moore().EnableTimer = True
Moore().CondDBtag = 'cond-20150601'
Moore().DDDBtag = 'dddb-20150526'
Moore().DataType = '2015'
Moore().CheckOdin = False

from Configurables import EventSelector
EventSelector().PrintFreq = 100

Moore().Split = 'Hlt2'

base_dir = '/localdisk/hlt1_KEEP'
runnrs = [153759]
Moore().inputFiles = [os.path.join(base_dir, f) for f in os.listdir(base_dir) if any((str(r) in f for r in runnrs))]
# Moore().inputFiles = ['Moore_Hlt1.mdf']

Moore().outputFile = 'Moore_Hlt2.mdf'

# CondDB for online
from Configurables import CondDB
CondDB().UseDBSnapshot = True
CondDB().DBSnapshotDirectory = "/group/online/hlt/conditions"
CondDB().EnableRunChangeHandler = False
CondDB().Tags["ONLINE"] = 'fake'
CondDB().setProp("IgnoreHeartBeat", True)

# GaudiPython
from GaudiPython.Bindings import AppMgr
gaudi = AppMgr()
gaudi.initialize()
gaudi.run(2000)
